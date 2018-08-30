#pragma once
#include"Motivated2DComponent.h"
class JG2DMesh;
class JGTexture;

class StaticMesh2DObject;

// 내일 할 일
// 그담에는2D엑터를 만들어보고 렌더링해보자.
// 그리고 계층 구조 이용해서 렌더링 해보자..



/*
Class : StaticMesh2DComponent 
@m wstring m_ShaderName : 적용할 셰이더 이름
@m JGTexture& m_Texture : 텍스쳐
@m JG2DMesh m_Mesh : 메시 
@m int m_ZOrder : 그리는 순서(값이 작을수록 나중에 그려진다.) */
class ENGINE_EXPORT StaticMesh2DComponent : public Motivated2DComponent
{
private:
	std::wstring m_ShaderName = TT("None");
	JGTexture* m_Texture = nullptr;
	JG2DMesh* m_Mesh     = nullptr;
public:
	StaticMesh2DComponent();
	virtual ~StaticMesh2DComponent();
	/*
	Exp : 2D용 메쉬 및 텍스쳐, 셰이더를 생성한다. 
	// 나중에 전역 클래스(ConstructHelper 클래스로 대체)
	@param const wstring& TexturePath : 텍스쳐경로 *///(지워질 예정)
	bool ConstructMesh2D(const std::wstring& TexturePath);



	/*
	Exp : StaticMesh2DComponent의 변수들을 셋팅한다. 
	@param StaticMesh2DObject* Object : JGConstructHelper::StaticMesh2D 클래스가 생성해준 오브젝트 포인터 */
	void SetStaticMesh2DComponent(StaticMesh2DObject* Object);
	/*
	Exp : 텍스쳐의 가로 길이를 가져온다. */
	float GetTextureWdith();
	/*
	Exp : 텍스쳐의 세로 길이를 가져온다. */
	float GetTextureHeight();
	/*
	Exp : 메시를 가져온다. */
	JG2DMesh* GetMesh();
	/*
	Exp : 텍스쳐를 가져온다. */
	JGTexture* GetTexture();
	/*
	Exp : 렌더링 한다.(셰이더가 다르다면 상속 받아서 커스텀 하셈 
	경고!! : Component의 Render함수를 호출할것..) */
	virtual void Render() override;
};