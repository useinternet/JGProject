#pragma once
#include "Object/ReObject.h"
#include "ShaderDataType.h"


namespace RE
{
	class ShaderObject : public ReObject
	{
	public:
	
	};

	class GbufferObject : public ShaderObject
	{
	public:
		std::shared_ptr<STStruct> m_Object;
		std::shared_ptr<STStruct> m_Camera;
		std::shared_ptr<STStruct> m_Skinned;

	};

	class LightObject : public ShaderObject
	{

	};

	class GlobalObject : public ShaderObject
	{

	};


}