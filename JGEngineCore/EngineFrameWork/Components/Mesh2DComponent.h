#pragma once
#include"Motivated2DComponent.h"
class JGMesh;
class JGTexture;

// 내일 할 일
// 메쉬 컴포넌트만 완성시키고 렌더링해보자
// 그담에는2D엑터를 만들어보고 렌더링해보자.
// 그담에는 텍스쳐 정보로 기반해 렌더링해보자
// 그리고 계층 구조 이용해서 렌더링 해보자..
class Mesh2DComponent : public Motivated2DComponent
{
private:
	std::wstring m_ShaderName = TT("None");
	std::unique_ptr<JGShaderData> m_ShaderData;
	std::unique_ptr<JGTexture> m_Texture;
	std::unique_ptr<JGMesh> m_Mesh;

public:
	Mesh2DComponent();
	virtual ~Mesh2DComponent();

	bool ConstructMesh2D(const std::wstring& TexturePath);
	/*
	Exp : 렌더링 한다. ( 상속받지 마셈. ) */
	virtual void Render() override;

};