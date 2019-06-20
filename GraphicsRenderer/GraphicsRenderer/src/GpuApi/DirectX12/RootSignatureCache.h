#pragma once
#include"Dx12Include.h"
#include"RootSignature.h"



namespace GR
{
	namespace Dx12
	{

		enum class ERootSignature
		{
			G_Common,
			C_InputOutput,
			C_GenerateMipMaps,
			C_IrrMap,
			C_SpecularBRDF,
			C_SpecularMap
		};
		enum EG_Common_RootParam
		{
			G_Common_RootParam_ObjectCB = 0,
			G_Common_RootParam_PassCB,
			G_Common_RootParam_Texture,
			G_Common_RootParam_CubeTexture,
			G_Common_RootParam_NumRootParam
		};
		enum EC_Equirect2Cube_RootParam
		{
			C_InputOutput_RootParam_InputTexture,
			C_InputOutput_RootParam_OutputTexture
		};
		enum EC_GernerateMipMaps
		{
			C_GenerateMipMaps_RootParam_GenerateMipsCB,
			C_GenerateMipMaps_RootParam_InputTexture,
			C_GenerateMipMaps_RootParam_OutputTexture
		};
		enum EC_IrrMap
		{
			EC_IrrMap_RootParam_InputTexture,
			EC_IrrMap_RootParam_OutputTexture
		};
		enum EC_SpecularBRDF
		{
			EC_SpecularBRDF_RootParam_OutputTexture
		};
		enum EC_SpecularMap
		{
			EC_SpecularMap_RootParam_Roughness,
			EC_SpecularMap_RootParam_InputTexture,
			EC_SpecularMap_RootParam_OutputTexture
		};
		enum ERootSignatureValue
		{
			G_Common_Value_NumTextureSlot = 8
		};
		class RootSignatureCache
		{
			std::map<ERootSignature, RootSignature> m_RootSignatureCache;
		public:
			RootSignature GetRootSignature(ERootSignature enumpass);
			std::mutex m_RootSigMutex;
		public:

		};
	}
}