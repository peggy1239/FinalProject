#ifndef Room2Scene_hpp
#define Room2Scene_hpp

#include "IScene.hpp"
#include "GameEngine.hpp"
#include "ImageButton.hpp"
#include "PlayerSelectScene.hpp"
#include "Player.hpp"
#include "Box.hpp"
#include "Key.hpp"
#include "Door.hpp"
#include "Subtitle2.hpp"
class Player;

class Room2Scene final : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_INSTANCE* bgmInstance;
public:
    Player* role;
    ALLEGRO_BITMAP* background;
    Door* door;
    Key* KEY;
    Box *box,*heart[5];
    Box* elderman;
    Subtitle2* subtitle;
    Trap* trap;
    bool subtitling;
    bool TrapTrap();
    int IsMute;
    int key = 0; //whether have key or not
    bool findOld = false;// if find elderman ->true;
    bool keyState[4]={false};
    int gender;
    int lives;
    void Update(float deltaTime) override;
    explicit Room2Scene() = default;
    void Initialize() override;
    void BackOnClick(int stage);
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    bool BoxAndPlayerIsNear();
    bool InfrontOld();
    bool InfrontDoor();
    void Terminate() override;
};

#endif /* Room2Scene_hpp */
