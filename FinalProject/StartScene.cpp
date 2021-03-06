#include "StartScene.hpp"
#include "PlayerSelectScene.hpp"
#include "PlayScene.hpp"
#include "HowScene.hpp"
#include "AudioHelper.hpp"
#include "LOG.hpp"
#include "IScene.hpp"
#include "GameEngine.hpp"
#include "ImageButton.hpp"

void StartScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    
    //Engine::Image img("Start.png",0,0,800,600,400,300);
    //AddNewObject(new Engine::Image("Start.png",0,0,800,600,400,300));
    IsMute = 1;


    AddNewObject(new Engine::Label("Darkness", "Gore.ttf", 120, halfW, halfH-50, 255, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Press enter to start", "Gore.ttf", 30, halfW, halfH+50 , 255, 255, 255, 255, 0.5, 0.5));
    //buttom
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("start/question1.png", "start/question2.png", 680 , 400, 100, 100);//AGAIN
    btn->SetOnClickCallback(std::bind(&StartScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    
    
    btn = new Engine::ImageButton("start/unmute1.png", "start/unmute2.png", 680, 500, 100, 100);//EXIT
    btn->SetOnClickCallback(std::bind(&StartScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    
    mute1 = al_load_bitmap("resources/images/start/mute1.png");
    mute2 = al_load_bitmap("resources/images/start/mute2.png");
    // Not a safe way, however we only free while change scene, so it's fine.
    ///*
    bgmInstance = al_create_sample_instance(Engine::Resources::GetInstance().GetSample("start.ogg").get());
    al_set_sample_instance_playmode(bgmInstance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(bgmInstance, al_get_default_mixer());
    al_play_sample_instance(bgmInstance);
    //

    
}
void StartScene::BackOnClick(int stage) {
    // Change to select scene.
    //Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    
    //PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("stage-select"));
    HowScene* scene = dynamic_cast<HowScene*>(Engine::GameEngine::GetInstance().GetScene("how"));
    Engine::ImageButton* btn;
    //scene->MapId = stage;
    if(stage==1){
        scene->IsMute = IsMute;
        Engine::GameEngine::GetInstance().ChangeScene("how");
    }
    else if(stage==2){
        //int value = 0;
        if(IsMute==1){
            al_set_sample_instance_gain(bgmInstance, 0);
            AudioHelper::BGMVolume = 0;
            IsMute = 0;
            al_draw_bitmap(mute1, 680, 500, 0);
            
            btn = new Engine::ImageButton("start/mute1.png", "start/mute2.png", 680, 500, 100, 100);//mute
            btn->SetOnClickCallback(std::bind(&StartScene::BackOnClick, this, 2));
            AddNewControlObject(btn);
        }
        else{
            al_set_sample_instance_gain(bgmInstance, 1);
            AudioHelper::BGMVolume = 1;
            IsMute = 1;
            
            btn = new Engine::ImageButton("start/unmute1.png", "start/unmute2.png", 680, 500, 100, 100);//unmute
            btn->SetOnClickCallback(std::bind(&StartScene::BackOnClick, this, 2));
            AddNewControlObject(btn);
        }
            
    }
    
    
}

void StartScene::Draw() const{
    ALLEGRO_BITMAP* StartImg = al_load_bitmap("resources/images/start/Start.png");
    al_draw_bitmap(StartImg, 0, 0, 0);
    Group::Draw();
    
}

void StartScene::OnKeyDown(int keyCode){
    PlayerSelectScene* scene = dynamic_cast<PlayerSelectScene*>(Engine::GameEngine::GetInstance().GetScene("player-select"));
    if(keyCode==ALLEGRO_KEY_ENTER){
        scene->IsMute = IsMute;
        Engine::GameEngine::GetInstance().ChangeScene("player-select");
    }
    
    
}
/*
void StartScene::Update(float deltaTime){
    
    Engine::ImageButton* btn;
    if (IsMute==0) {
        btn = new Engine::ImageButton("start/mute1.png", "start/mute2.png", 680, 500, 100, 100);//EXIT
        btn->SetOnClickCallback(std::bind(&StartScene::BackOnClick, this, 2));
        AddNewControlObject(btn);
    }
    else{
        btn = new Engine::ImageButton("start/unmute1.png", "start/unmute2.png", 680, 500, 100, 100);//EXIT
        btn->SetOnClickCallback(std::bind(&StartScene::BackOnClick, this, 2));
        AddNewControlObject(btn);
        
    }
 
}
 */
/*
void StartScene::OnMouseDown(int button, int mx, int my) {
    
    
    if(IsMute==1){
        int value = 0;
        al_set_sample_instance_gain(bgmInstance, value);
        AudioHelper::BGMVolume = value;
        IsMute = value;
    }
    else{
        int value = 1;
        al_set_sample_instance_gain(bgmInstance, value);
        AudioHelper::BGMVolume = value;
        IsMute = value;
    }
    
    
    
}
*/
