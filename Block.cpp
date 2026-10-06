#include "Block.h"
#include"Engine/Model.h"
#include"Engine/BoxCollider.h"
#include"Player.h"

namespace
{
	const XMFLOAT3 COL_POS = { 0.0f,0.5f,0.0f };
	const XMFLOAT3 COL_SIZE = { 2.0f,1.0f,2.0f };
}

Block::Block(GameObject* parent)
	:GameObject(parent,"Block"),hModel(-1)
{
}

void Block::Initialize()
{
	hModel = Model::Load("Block_Green.fbx");
	BoxCollider* bCol = new BoxCollider(COL_POS, COL_SIZE);
	AddCollider(bCol);
}

void Block::Update()
{
}

void Block::Draw()
{
	Model::SetTransform(hModel, transform_);
	Model::Draw(hModel);
}

void Block::Release()
{
}

void Block::OnCollision(GameObject* pTarget)
{
	if (pTarget->GetObjectName() != "Player")return;

	Player* p = dynamic_cast<Player*>(pTarget);
	XMFLOAT3 pPos = p->GetPosition();
	XMFLOAT3 pSize = p->GetColSize();

	Direction pDirection = CalculateDirection(pPos, pSize);
	switch (pDirection)
	{
	case Direction::TOP:
		p->SetPosition(pPos.x, transform_.position_.y + COL_SIZE.y, pPos.z);
		p->OnGround();
	case Direction::BOTTOM:
		p->SetPosition(pPos.x, transform_.position_.y - pSize.y - 0.01f, pPos.z);
		p->CollisionOnBlock();
	case Direction::LEFT:
		//p->SetPosition(transform_.position_.x - COL_SIZE.x / 2.0f  - pSize.x / 2.0f - 0.01f, pPos.y, pPos.z);
		p->CollisionWall();
	case Direction::RIGHT:
		p->SetPosition(transform_.position_.x + COL_SIZE.x / 2.0f + pSize.x / 2.0f + 0.01f, pPos.y, pPos.z);
		p->CollisionWall();
	}

}

Direction Block::CalculateDirection(XMFLOAT3 pos,XMFLOAT3 size)
{
	if (pos.y >= transform_.position_.y + COL_SIZE.y)return Direction::TOP;
	else if (pos.y + size.y < transform_.position_.y)return Direction::BOTTOM;
	else if (pos.x < transform_.position_.x)return Direction::LEFT;
	else return Direction::RIGHT;
}


