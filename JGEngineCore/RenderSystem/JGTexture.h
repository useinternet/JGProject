#pragma once
#include"../EngineStatics/Engine.h"


//
class JGDeviceD;


/*
Class : JGTexture
@m vector<ID3D11ShaderResourceView*> m_vTexture     : 텍스쳐 배열
@m vector<D3DX11_IMAGE_INFO>         m_vTextureInfo : 텍스쳐 정보 배열
*/
class ENGINE_EXPORT JGTexture
{
private:
	 std::vector<ID3D11ShaderResourceView*> m_vTexture;
	 std::vector<D3DX11_IMAGE_INFO> m_vTextureInfo;
public:
	JGTexture();
	~JGTexture();
	/*
	Exp : 텍스쳐 추가
	@param JGDeviceD* Device   : DirectX 장치
	@param const wstring& Path : 텍스쳐 디렉토리 경로  */
	bool Add(JGDeviceD* Device,const std::wstring& Path);
	/*
	Exp : 텍스쳐를 얻어온다.
	@param const UINT idx : 얻어올 텍스쳐 인덱스 */
	ID3D11ShaderResourceView* Get(const UINT idx);
	/*
	Exp : 텍스쳐배열의 시작부분을 얻어온다. */
	ID3D11ShaderResourceView* GetEntry();
	/*
	Exp : 텍스쳐정보를 얻어온다.
	@param const UINT idx : 얻어올 텍스쳐정보 인덱스 */
	D3DX11_IMAGE_INFO& GetInformation(const UINT idx);
};