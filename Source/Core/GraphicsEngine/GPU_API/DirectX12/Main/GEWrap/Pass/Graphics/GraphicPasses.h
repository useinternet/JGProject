#pragma once
#include "GraphicPassDefine.h"


namespace DX12
{
	class Entry : public DX12_GraphicPass
	{
	public:
		static std::string ShaderPath;

	public:
		Entry();
	};



	class GBuffer : public DX12_GraphicPass
	{
	public:
		static std::string ShaderPath;

	public:
		GBuffer();
	};


	class Lighting : public DX12_GraphicPass
	{
	public:
		static std::string ShaderPath;

	public:
		Lighting();
	};



	class ShadowMap : public DX12_GraphicPass
	{
	public:
		static std::string ShaderPath;

	public:
		ShadowMap();
	};
	/*
	유저 기준   깊이  서로 비교 후 
	라이트 기준 깊이
	라이트 기분 worldviewproj
	*/

}