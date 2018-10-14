#pragma once
#include"../../Components/Motivated2DComponent.h"

enum class EPivot;
class StaticMesh2DComponent;
class Box2DCollisionComponent;
class ENGINE_EXPORT Block : public Motivated2DComponent
{
private:
	std::vector<std::shared_ptr<StaticMesh2DComponent>> m_vBlocks;
	std::unique_ptr<Box2DCollisionComponent>            m_Collision;
	size_t m_WidthCount = 0;
	size_t m_HeightCount = 0;
public:
	Block();
	~Block();
	virtual void BeginComponent(World* world) override;
	void CreateBlock(const std::wstring& BlockTexutrePath, size_t BlockWidthCount,size_t BlockHeightCount);
	Box2DCollisionComponent* GetCollision();
};
