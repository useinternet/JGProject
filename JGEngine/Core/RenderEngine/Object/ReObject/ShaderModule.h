#pragma once




#include "Object/ReObject.h"

namespace RE
{
	/*
	-- ShaderModule, GlobalShaderDataArea, GlobalShaderFuncArea, ShaderDataAream,ShaderFuncArea 
	1. ShaderModule 하나당 소스코드 하나
	2. Shader 요소(그래픽) -> 코드 병함
	  - inputlayout
	  - constant buffer
	  - structed buffer
	  - texture
	  - unordered texture
	  - var
	  - const, static 
	  - sampler
	  - 
	3. 컴파일 단계별


     ShaderData
	 int
	 str



	*/
	class ShaderModule : public ReObject
	{
	public:
		struct InputLayout
		{
			
			ShaderData data;
			DXGI_FORMAT format;
		};
		struct ConstantBuffer
		{
			// 바인딩 번호
			// Data
			//
		};
	public:
		

	};


	class GraphicsShaderModule : public ShaderModule
	{


	};
	class ComputeShaderModule : public ShaderModule
	{




	};

}

