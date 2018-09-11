#pragma once
#include"../JGHLSLShaderDevice/JGShader.h"


class ENGINE_EXPORT JGColorShader : public JGShader
{
private:

public:
	JGColorShader();
	virtual ~JGColorShader();
protected:
	virtual bool CreateShader() override;
	virtual void JGShaderDataSetting(JGShaderData* Data) override;
};