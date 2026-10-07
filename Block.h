#pragma once
#include"Engine/GameObject.h"

enum class Direction
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
};

class Block : public GameObject
{
public:
	Block(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	void OnCollision(GameObject* pTarget) override;
	void ConvertToMovingBlock(XMFLOAT3 move,float time = 1.0f);
private:
	//モデルハンドル
	int hModel;
	//終了位置
	XMVECTOR endPosition;
	//開始位置
	XMVECTOR startPosition;
	//移動にかかる時間
	float moveTime;
	//移動するかどうかのbool
	bool canMoving;
	//実際の時間
	float timer;
private:
	void LerpReset();
};