#include <string>
#include <queue>
#include <fstream>
#include "Image.hpp"
#include "Label.hpp"
#include "ImageButton.hpp"
#include "PlayScene.hpp"
#include "LOG.hpp"
#include "Enemy.hpp"
#include "TurretButton.hpp"
#include "Plane.hpp"
#include "DirtyEffect.hpp"
#include "AudioHelper.hpp"

bool PlayScene::gender = false;
bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER };
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    // TODO 5 (1/2): There's a bug in this file, which crashes the game when you win. Try to find it.
    // TODO 5 (2/2): There's a cheat code in this file. Try to find it.
    
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    UIGroup->AddNewObject(imgTarget);
    AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Update(float deltaTime) {
    // Fake speed up.
    deltaTime *= SpeedMult;
    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    // Reference: Bullet-Through-Paper
    if (deltaTime < 3) {
        IScene::Update(deltaTime);
    } else if (deltaTime < 6) {
        IScene::Update(deltaTime / 2);
        IScene::Update(deltaTime / 2);
    } else if (deltaTime < 9) {
        IScene::Update(deltaTime / 3);
        IScene::Update(deltaTime / 3);
        IScene::Update(deltaTime / 3);
    } else {
        IScene::Update(deltaTime / 4);
        IScene::Update(deltaTime / 4);
        IScene::Update(deltaTime / 4);
        IScene::Update(deltaTime / 4);
    }
    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    }
    // Check if we should create new enemy.
    ticks += deltaTime;
    if (enemyWaveData.empty()) {
        if (EnemyGroup->GetObjects().empty()) {
            // Free resources.
            /*
             delete TileMapGroup;
             delete GroundEffectGroup;
             delete DebugIndicatorGroup;
             delete TowerGroup;
             delete EnemyGroup;
             delete BulletGroup;
             delete EffectGroup;
             delete UIGroup;
             delete imgTarget;
             */
            // Win.
            Engine::GameEngine::GetInstance().ChangeScene("win");
        }
        return;
    }
    auto current = enemyWaveData.front();
    if (ticks < current.second)
        return;
    ticks -= current.second;
    enemyWaveData.pop_front();
    const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
    Enemy* enemy;
    
    enemy->UpdatePath(mapDistance);
    // Compensate the time lost.
    enemy->Update(ticks);
}
void PlayScene::Draw() const {
    
    ALLEGRO_BITMAP* StartImg = al_load_bitmap("resources/images/play/playscene.png");
    al_draw_bitmap(StartImg, 0, 0, 0);
    
    IScene::Draw();
    
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && preview) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1) {
        if (mapState[y][x] != TILE_OCCUPIED) {
            if (!preview)
                return;
            // Check if valid.
            if (!CheckSpaceValid(x, y)) {
                Engine::Sprite* sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            EarnMoney(-preview->GetPrice());
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            TowerGroup->AddNewObject(preview);
            // Remove Preview.
            preview = nullptr;
            
            mapState[y][x] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        }
    }
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    }
    else {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > code.size())
            keyStrokes.pop_front();
        if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size()) {
            auto it = keyStrokes.begin();
            for (int c : code) {
                if (*it != c && (c == ALLEGRO_KEYMOD_SHIFT && *it != ALLEGRO_KEY_LSHIFT && *it != ALLEGRO_KEY_RSHIFT))
                    return;
                ++it;
            }
            EffectGroup->AddNewObject(new Plane());
            money += 10000;
        }
    }
    if (keyCode == ALLEGRO_KEY_Q) {
        // Hotkey for MachineGunTurret.
        UIBtnClicked(0);
    }
    else if (keyCode == ALLEGRO_KEY_W) {
        // Hotkey for LaserTurret.
        UIBtnClicked(1);
    }
    else if (keyCode == ALLEGRO_KEY_E) {
        // Hotkey for MissileTurret.
        UIBtnClicked(2);
    }
    else if (keyCode == ALLEGRO_KEY_R) {
        // Hotkey for NewTurret.
        UIBtnClicked(3);
    }
    // TODO 2 (5/8): Make the R key to create the 4th turret.
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    }
}
void PlayScene::Hit() {
    lives--;
    UILives->Text = std::string("Life ") + std::to_string(lives);
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}

void PlayScene::ReadEnemyWave() {
    std::string filename = std::string("resources/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat) {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::ConstructUI() {
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
    UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
    UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88));
    
}

void PlayScene::UIBtnClicked(int id) {
    if (preview)
        UIGroup->RemoveObject(preview->GetObjectIterator());
   
    // TODO 2 (4/8): On callback, create the 4th tower.
    if (!preview)
        return;
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[0][0] == -1)
        return false;
    for (auto& it : EnemyGroup->GetObjects()) {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0 || pnt.x >= MapWidth || pnt.y < 0 || pnt.y >= MapHeight)
            continue;
        if (map[pnt.y][pnt.x] == -1)
            return false;
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto& it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();
        
        if( p.y + 1 <= MapHeight - 1 && p.y + 1 >= 0){
            if(mapState[p.y +1][p.x] ==TILE_DIRT && map[p.y +1][p.x]== -1){
                //mapState[p.y + 1][p.x + 1] = TILE_FLOOR;
                map[p.y + 1][p.x] = map[p.y][p.x] + 1;
                que.push(Engine::Point(p.x,p.y + 1));
            }
        }
        if( p.y -1 <= MapHeight - 1 && p.y -1 >= 0){
            if(mapState[p.y -1][p.x] ==TILE_DIRT&& map[p.y -1][p.x] ==-1){
                map[p.y - 1][p.x] = map[p.y][p.x] + 1;
                que.push(Engine::Point(p.x,p.y - 1));
                std:: cout <<" "<< p.y - 1 <<" | " << p.x<< std:: endl;
            }
            
            
        }
        if( p.x -1 <= MapWidth - 1 && p.x -1>=0){
            if(mapState[p.y][p.x - 1] ==TILE_DIRT&& map[p.y][p.x - 1] ==-1){
                map[p.y][p.x - 1] = map[p.y][p.x] + 1;
                que.push(Engine::Point(p.x - 1,p.y));
                std:: cout <<" "<< p.y  <<" | " << p.x - 1 << std:: endl;
            }
            
            
        }
        if( p.x + 1<= MapWidth - 1 && p.x+1 >=0){
            if(mapState[p.y][p.x + 1] ==TILE_DIRT && map[p.y][p.x + 1] ==-1){
                map[p.y][p.x + 1] = map[p.y][p.x] + 1;
                que.push(Engine::Point(p.x + 1,p.y));
                std:: cout <<" "<< p.y <<" | " << p.x + 1 << std:: endl;
            }
            
            
        }
        
        
    }
    
    // TODO 3 (1/1): Implement a BFS starting from the most right-bottom block in the map.
    //               For each step you should assign the corresponding distance to the most right-bottom block.
    //               mapState[y][x] is TILE_DIRT if it is empty.
    
    
    
    
    
    //throw std::logic_error("CalculateBFSDistance is not implemented yet.");
    
    for (int i=0; i< MapWidth; i++) {
        for (int j=0; j< MapHeight; j++) {
            std::cout << map[j][i] <<" ";
        }
        std::cout <<std::endl;
    }
    
    return map;
}
