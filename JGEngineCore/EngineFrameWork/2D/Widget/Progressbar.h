#pragma once
#include"Widget.h"
#include"../../../RenderSystem/JGMaterial/JG2DMesh.h"
#define BarTypeCount 3;

// 나중에 프로그래스바 따개도 따로하자..
// 진행중인 프로그래스바도 생각해ㄷ서 만들기
class JTimerHandle;
class StaticMesh2DComponent;
class ConstructObject;
enum class ENGINE_EXPORT EBarType
{
	Full,Empty
};
class ENGINE_EXPORT Progressbar : public Widget
{
private:
	struct SProgressBar
	{
		std::unique_ptr<StaticMesh2DComponent> LeftEnd;
		std::unique_ptr<StaticMesh2DComponent> RightEnd;
		std::unique_ptr<StaticMesh2DComponent> Mid;
	};
private:
	static std::map<std::wstring, ConstructObject*> m_mBarPath;
private:
	std::unique_ptr<StaticMesh2DComponent> m_FullBar;
	std::unique_ptr<StaticMesh2DComponent> m_EmptyBar;
	float m_Aspect = 1.0f;

public:
	Progressbar();
	virtual ~Progressbar();
	virtual void BeginComponent(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	void CreateProgressBar(const std::wstring& FullBarPath, const std::wstring& EmptyBarPath,EPivot pivot);
	void SetBindingData(const float Aspect);


private:
	void CreateBarResource(StaticMesh2DComponent* Bar, const std::wstring& path,EPivot pivot);
	bool ResourceCheck(const std::wstring& CheckPath);
};