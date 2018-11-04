#include"TextureManager.h"
using namespace JGRC;
using namespace std;

TextureManager* TextureManager::Instance = nullptr;
TextureManager::TextureManager()
{

}
TextureManager::~TextureManager()
{

}

TextureManager* TextureManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new TextureManager;
	}
	return Instance;
}

void TextureManager::Release()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
JGTexture* TextureManager::CreateTexture()
{
	unique_ptr<JGTexture> t = make_unique<JGTexture>();
	JGTexture* result = t.get();
	m_vTextures.push_back(move(t));
	return result;
}