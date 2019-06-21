#include "LoseScene.hpp"
#include "Label.hpp"
#include "AudioHelper.hpp"

void LoseScene::Initialize() {
	// TODO 1 (2/2): You can imitate the 2 files: 'LoseScene.hpp', 'LoseScene.cpp' to implement your start scene.
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	AddNewObject(new Engine::Label("YOU DIE", "Gore.ttf", 80, halfW-100, halfH / 2, 255, 0, 0, 255, 0.5, 0.5));
	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("lose/again1.png", "lose/again2.png", halfW - 300, halfH * 3 / 2 -100, 280, 120);//AGAIN
	btn->SetOnClickCallback(std::bind(&LoseScene::BackOnClick, this, 1));
	AddNewControlObject(btn);
    
    btn = new Engine::ImageButton("lose/exit1.png", "lose/exit2.png", halfW + 30, halfH * 3 / 2-100, 280, 120);//EXIT
    btn->SetOnClickCallback(std::bind(&LoseScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    
    
	//AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
	AudioHelper::PlayAudio("lose.wav");
}
void LoseScene::BackOnClick(int stage) {
	// Change to select scene.
    
    
	if(stage==1)Engine::GameEngine::GetInstance().ChangeScene("room1");
    else Engine::GameEngine::GetInstance().ChangeScene("start");
}
