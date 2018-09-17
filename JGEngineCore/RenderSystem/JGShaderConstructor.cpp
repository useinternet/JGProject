#include "JGShaderConstructor.h"
#include"JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"ShaderCode/JG2DSpriteShader.h"
#include"ShaderCode/JGTextShader.h"
#include"ShaderCode/JG2DAnimationShader.h"
#include"ShaderCode/JGColorShader.h"
using namespace std;
JGShaderConstructor::JGShaderConstructor()
{

}

JGShaderConstructor::~JGShaderConstructor()
{

}
void JGShaderConstructor::LinkHLSLDevice(JGHLSLShaderDevice* Device)
{
	bool result = true;
	Device->AddShader<JG2DSpriteShader>(TT("2DSpriteShader"));
	Device->AddShader<JGTextShader>(TT("TextShader"));
	Device->AddShader<JG2DAnimationShader>(TT("2DAnimationShader"));
	Device->AddShader<JGColorShader>(TT("ColorShader"));
}
