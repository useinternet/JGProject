#pragma once

#include"Widget.h"
#include"../../../RenderSystem/JGMaterial/JG2DMesh.h"

class StaticMesh2DComponent;
class ENGINE_EXPORT ImageBox : public Widget
{
private:
	std::unique_ptr<StaticMesh2DComponent> m_Image;
	float m_BlendAlpha = 1.0f;
public:
	ImageBox();
	virtual ~ImageBox();

	bool CreateImage(const std::wstring& ImagePath, const EPivot pivot);
	void SetBlend(const float a);
	void AddBlend(const float a);

};