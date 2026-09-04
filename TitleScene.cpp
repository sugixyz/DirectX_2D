#include "TitleScene.h"
#include"Engine/Image.h"
#include"Engine/Input.h"
#include"Engine/SceneManager.h"


TitleScene::TitleScene(GameObject* parent)
	:GameObject(parent,"TitleScene"),hImage(-1)
{
}

void TitleScene::Initialize()
{
	hImage = Image::Load("title.png");
	assert(hImage >= 0);
}

void TitleScene::Update()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		SceneManager* sceneManager = (SceneManager*)(this->GetParent());
		sceneManager->ChangeScene(SCENE_ID_TEST);
	}
}

void TitleScene::Draw()
{
	Image::SetTransform(hImage, transform_);
	Image::Draw(hImage);
}

void TitleScene::Release()
{
	//Image::Release(hImage);
}
