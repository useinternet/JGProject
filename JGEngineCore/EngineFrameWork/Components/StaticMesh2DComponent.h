#pragma once
#include"Motivated2DComponent.h"
class JGMesh;
class JGTexture;




// 내일 할 일
// 그담에는2D엑터를 만들어보고 렌더링해보자.
// ConstructHelper 만들기
// 그리고 계층 구조 이용해서 렌더링 해보자..


class ENGINE_EXPORT StaticMesh2DComponent : public Motivated2DComponent
{
private:
	typedef struct Mesh2DVertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 tex;
	}Mesh2DVertexType;
private:
	std::wstring m_ShaderName = TT("None");
	std::unique_ptr<JGTexture> m_Texture;
	std::unique_ptr<JGMesh> m_Mesh;
public:
	StaticMesh2DComponent();
	virtual ~StaticMesh2DComponent();
	/*
	Exp : 2D용 메쉬 및 텍스쳐, 셰이더를 생성한다. 
	// 나중에 전역 클래스(ConstructHelper 클래스로 대체)
	@param const wstring& TexturePath : 텍스쳐경로 */
	bool ConstructMesh2D(const std::wstring& TexturePath);

	float GetTextureWdith();
	float GetTextureHeight();

	/*
	Exp : 렌더링 한다. ( 상속받지 마셈. ) */
	virtual void Render() override;
private:
	void Create2DMesh();
};