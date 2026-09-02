#include "Ground.h"
#include"Engine/Model.h"
#include"Player.h"
#include"Engine/Text.h"
#include"Engine/CsvReader.h"

namespace
{
	int WALL_CSV = 1;
	int PLAYER_CSV = 2;
	//int ENEMY_CSV = 3;
}

Ground::Ground(GameObject* parent)
	:GameObject(parent), hBlock(-1), mapHeight(-1), mapWidth(-1)
{
	CsvReader csv;
	csv.Load("map.csv");
	mapWidth = csv.GetWidth();
	mapHeight = csv.GetHeight();
	map = std::vector<std::vector<int>>(mapHeight, std::vector<int>(mapWidth, 0));
	
	for (int y = 0;y < mapHeight;y++)
	{
		for (int x = 0;x < mapWidth;x++)
		{
			int value = csv.GetValue(x, y);
			map[y][x] = value;
			if (value != WALL_CSV)
			{
				XMFLOAT3 pos = CalculatePosition(x, y);

				if (value == PLAYER_CSV)
				{
					GameObject* p = FindObject("Player");
					p->SetPosition(pos);
				}
			}
		}
	}
}

void Ground::Initialize()
{
	hBlock = Model::Load("Block_Green.fbx");
}

void Ground::Update()
{
}

void Ground::Draw()
{
	Transform bt;
	for (int y = 0;y < mapHeight;y++)
	{
		for (int x = 0; x < mapWidth;x++)
		{
			if(map[y][x] == 1)
			{
				bt.position_.x = x * 2;
				bt.position_.y = -(y - mapHeight);
				Model::SetTransform(hBlock, bt);
				Model::Draw(hBlock);
			}
		}
	}
}

void Ground::Release()
{
}

XMFLOAT3 Ground::CalculatePosition(int x, int y)
{
	XMFLOAT3 pos;
	//pos.x = (x - 5.0f) * 2 + 1;
	pos.x = x * 2;
	//pos.y = 0.0f;
	pos.y = -(y - mapHeight);
	pos.z = 0.0f;

	return pos;
}
