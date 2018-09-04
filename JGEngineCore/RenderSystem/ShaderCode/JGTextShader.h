#pragma once
#include"../JGHLSLShaderDevice/JGShader.h"




// 하던일 셰이더 코드 완성됬으니까
// 담에는 세이더 적용해서 렌더링 해보고
// 



class JGTextShader : public JGShader
{
public:
	JGTextShader();
	virtual ~JGTextShader();

protected:
	virtual bool CreateShader() override;
	virtual void JGShaderDataSetting(JGShaderData* Data) override;
	virtual void JGTextureSetting(JGTexture* Texture) override;
};