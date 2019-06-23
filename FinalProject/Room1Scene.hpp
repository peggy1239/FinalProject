//
//  Room1Scene.hpp
//  FinalProject
//
//  Created by lo peichieh on 2019/6/2.
//  Copyright © 2019 lo peichieh. All rights reserved.
//

#ifndef Room1Scene_hpp
#define Room1Scene_hpp
#include "IScene.hpp"
#include "GameEngine.hpp"
#include "ImageButton.hpp"
#include "PlayerSelectScene.hpp"
#include "Player.hpp"
#include "Box.hpp"
#include "Key.hpp"
#include "Door.hpp"
class Player;
class Key;
class Box;
class Door;
class Room1Scene final : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_INSTANCE* bgmInstance;
public:
    Player* role;
    ALLEGRO_BITMAP* background;
    Door* door;
    Key* KEY;
    Box *box,*guider,*heart[5];
    int sub;
    bool key = false; //whether have key or not
    bool keyState[4]={false};
    int gender;
    int lives;
    void Update(float deltaTime) override;
    explicit Room1Scene() = default;
    void Initialize() override;
    void BackOnClick(int stage);
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    bool BoxAndPlayerIsNear();
    bool InfrontDoor();
    void Terminate() override;
};
#endif /* Room1Scene_hpp */
