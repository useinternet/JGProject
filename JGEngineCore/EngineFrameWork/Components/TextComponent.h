#pragma once
#include"StaticMesh2DComponent.h"

/*
Class : TextComponent */
class ENGINE_EXPORT TextComponent : public StaticMesh2DComponent
{
private:
	std::unique_ptr<std::wstring> m_Text;
	std::unique_ptr<JG2DMesh> m_TextMesh;
	std::unique_ptr<struct STextBuffer_PS> m_TextBufferDesc;



	std::string m_FontPath;
	float m_TextSize = 0.0f;


	//
	float m_FramePerSecond  = 0.0f;
	float m_AccSecond = 0.0f;
public:
	TextComponent();
	virtual ~TextComponent();
	/*
	Exp : 매 프레임 한번 실행되는 함수이다. ( 부모 클래스의 Tick도 호출해야한다. )*/
	virtual void Tick(const float DeltaTime) override;
	/*
	Exp : 생성자 오브젝트를 셋팅한다. 
	@param ConstructObject* Object : 생성자 오브젝트 */
	virtual void SetConstructObject(ConstructObject* Object);
	/*
	Exp : 텍스트컴포넌트에 입력한다. */
	void SetText(const wchar_t* Text, ...);
	/*
	Exp: 텍스트 색상을 설정한다. */
	void SetTextColor(const float r, const float g, const float b);
	/*
	Exp : 텍스트 알파값을 설정한다. */
	void SetTextAplha(const float a);
	/*
	Exp : 텍스트를 1초당 몇번 업데이트 시킬것인지 정한다.*/
	void SetFramePerSecond(float Frame);
	/*
	Exp : 텍스트 사이즈를 정한다. */
	void SetTextSize(const float TextSize);
private:
	/*
	Exp : 셰이더 상수버퍼의 파라미터들을 설정한다. ( 상속 받아서 쓰세요 / 부모 함수 호출 안해도되요.. */
	virtual void ShaderParamSetting(JGShaderData* Data);


	bool CreateTextMesh();
};