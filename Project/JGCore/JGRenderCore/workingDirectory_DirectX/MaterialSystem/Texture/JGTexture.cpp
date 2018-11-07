#include"JGTexture.h"
#include"DirectX/DirectX.h"
using namespace JGRC;
using namespace std;

unordered_map<string, ID3D11ShaderResourceView*> JGTexture::m_umSRVs;
DirectX* JGTexture::m_DX = nullptr;
JGTexture::JGTexture()
{
	m_DX = DirectX::GetInstance();
}
JGTexture::~JGTexture()
{
	for (auto& iter : m_vSRV)
	{
		iter->Release();
		iter = nullptr;
	}
}
void JGTexture::AddTexture(ID3D11ShaderResourceView* srv)
{
	m_vSRV.push_back(srv);
}
void JGTexture::AddTexture(const std::string& TexturePath)
{
	auto check = m_umSRVs.find(TexturePath);
	if (check != m_umSRVs.end())
	{
		m_vSRV.push_back(m_umSRVs[TexturePath]);
	}
	ID3D11ShaderResourceView* view = nullptr;
	HRESULT result = S_OK;
	wstring wpath;
	wpath.assign(TexturePath.begin(), TexturePath.end());
	result = D3DX11CreateShaderResourceViewFromFile(m_DX->GetDevice(), wpath.c_str(),
		nullptr, nullptr, &view, nullptr);
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::JGTexture", "Create Texture  " + TexturePath + "  Failed");
		return;
	}
	m_vSRV.push_back(view);
	m_umSRVs.insert(pair<string, ID3D11ShaderResourceView*>(TexturePath, view));
}
ID3D11ShaderResourceView** JGTexture::GetAddress(const uint idx)
{
	return &m_vSRV[idx];
}
vector<ID3D11ShaderResourceView*>& JGTexture::GetArray()
{
	return m_vSRV;
}
uint JGTexture::Size()
{
	return m_vSRV.size();
}