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
private:
	//モデルハンドル
	int hModel;
private:
	Direction CalculateDirection(XMFLOAT3 pos,XMFLOAT3 size);
};