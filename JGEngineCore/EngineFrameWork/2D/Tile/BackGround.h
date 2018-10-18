#pragma once

#include"../../Components/Motivated2DComponent.h"

class ENGINE_EXPORT BackGround : public Motivated2DComponent
{
private:
	std::unique_ptr<class StaticMesh2DComponent> m_BackGround[2];
	JGVector2D* m_CenterVector = nullptr;
	JGVector2D m_MoveSpeed;
	JGVector2D m_ScaleSpeed;
	bool m_bHorizon = false;
	float DeltaCameraX = 0.0f;
	float DeltaCameraY = 0.0f;
public:
	BackGround();
	virtual ~BackGround();
	virtual void BeginComponent(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	void CreateBackGround(const std::wstring& BackGroundPath, const JGVector2D& MoveSpeed,const bool Horizon = true);
	void Move(const float ScaleX,const float ScaleY);
	bool IsGetOffWidthDisplay(StaticMesh2DComponent* mesh);
	bool IsGetOffHeightDisplay(StaticMesh2DComponent* mesh);
	void RegistorCenter(JGVector2D* vec);
};