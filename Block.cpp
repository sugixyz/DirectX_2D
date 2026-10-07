#include "Block.h"
#include"Engine/Model.h"
#include"Engine/BoxCollider.h"
#include"Player.h"

namespace
{
	const XMFLOAT3 COL_POS = { 0.0f,0.5f,0.0f };
	const XMFLOAT3 COL_SIZE = { 2.0f,1.0f,2.0f };

    const float DELTA_TIME = 1.0f / 60.0f;
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

    startPosition = XMLoadFloat3(&transform_.position_);
    endPosition = startPosition;

    moveTime = 1.0f;
    timer = 0.0f;

    canMoving = false;
}

void Block::Update()
{
    if (!canMoving)return;

    //time += DELTA_TIME;
    timer += 0.016f;

    float t = timer / moveTime;
    if (t >= 1.0f)t = 1.0f;

    XMVECTOR pos = XMVectorLerp(startPosition, endPosition, t);
    XMStoreFloat3(&transform_.position_, pos);

    if (t >= 1.0f)
    {
        LerpReset(); 
        timer = 0.0f;
    }
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
    if (pTarget->GetObjectName() != "Player") return;

    Player* p = dynamic_cast<Player*>(pTarget);
    if (!p) return;


    XMFLOAT3 pPos = p->GetPosition();
    XMFLOAT3 pSize = p->GetColSize();

    // --- 各軸のめり込み量（Overlap）を計算 ---

    // X軸の中心距離とサイズ半和から重なりを算出
    float diffX = pPos.x - transform_.position_.x;
    float overlapX = (COL_SIZE.x / 2.0f + pSize.x / 2.0f) - fabsf(diffX);

    // Y軸（足元基準）のめり込み量を算出
    float topMin = (pPos.y + pSize.y < transform_.position_.y + COL_SIZE.y)
        ? (pPos.y + pSize.y)
        : (transform_.position_.y + COL_SIZE.y);

    float bottomMax = (pPos.y > transform_.position_.y)
        ? pPos.y
        : transform_.position_.y;

    float overlapY = topMin - bottomMax;

    // 重なっていない場合は処理しない
    if (overlapX <= 0.0f || overlapY <= 0.0f) return;

    // --- めり込みが浅い方向へ押し戻す ---

    if (overlapX < overlapY)
    {
        // X軸（左右）の押し戻し
        if (diffX < 0.0f)
        {
            // 左側へ押し戻す
            p->SetPosition(transform_.position_.x - (COL_SIZE.x / 2.0f + pSize.x / 2.0f), pPos.y, pPos.z);
            p->CollisionWall();
        }
        else
        {
            // 右側へ押し戻す
            p->SetPosition(transform_.position_.x + (COL_SIZE.x / 2.0f + pSize.x / 2.0f), pPos.y, pPos.z);
            p->CollisionWall();
        }
    }
    else
    {
        // Y軸（上下）の押し戻し
        // プレイヤーの中心（足元 + 高さ/2）とブロックの中心（足元 + 高さ/2）の比較で上下判定
        float pCenterY = pPos.y + pSize.y / 2.0f;
        float bCenterY = transform_.position_.y + COL_SIZE.y / 2.0f;

        if (pCenterY > bCenterY)
        {
            // 上へ押し戻す（着地：プレイヤーの足元をブロックの上面に合わせる）
            p->SetPosition(pPos.x, transform_.position_.y + COL_SIZE.y, pPos.z);
            p->OnGround();
        }
        else
        {
            // 下へ押し戻す（天井衝突：プレイヤーの頭をブロックの底面に合わせる）
            p->SetPosition(pPos.x, transform_.position_.y - pSize.y, pPos.z);
            p->CollisionOnBlock();
        }
    }
}

void Block::ConvertToMovingBlock(XMFLOAT3 move,float time)
{
    XMVECTOR moveVec = XMLoadFloat3(&move);
    startPosition = XMLoadFloat3(&transform_.position_);
    endPosition = startPosition + moveVec;
    moveTime = time;

    canMoving = true;
}

void Block::LerpReset()
{
    XMVECTOR tmp = startPosition;
    startPosition = endPosition;
    endPosition = tmp;
}
