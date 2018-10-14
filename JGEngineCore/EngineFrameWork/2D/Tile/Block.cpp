#include"Block.h"
#include"../../Components/Box2DCollisionComponent.h"
#include"../../Components/StaticMesh2DComponent.h"
#include"../../../EngineStatics/JGConstructHelper.h"

using namespace std;

Block::Block()
{
	RegisterComponentID(typeid(this));
	m_Collision = make_unique<Box2DCollisionComponent>();
	m_Collision->SetBodyType(E2DBodyType::Static);
}

Block::~Block()
{


}
void Block::BeginComponent(World * world)
{
	float BlockWidth = m_vBlocks[0]->GetTextureWdith();
	float BlockHeight = m_vBlocks[0]->GetTextureHeight();
	m_Collision->SetComponentLocation(
		GetComponentWorldLocation().X() + ((BlockWidth * m_WidthCount) / 2),
		GetComponentWorldLocation().Y() + ((BlockHeight * m_HeightCount) / 2));
	Motivated2DComponent::BeginComponent(world);
}
void Block::CreateBlock(const wstring& BlockTexutrePath, size_t BlockWidthCount, size_t BlockHeightCount)
{
	m_WidthCount = BlockWidthCount;
	m_HeightCount = BlockHeightCount;
	JGConstructHelper::StaticMesh2D Block_Mesh(GetComponentName(), EPivot::TopLeft, BlockTexutrePath);
	if (Block_Mesh.Success)
	{
		for (int i = 0; i < BlockHeightCount; ++i)
		{
			for (int j = 0; j < BlockWidthCount; ++j)
			{
				unique_ptr<StaticMesh2DComponent> block = make_unique<StaticMesh2DComponent>();
				// 컴포넌트 초기화( 이름등록, 시작 컴포넌트 , 소유 오브젝트 넣기)
				block->BeginComponent(GetWorld());
				block->SetOwnerObject(GetOwnerObject());
				wchar_t num[4];
				wsprintf(num, TT("%d"), i);
				wstring numString = num;
				block->RegisterName(GetComponentName() + numString);

				// 자식에 추가하고 위치 설정
				AddChild(block.get());
				block->SetConstructObject(Block_Mesh.Object);
				block->SetComponentLocation(block->GetTextureWdith() * j,block->GetTextureHeight() * i);
				m_vBlocks.push_back(move(block));
			}

		}
	}
	float hw = (m_vBlocks[0]->GetTextureWdith() * BlockWidthCount) / 2;
	float hh = (m_vBlocks[0]->GetTextureHeight() * BlockHeightCount) / 2;


	m_Collision->SetAsBox(hw, hh);
	AddChild(m_Collision.get());
}

Box2DCollisionComponent* Block::GetCollision()
{
	return m_Collision.get();
}
