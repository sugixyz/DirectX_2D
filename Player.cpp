#include "Player.h"
#include "Engine/Model.h"
#include "Engine/Debug.h"
#include "TestScene.h"
#include"Engine/Input.h"
#include"Ground.h"
#include"Engine/Collider.h"
#include"Engine/Camera.h"

namespace
{
	enum PlayerState
	{
		PLAYER_IDLE,
		PLAYER_WALK,
		PLAYER_TURN,
		PLAYER_JUMP,
		STATE_MAX
	};

	PlayerState pState = PLAYER_IDLE;

	//ラープの目標値
	float targetAngle = 0.0f;
	//ラープの初期値
	float initAngle = 0.0f;
	float turnFrame = 0.0f;
	//45度回転するのにかかるフレーム
	const float TURN_FRAME = 5.0f;

	const float RADIUS = 0.75f;
	const float SPEED = 0.1f;

	const float CAM_MIN_CLANP = 7 * 2.0f;
	const float CAM_MAX_CLANP = 23 * 2.0f;

	std::vector < std::vector<int>> gMap;
}

Player::Player(GameObject* parent)
	:GameObject(parent,"Player"), hWalkModel(-1), hIdleModel(-1),velocityY(0.0f)
{
}

void Player::Initialize()
{
	hIdleModel = Model::Load("Idle.fbx");
	Model::SetAnimFrame(hIdleModel, 0, 117, 1.0);

	hWalkModel = Model::Load("Walking.fbx");
	Model::SetAnimFrame(hWalkModel, 0, 57, 1.0);

	SphereCollider* col = new SphereCollider(XMFLOAT3(0, 0, 0), 0.75f);
	AddCollider(col);

	transform_.rotate_.y = 270;
}

void Player::Update()
{
	UpdateCameraPosition();

	switch (pState)
	{
	case PLAYER_IDLE:
		UpdateIdle();
		break;
	case PLAYER_WALK:
		UpdateWalk();
		break;
	case PLAYER_JUMP:
		UpdateJump();
		break;
	case PLAYER_TURN:
		UpdateTurn();
		break;
	}

}

void Player::Draw()
{
	if (pState == PLAYER_IDLE)
	{
		Model::SetTransform(hIdleModel, transform_);
		Model::Draw(hIdleModel);
	}
	else if (pState == PLAYER_WALK || pState == PLAYER_TURN || pState == PLAYER_JUMP)
	{
		Model::SetTransform(hWalkModel, transform_);
		Model::Draw(hWalkModel);
	}
}


void Player::Release()
{
}

void Player::OnCollision(GameObject* pTarget)
{
}

bool Player::CheckMap(const XMVECTOR& newPos)
{
	gMap = ground->GetMapData();

	XMFLOAT3 nPos;
	XMStoreFloat3(&nPos, newPos);
	int x, y;
	x = nPos.x / 2;
	y = -nPos.y + 20;
	if (x < 0 || 30< x)return false;
	if (y < 0 || 20 < y)return false;
	//char str[50];
	//sprintf_s(str, sizeof(str), "( %d , %d )", x, z);
	//Debug::Log(str);

	return gMap[y][x] == 1;

}

void Player::UpdateCameraPosition()
{
	XMFLOAT3 camPos = transform_.position_;
	//Xのクランプ
	if (camPos.x < CAM_MIN_CLANP)camPos.x = CAM_MIN_CLANP;
	if (camPos.x > CAM_MAX_CLANP)camPos.x = CAM_MAX_CLANP;

	camPos.y = 9.5;
	camPos.z = -20;
	Camera::SetPosition(camPos);
	camPos.y = 9.0f;
	camPos.z = -5;
	Camera::SetTarget(camPos);
}

void Player::UpdateIdle()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		StartJump();
		return;
	}

	if (Input::IsKey(DIK_A))
	{
		MoveOrTurn(90.0f);
	}
	else if (Input::IsKey(DIK_D))
	{
		MoveOrTurn(270.0f);
	}
}

void Player::UpdateWalk()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		StartJump();
		return;
	}

	float moveX = 0.0f;
	if (Input::IsKey(DIK_A))moveX = -1.0f;
	if (Input::IsKey(DIK_D))moveX = 1.0f;

	if (moveX == 0.0f)
	{
		pState = PLAYER_IDLE;
		return;
	}

	float nextAngle = (moveX < 0.0f) ? 90.0f : 270.0f;
	if (abs(nextAngle - transform_.rotate_.y) >= 90.0f)
	{
		StartTurn(nextAngle);
		return;
	}

	Move(moveX);
}

void Player::UpdateJump()
{
	transform_.position_.y += velocityY;
	velocityY -= 0.005f;

	float moveX = 0.0f;
	if (Input::IsKey(DIK_A))moveX = -1.0f;
	if (Input::IsKey(DIK_D))moveX = 1.0f;

	if (moveX != 0.0f)
	{
		transform_.rotate_.y = (moveX < 0.0f) ? 90.0f : 270.0f;
		Move(moveX);
	}

	if (transform_.position_.y <= 2.0f)
	{
		transform_.position_.y = 2.0f;
		pState = PLAYER_WALK;
	}
}

void Player::UpdateTurn()
{
	static int frame = 0;
	frame++;

	float t = frame / turnFrame * 1.0f;
	float angle = targetAngle - initAngle;

	if (angle > 180.0f)angle -= 360.0f;
	else if (angle < -180.0f)angle += 360.0f;

	transform_.rotate_.y = initAngle + angle * t;

	if (frame >= turnFrame)
	{
		transform_.rotate_.y = targetAngle;
		pState = PLAYER_WALK;
		frame = 0;
	}
}

void Player::MoveOrTurn(float deg)
{
	if (abs(deg - transform_.rotate_.y) >= 90.0f)
	{
		StartTurn(deg);
	}
	else
	{
		pState = PLAYER_WALK;
	}
}

void Player::StartJump()
{
	pState = PLAYER_JUMP;
	velocityY = 0.1f;
}

void Player::StartTurn(float targetDeg)
{
	pState = PLAYER_TURN;
	targetAngle = targetDeg;
	initAngle = transform_.rotate_.y;

	float angle = abs(targetAngle - initAngle);
	if (angle > 180.0f)angle -= 180.0f;

	turnFrame = angle * TURN_FRAME / 45.0f;
}

void Player::Move(float dirX)
{
	XMVECTOR pos = XMLoadFloat3(&transform_.position_);
	XMVECTOR vec = XMVectorSet(dirX, 0, 0, 0);
	vec = XMVector3Normalize(vec);

	pos = XMVectorAdd(pos, vec * SPEED);
	if (CheckMap(XMVectorAdd(pos, vec * RADIUS)))
	{
		pos = XMVectorSubtract(pos, vec * SPEED);
	}
	XMStoreFloat3(&transform_.position_, pos);
}
