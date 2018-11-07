#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"


namespace JGRC
{
	class CORE_EXPORT JGTexture
	{
		static std::unordered_map<std::string, ID3D11ShaderResourceView*> m_umSRVs;
		static class DirectX* m_DX;
	private:
		std::vector<ID3D11ShaderResourceView*> m_vSRV;
	public:
		JGTexture();
		~JGTexture();
		void AddTexture(ID3D11ShaderResourceView* srv);
		void AddTexture(const std::string& TexturePath);
		ID3D11ShaderResourceView** GetAddress(const uint idx);
		std::vector<ID3D11ShaderResourceView*>& GetArray();
		uint Size();
	};
}

