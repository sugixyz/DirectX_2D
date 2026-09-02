#include "Player.h"
#include "Engine/Model.h"
#include "Engine/Debug.h"
#include "TestScene.h"
#include"Engine/Input.h"
#include"Ground.h"
#include"Engine/Debug.h"
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

	const float CAM_MIN_CLANP = 7 * 2.0f;
	const float CAM_MAX_CLANP = 58 * 2.0f;

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

	if (pState == PLAYER_JUMP)
	{
		transform_.position_.y += velocityY;
		velocityY -= 0.005f;
		if (transform_.position_.y < 2.0f)
		{
			transform_.position_.y = 2.0f;
			pState = PLAYER_WALK;
			return;
		}
		return;
	}

	if (pState == PLAYER_TURN)
	{
		static int frame = 0;
		frame++;
		float t = frame / turnFrame;
		float angle = targetAngle - initAngle;
		if (angle > 180)angle = angle - 360;
		else if (angle < -180)angle = angle + 360;
		transform_.rotate_.y = initAngle + angle * t;
		if (frame >= turnFrame)
		{
			pState = PLAYER_WALK;
			transform_.rotate_.y = targetAngle;
			frame = 0;
		}
		return;
	}

	XMVECTOR pos = XMLoadFloat3(&transform_.position_);
	XMFLOAT3 move = XMFLOAT3(0, 0, 0);
	const float SPEED = 0.05f;
	pState = PLAYER_IDLE;

	float prevAngleY = transform_.rotate_.y;

	if (Input::IsKey(DIK_A))
	{
		move.x = -1;
		pState = PLAYER_WALK;
	}
	if (Input::IsKey(DIK_D))
	{
		move.x = 1;
		pState = PLAYER_WALK;
	}
	if (Input::IsKeyDown(DIK_SPACE))
	{
		pState = PLAYER_JUMP;
		velocityY = 0.1f;
		return;
	}

	if (pState == PLAYER_IDLE)return;

	float currentAngleY = atan2f(move.x, move.z) * 180 / XM_PI + 180;
	if (abs(currentAngleY - prevAngleY) >= 45)
	{
		pState = PLAYER_TURN;
		targetAngle = currentAngleY;
		initAngle = prevAngleY;
		float angle = abs(currentAngleY - prevAngleY);
		if (angle > 180)angle = angle - 180;
		turnFrame = angle * TURN_FRAME / 45 ;
	}

	if (pState == PLAYER_TURN)return;

	transform_.rotate_.y = currentAngleY;
	XMVECTOR vec = XMLoadFloat3(&move);
	vec = XMVector3Normalize(vec);

	pos = XMVectorAdd(pos, vec * SPEED);
	if (CheckMap(XMVectorAdd(pos,vec * RADIUS)))pos = XMVectorSubtract(pos, vec * SPEED);
	XMStoreFloat3(&transform_.position_, pos);
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
	x = nPos.x;
	y = -nPos.y + 20;
	if (x < 0 || 120< x)return false;
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

	camPos.y += 7.5;
	camPos.z -= 20;
	Camera::SetPosition(camPos);
	camPos.y -= 0.5f;
	camPos.z = -5;
	Camera::SetTarget(camPos);
}
