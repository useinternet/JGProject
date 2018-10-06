#pragma once

#include"Widget.h"
#include"../../../RenderSystem/JGMaterial/JG2DMesh.h"
class JGTexture;
class StaticMesh2DComponent;
class ConstructObject;
/*
Class : ImageBox
Exp : UI 용 이미지 박스 .. 
@m unique_ptr<StaticMesh2DComponent> m_Image :  이미지 컴포넌트 */
class ENGINE_EXPORT ImageBox : public Widget
{
private:
	static std::map<std::wstring, ConstructObject*> m_mImagePath;
	std::unique_ptr<StaticMesh2DComponent> m_Image;
	float m_BlendAlpha = 1.0f;
public:
	ImageBox();
	virtual ~ImageBox();

	/*
	Exp : 이미지 박스를 생성합니다. 
	@param const wstring& ImagePath : 이미지 경로 
	@param const EPivot pivot : 렌더링 기준 */
	bool CreateImage(const std::wstring& ImagePath, const EPivot pivot);
	/*
	Exp : 알파 값 셋팅 */
	void SetBlend(const float a);
	/*
	Exp : 알파 값을 해당값 만큼 더하기 */
	void AddBlend(const float a);


	void ReSizeImage(const float width, const float height);

};