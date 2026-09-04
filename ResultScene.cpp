#include "ResultScene.h"
#include"Engine/Text.h"
#include"Engine/Image.h"
#include"Engine/Input.h"
#include"Engine/SceneManager.h"

ResultScene::ResultScene(GameObject* parent)
	:GameObject(parent, "ResultScene"),hImage(-1)
{
}

void ResultScene::Initialize()
{
	hImage = Image::Load("clear.png");
	assert(hImage >= 0);
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
	Image::SetTransform(hImage,transform_);
	Image::Draw(hImage);
}

void ResultScene::Release()
{
}
