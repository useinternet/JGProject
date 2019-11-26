#pragma once


#include "DxObject.h"
#include <string>
#include <memory>
namespace RE
{
	class CommandList;
	class Texture;
	class TextureManager : public DxObject
	{
	public:
		TextureManager(const std::string& name = "TextureManager");
		void  InitLoad(const std::string& path);
		static const Texture& GetTexture(const std::string& name);
	private:
		bool TextureLoad(Texture& t, const fs::path& p, CommandList* cmdList);
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureCahce;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_Temp;
	};
}