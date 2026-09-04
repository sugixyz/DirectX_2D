#include "TitleScene.h"
#include"Engine/Text.h"
#include"Engine/Image.h"
#include"Engine/Input.h"
#include"Engine/SceneManager.h"


TitleScene::TitleScene(GameObject* parent)
	:GameObject(parent,"TitleScene"),text(nullptr),hImage(-1)
{
}

void TitleScene::Initialize()
{
	text = new Text();
	text->Initialize();

	hImage = Image::Load("title.jpg");
	assert(hImage > 0);
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

	text->Draw(40, 40, "START_PUSH_SPACE");
}

void TitleScene::Release()
{
	//text->Release();
	//Image::Release(hImage);
}
