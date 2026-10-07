#include "Ground.h"
#include"Engine/Model.h"
#include"Player.h"
#include"Block.h"
#include"Engine/Text.h"
#include"Engine/CsvReader.h"

namespace
{
	int WALL_CSV = 1;
	int MOVE_X_CSV = 2;
	int MOVE_Y_CSV = 3;
	int PLAYER_CSV = 4;
}

Ground::Ground(GameObject* parent)
	:GameObject(parent), hBlock(-1),hBack(-1), mapHeight(-1), mapWidth(-1)
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
			
			XMFLOAT3 pos = CalculatePosition(x, y);

			if (value == PLAYER_CSV)
			{
				GameObject* p = FindObject("Player");
				p->SetPosition(pos);
			}
			else if (value >= WALL_CSV)
			{
				Block* block = Instantiate<Block>(this->GetParent());
				block->SetPosition(pos);

				if (value == MOVE_X_CSV)
				{
					XMFLOAT3 direction = { 4.0f,0.0f,0.0f };
					block->ConvertToMovingBlock(direction,2.0f);
				}
				else if (value == MOVE_Y_CSV)
				{
					XMFLOAT3 direction = { 0.0f,2.0f,0.0f };
					block->ConvertToMovingBlock(direction,2.0f);
				}
			}
		}
	}
}

void Ground::Initialize()
{
	hBlock = Model::Load("Block_Green.fbx");
	hBack = Model::Load("BackGround.fbx");
}

void Ground::Update()
{
}

void Ground::Draw()
{
	Model::SetTransform(hBack, transform_);
	Model::Draw(hBack);

	Transform bt;
	for (int y = 0;y < mapHeight;y++)
	{
		for (int x = 0; x < mapWidth;x++)
		{
			if(map[y][x] == 1)
			{
				bt.position_.x = x * 2 + 1;
				bt.position_.y = -(y - mapHeight) - 1;
				//Model::SetTransform(hBlock, bt);
				//Model::Draw(hBlock);
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
	pos.x = x * 2;
	pos.y = -(y - mapHeight);
	pos.z = 0.0f;

	return pos;
}
