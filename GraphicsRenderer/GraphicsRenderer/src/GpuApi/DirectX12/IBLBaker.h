#pragma once
#include"Dx12Include.h"
#include"GraphicsResource.h"
#include"PipelineState.h"

namespace GR
{
	namespace Dx12
	{
		class ComputeCommander;
		class IBLBaker
		{
			static std::wstring m_SpBRDFShaderPath;
			static std::wstring m_SpMapShaderPath;
			static std::wstring m_IrrMapShaderPath;
		public:
			Texture m_SpecularBRDF;

			ComputePSO m_SpBRDFPSO;
			ComputePSO m_SpMapPSO;
			ComputePSO m_IrrMapPSO;


			void Init();
			bool Bake(ComputeCommander* commander, const Texture& Intexture,
				Texture* OutbrdfTexture, Texture* OutspMap, Texture* Outirrmap);
		};

	}
}