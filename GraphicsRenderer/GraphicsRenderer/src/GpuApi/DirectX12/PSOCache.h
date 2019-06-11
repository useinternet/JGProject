#pragma once
#include"Dx12Include.h"
#include"PipelineState.h"



namespace GR
{
	namespace Dx12
	{
		class RootSignature;
		class PSOCache
		{
		public:
			enum EGraphicsPSO
			{
				GBUFFER,
				SKYBOX,
				TONEMAPPING,
				NUMPSO
			};
			enum EComputePSO
			{
				EQUIRECT2CUBE,
				GENERATEMIPMAPS,
				IRRADIANCEMAP,
				SPECULARBRDF,
				SPECULARMAP
			};
			enum EGraphicsOption
			{
				SOLID = 1,
				WIREFRAME = 2
			};
			enum
			{
				USE_NULL = 0
			};
			enum EGraphicsMacroOption
			{
				USE_ALBEDO = 1,
				USE_NORMAL = 2,
				USE_SPECULAR = 4,
				USE_METALLIC = 8,
				USE_ROUGHNESS = 16,
				USE_A0 = 32,
				USE_CUSTOM0 = 64,
				USE_CUSTOM1 = 128,

				// ¸Þ½Ã
				USE_SKINNED = 256
			};
			enum EComputeMacroOption
			{
			};
		private:
			using GraphicsPSOMap = std::unordered_map<uint32_t, GraphicsPSO>;
			using ComputePSOMap  = std::unordered_map<uint32_t, ComputePSO>;
			std::wstring   m_ShaderDirPath = L"./";
			GraphicsPSOMap m_GraphicsPSOMap;
			ComputePSOMap m_ComputePSOMap;
			std::mutex m_GraphicsPSOMutex;
			std::mutex m_ComputePSOMutex;
		public:
			void BindShaderDirPath(const std::wstring& path) {
				m_ShaderDirPath = path;
			}
			GraphicsPSO GetGraphicsPSO(RootSignature& rootSig, EGraphicsPSO enumPass, EGraphicsMacroOption option);
			ComputePSO  GetComputePSO(RootSignature& rootSig, EComputePSO enumCompute, EComputeMacroOption option);

			// Graphics
		private:
			void CreateGBufferPSO(GraphicsPSO& pso, EGraphicsMacroOption option);
			void CreateSkyPSO(GraphicsPSO& pso, EGraphicsMacroOption option);
			void CreateToneMappingPSO(GraphicsPSO& pso, EGraphicsMacroOption option);
			// Compute
		private:
			void CreateEquirect2Cube(ComputePSO& pso, EComputeMacroOption option);
			void CreateGenerateMipMaps(ComputePSO& pso, EComputeMacroOption option);
			void CreateIrradianceMap(ComputePSO& pso, EComputeMacroOption option);
			void CreateSpecularBRDF(ComputePSO& pso, EComputeMacroOption option);
			void CreateSpecularMap(ComputePSO& pso, EComputeMacroOption option);
			void DefaultBindComputeShader(ComputePSO& pso, const std::wstring& shadername);
		};

	}
}