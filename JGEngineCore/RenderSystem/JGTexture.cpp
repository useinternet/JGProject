#include"JGTexture.h"
#include"JGDeviceD.h"
#include"../EngineStatics/JGLog.h"
using namespace Microsoft::WRL;
using namespace std;

JGTexture::JGTexture()
{

}
JGTexture::~JGTexture()
{
	for (size_t i = 0; i < m_vTexture.size(); ++i)
	{
		m_vTexture[i]->Release();
		m_vTexture[i] = nullptr;
	}
}

bool JGTexture::Add(JGDeviceD* Device, const wstring& Path)
{
	HRESULT result = S_OK;
	D3DX11_IMAGE_INFO TextureInfo;
	ID3D11ShaderResourceView* texture;


	// 이미지 로드
	result = D3DX11CreateShaderResourceViewFromFile(
		Device->GetDevice(), Path.c_str(), nullptr, nullptr,
		&texture, nullptr);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Warning, TT("Create Texture Failed. / %s"), Path.c_str());
		return false;
	}
	// 이미지 정보 로드
	result = D3DX11GetImageInfoFromFile(Path.c_str(), nullptr, &TextureInfo, nullptr);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Warning, TT("Create TextureInformation Failed. / %s"), Path.c_str());
		return false;
	}

	// 정보 저장
	m_vTexture.push_back(move(texture));
	m_vTextureInfo.push_back(move(TextureInfo));
	return true;
}

ID3D11ShaderResourceView* JGTexture::Get(const UINT idx)
{
	if (m_vTexture.size() <= idx)
	{
		return nullptr;
	}
	return m_vTexture[idx];
}
ID3D11ShaderResourceView* JGTexture::GetEntry()
{
	if (m_vTexture.size() == 0)
	{
		return nullptr;
	}
	return m_vTexture[0];
}

D3DX11_IMAGE_INFO& JGTexture::GetInformation(const UINT idx)
{
	return m_vTextureInfo[idx];
}
