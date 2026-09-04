#pragma once
#include "Engine/GameObject.h"

class Ground;

class Player :
    public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Player(GameObject* parent);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;
	//地面のポインタを取得
	void SetGround(Ground* g) { ground = g; }
	void OnCollision(GameObject* pTarget) override;
private:
	//歩きアニメーションモデル
	int hWalkModel;
	//待機アニメーションモデル
	int hIdleModel;
	//地面のポインタ
	Ground* ground;

	XMFLOAT3 velocity;
private:
	bool CheckMap(const XMVECTOR& newPos);
	void UpdateCameraPosition();

	void UpdateIdle();
	void UpdateWalk();
	void UpdateJump();
	void UpdateTurn();

	void MoveOrTurn(float deg);
	void StartJump();
	void StartTurn(float targetDeg);
	void Move();

	void CheckGoal();
};

