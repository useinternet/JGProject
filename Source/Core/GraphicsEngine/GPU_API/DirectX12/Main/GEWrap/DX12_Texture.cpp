#include "pch.h"
#include "DX12_Texture.h"

using namespace std;

namespace DX12
{
	void* DX12_Texture::GetUserTextureData() const
	{
		return (void*)&m_Texture;
	}
	const GE::TextureInfo& DX12_Texture::GetInfo() const
	{
		return m_Info;

		// TODO: 여기에 return 문을 삽입합니다.
	}
	bool DX12_Texture::IsValid() const
	{
		return m_Texture.IsValid();
	}
}