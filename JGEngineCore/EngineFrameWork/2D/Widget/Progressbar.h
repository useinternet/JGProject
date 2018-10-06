#pragma once
#include"Widget.h"

#define BarTypeCount 3;

class ENGINE_EXPORT Progressbar : public Widget
{
	enum class EBarType
	{
		Full,Empty,Progressing
	};
private:
	class StaticMesh2DComponent* m_Bar[3];
	float m_Aspect = 1.0f;

public:
	Progressbar();
	virtual ~Progressbar();
	void CreateProgressBar(const std::wstring& FullBarPath, const std::wstring& EmptyBarPath,
		std::wstring& ProgressingBarPath);
	void SetBindingData(const float Aspect);
};