#pragma once


#include "DxObject.h"
#include <string>
#include <memory>
namespace RE
{
	class CommandList;
	class Texture;
	class TextureEstimate
	{
	public:
		std::vector<Texture*> requested_t;
		Texture* origin_t = nullptr;
		int value_index = -1;
	};
	class TextureManager : public DxObject
	{
	public:
		TextureManager(const std::string& name = "TextureManager");
		void  InitLoad(const std::string& path);
		void  Update();
		static void  RequestLoadAndGetTexture(const std::string& name, Texture* t = nullptr);
		static void  RequestCancelLoadTexture(Texture* t);
		static const Texture& GetTexture(const std::string& name);
		static bool RegisterTexture(const std::string& name, const Texture& t);
		static bool UnRegisterTexture(const std::string& name);
	private:
		bool TextureLoad(Texture& t, const fs::path& p, CommandList* cmdList);
	private:
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureCahce;
		std::unordered_map<std::string, std::shared_ptr<TextureEstimate>> m_TextureEstimates;
		std::queue<TextureEstimate*> m_SumittedTextureQueue;
	};
}