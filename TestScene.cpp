#include "TestScene.h"
#include "Player.h"
#include"Ground.h"
//#include"Feed.h"
#include"Engine/Camera.h"
#include"Engine/Text.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene"),myScore(10)
{
}

//初期化
void TestScene::Initialize()
{	
	Player* p = Instantiate <Player>(this);
	Ground* g = Instantiate<Ground>(this);
	//Feed* f = Instantiate<Feed>(this);
	p->SetGround(g);

	Camera::SetPosition(XMFLOAT3(0, 12, -20));
	Camera::SetTarget(XMFLOAT3(0, 0, 0));

	pText = new Text;
	pText->Initialize();
}

//更新
void TestScene::Update()
{
}

//描画
void TestScene::Draw()
{
	char scoreT[30];
	sprintf_s(scoreT, sizeof(scoreT), "SCORE:%04d", myScore);
	pText->Draw(20, 20, scoreT);
}

//開放
void TestScene::Release()
{
	pText->Release();
}
