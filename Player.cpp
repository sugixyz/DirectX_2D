#include "Player.h"
#include "Engine/Model.h"
#include"Engine/Input.h"
#include"Ground.h"
#include"Engine/Collider.h"
#include"Engine/Camera.h"
#include"Engine/SceneManager.h"

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

	//当たり判定の半径
	const float RADIUS = 0.75f;
	//１フレーム間の速度
	const float SPEED = 0.1f;
	//X軸の減速率
	const float DECELERATION_RATE = 0.85f;
	//ジャンプの初速
	const float JUMP_INITIAL_SPEED = 0.19f;
	//重力（１フレーム毎の減速速度）
	const  float GRAVITY = -0.005;

	const float CAM_MIN_CLANP = 7 * 2.0f;
	const float CAM_MAX_CLANP = 23 * 2.0f;

	std::vector < std::vector<int>> gMap;
}

Player::Player(GameObject* parent)
	:GameObject(parent,"Player"), hWalkModel(-1), hIdleModel(-1),ground(nullptr),velocity(0.0f,0.0f,0.0f)
{
}

void Player::Initialize()
{
	hIdleModel = Model::Load("Idle.fbx");
	Model::SetAnimFrame(hIdleModel, 0, 117, 1.0);

	hWalkModel = Model::Load("Walking.fbx");
	Model::SetAnimFrame(hWalkModel, 0, 57, 1.0);

	BoxCollider* bCol = new BoxCollider(XMFLOAT3(0.0f, 1.8f, 0.0f), XMFLOAT3(1.5f, 3.6f, 1.5f));
	AddCollider(bCol);

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

	CheckGoal();
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

bool Player::IsWall(float x, float y)
{
	gMap = ground->GetMapData();

	int mapX, mapY;
	mapX = (x - 1.0f) / 2.0f + 0.5f;
	mapY = 20 - y;

	//マップ範囲外は壁として判定
	if (mapX < 0 || 30 <= mapX)return true;
	if (mapY < 0 || 20 <= mapY)return true;

	return gMap[mapY][mapX] == 1;
}

void Player::UpdateCameraPosition()
{
	XMFLOAT3 camPos = transform_.position_;
	//Xのクランプ
	if (camPos.x < CAM_MIN_CLANP)camPos.x = CAM_MIN_CLANP;
	if (camPos.x > CAM_MAX_CLANP)camPos.x = CAM_MAX_CLANP;

	camPos.y = 8.5;
	camPos.z = -20;
	Camera::SetPosition(camPos);
	camPos.y = 8.0f;
	camPos.z = -5;
	Camera::SetTarget(camPos);
}

void Player::UpdateIdle()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		StartJump();
	}

	if (Input::IsKey(DIK_A))
	{
		velocity.x = -SPEED;
		MoveOrTurn(90.0f);
	}
	else if (Input::IsKey(DIK_D))
	{
		velocity.x = SPEED;
		MoveOrTurn(270.0f);
	}
	else
	{
		velocity.x = 0.0f;
	}

	MoveX();
	MoveY();
}

void Player::UpdateWalk()
{
	if (Input::IsKeyDown(DIK_SPACE))
	{
		StartJump();
	}

	if (Input::IsKey(DIK_A))velocity.x = -SPEED;
	else if (Input::IsKey(DIK_D))velocity.x = SPEED;
	else velocity.x *= DECELERATION_RATE;

	if (abs(velocity.x) <= 0.001f)
	{
		velocity.x = 0.0f;
		pState = PLAYER_IDLE;
	}

	if (velocity.x != 0.0f)
	{
		float nextAngle = (velocity.x < 0.0f) ? 90.0f : 270.0f;
		if (abs(nextAngle - transform_.rotate_.y) >= 90.0f)
		{
			StartTurn(nextAngle);
			return;
		}
	}

	MoveX();
	MoveY();
}

void Player::UpdateJump()
{
	velocity.x *= 0.995f;
	if (abs(velocity.x) <= 0.001f)velocity.x = 0.0f;

	MoveX();
	MoveY();
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
	velocity.y = JUMP_INITIAL_SPEED;
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

void Player::MoveX()
{
	transform_.position_.x += velocity.x;

	//プレイヤーの左右の端
	float left = transform_.position_.x - RADIUS;
	float right = transform_.position_.x + RADIUS;
	float bottom = transform_.position_.y + 0.1f;
	float top = transform_.position_.y + 3.6f - 0.1f;

	if (velocity.x > 0.0f)
	{
		if (IsWall(right, bottom) || IsWall(right, top))
		{
			int mapX = static_cast<int>(right / 2);
			float blockLeft = mapX * 2.0f;
			transform_.position_.x = blockLeft - RADIUS;
			velocity.x = 0.0f;
		}
	}

	else if (velocity.x < 0.0f)
	{
		if (IsWall(left, bottom) || IsWall(left, top))
		{
			int mapX = static_cast<int>(left / 2);
			float blockRight = (mapX + 1) * 2.0f;
			transform_.position_.x = blockRight + RADIUS;
			velocity.x = 0.0f;
		}
	}
}

void Player::MoveY()
{
	velocity.y += GRAVITY;
	transform_.position_.y += velocity.y;

	float left = transform_.position_.x - RADIUS + 0.1f;
	float right = transform_.position_.x + RADIUS - 0.1f;
	float bottom = transform_.position_.y;
	float top = transform_.position_.y + 3.6f;

	if (velocity.y <= 0.0f)
	{
		if (IsWall(left, bottom - 0.1f) || IsWall(right, bottom - 0.1f))
		{
			int mapY = static_cast<int>(20.0f - bottom + 0.1f);
			float blockTop = 20.0f - mapY;
			transform_.position_.y = blockTop;
			velocity.y = 0.0f;

			if (pState == PLAYER_JUMP)
			{
				pState = (velocity.x != 0.0f) ? PLAYER_WALK : PLAYER_IDLE;
			}
		}
		else
		{
			if (pState != PLAYER_JUMP)
			{
				pState = PLAYER_JUMP;
			}
		}
	}
	else if (velocity.y > 0.0f)
	{
		if (IsWall(left, top) || IsWall(right, top))
		{
			int mapY = static_cast<int>(-top + 20);
			float blockBottom = (20.0f - mapY);
			transform_.position_.y = blockBottom - 3.6f;
			velocity.y = 0.0f;
		}
	}
}

void Player::CheckGoal()
{
	if (transform_.position_.x >= 57)
	{
		SceneManager* sceneManagere = dynamic_cast<SceneManager*>(FindObject("SceneManager"));
		sceneManagere->ChangeScene(SCENE_ID_RESULT);
	}
}
