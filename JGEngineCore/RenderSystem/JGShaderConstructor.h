#pragma once
#include"../EngineStatics/Engine.h"

class JGHLSLShaderDevice;
class ENGINE_EXPORT JGShaderConstructor
{
public:
	JGShaderConstructor();
	JGShaderConstructor(const JGShaderConstructor& copy) = delete;
	~JGShaderConstructor();


	void LinkHLSLDevice(JGHLSLShaderDevice* Device);



};