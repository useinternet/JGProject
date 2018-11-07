#pragma once
#include"MaterialSystem/Material.h"

namespace JGRC
{
	class SRSMaterial : private Material
	{
	private:
		std::unique_ptr<jgMatrix4x4> m_wvpMatrix;
	public:
		SRSMaterial();
		virtual ~SRSMaterial();
		void  SetMesh(Mesh* mesh);
		real* GetParam(const std::string& paramName);
		void  SetParam(const std::string& paramName, void* Data);
		uint  GetParamCount(const std::string& paramName);
		void Render(ID3D11ShaderResourceView* WorldPos, ID3D11ShaderResourceView* Albedo,
			ID3D11ShaderResourceView* Normal, ID3D11ShaderResourceView* Depth);
	};
}
