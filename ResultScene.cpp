#include "ResultScene.h"
#include"Engine/Text.h"
#include"Engine/Input.h"
#include"Engine/SceneManager.h"

ResultScene::ResultScene(GameObject* parent)
	:GameObject(parent, "ResultScene")
{
}

void ResultScene::Initialize()
{
	text = new Text();
	text->Initialize();
}

void ResultScene::Update()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		SceneManager* sceneManager = (SceneManager*)(this->GetParent());
		sceneManager->ChangeScene(SCENE_ID_TITLE);
	}
}

void ResultScene::Draw()
{
	text->Draw(40, 40, "TITLE_PUSH_SPACE");
}

void ResultScene::Release()
{
	//text->Release();
}
