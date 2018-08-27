#include "JGShaderConstructor.h"
#include"JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"ShaderCode/SampleShader.h"
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
	Device->AddShader<SampleShader>(TT("SampleShader"));
}
