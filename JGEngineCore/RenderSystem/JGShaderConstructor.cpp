#include "JGShaderConstructor.h"
#include"JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"ShaderCode/JG2DSpriteShader.h"
#include"ShaderCode/JGTextShader.h"

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
}
