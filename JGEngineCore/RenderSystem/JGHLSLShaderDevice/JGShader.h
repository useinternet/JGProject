#pragma once
#include"../../EngineStatics/Engine.h"
#include"JGShaderData.h"


class JGDeviceD;
class JGBufferManager;
class JGTexture;
enum class EJGMapType;


/*
Class : JGShader
@m string EntryPoint : 셰이더 시작 지점( Default : "main" ) 
@m string Version    : 컴파일할 셰이더 버전 ( Default : "5_0" )
@m ComPtr<ID3D11VertexShader> m_VertexShader : 버텍스셰이더 
@m ComPtr<ID3D11PixelShader> m_PixelShader : 픽셀 셰이더
@m vector<D3D11_INPUT_ELEMENT_DESC> m_ElementDesc : 셰이더 요소 목록 배열
@m vector<D3DSamplerState> m_vSamplerStates : 샘플러스테이트 배열 */
class ENGINE_EXPORT JGShader
{
protected:
	/*
	EnumClass : ESamplerType 
	@e Wrap : Wrap 형식
	@e Clamp : Clamp 형식 */
	enum class ESamplerType
	{
		Wrap, Clamp
	};
	/*
	ERegisterType 
	@e Vertex : VertexShader에 등록
	@e Pixel : PixelShader에 등록 */
	enum class ERegisterType
	{
		Vertex, Pixel
	};
private:
	typedef std::pair<const std::wstring, Microsoft::WRL::ComPtr<ID3D11SamplerState>> D3DSamplerState;
protected:
	const std::string EntryPoint = "main";
	const std::string Version    = "5_0";
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_InputLayout;
	std::vector<D3D11_INPUT_ELEMENT_DESC>      m_ElementDesc;
	std::vector<D3DSamplerState>               m_vSamplerStates;
	JGBufferManager* m_pBufferManager;
	JGDeviceD* m_pDevice;

	UINT m_VertexNumber = 0;
	UINT m_PixelNumber = 0;
public:
	JGShader();
	virtual ~JGShader();
	bool Create(JGDeviceD* Device, JGBufferManager* BufferManager);
	void Render(JGShaderData* Data,JGTexture* Texture);
protected:
	virtual bool CreateShader();
	virtual void JGShaderDataSetting(JGShaderData* Data);
	virtual void JGTextureSetting(JGTexture* Texture);
	/*
	Exp : 샘플러스테이트를 추가한다.
	@param const char* SemanticName : 시멘틱 이름
	@param UINT SemanticIndex       : 시멘틱 인덱스
	@param DXGI_FORMAT Format       : 형식
	@param UINT InputSlot           : 입력 슬롯
	@param D3D11_INPUT_CLASSIFICATION InputSlotClass : 입력슬롯클래스(인스턴스용)
	@param UINT InstanceDataStepRate : 인스턴스용(나중에)  */
	virtual void AddInputElement( const char* SemanticName, UINT SemanticIndex, DXGI_FORMAT Format, 
		UINT InputSlot = 0, D3D11_INPUT_CLASSIFICATION InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
		UINT InstanceDataStepRate = 0);

	/*
	Exp : 상수 버퍼를 추가한다. 
	@param const std::wstring& BufferName : 버퍼 이름
	@param EJGUsageType usage : 버퍼 용도
	@param EJGCPUType CPUtype : 버퍼 Cpu 타입
	@param size_t size : 바인딩될 데이터 타입 크기 */
	virtual void AddConstantBuffer(const std::wstring& BufferName, size_t size);
	/*
	Exp : 샘플러스테이트를 추가한다.
	@param : JGDeviceD* Device  : JGDevice 장치 포인터
	@param : const wstring name : 추가할 샘플러 이름 
	@param : ESamplerType       : 샘플러 타입  */
	virtual void AddSamplerState(JGDeviceD* Device,const std::wstring& name, ESamplerType type);
	/*
	Exp : 상수버퍼를 쓰고 셰이더에 등록한다. 
	@param const UINT idx : 상수버퍼 등록 위치 
	@param JGShaderData* Data : 셰이더 데이터
	@param EJGMapType : 쓰기 타입(어떻게쓸건지)
	@param ERegisterType : 버퍼 등록 타입 (버텍스 / 픽셀) */
	virtual void Write_RegisterBuffer(const std::wstring& BufferName, JGShaderData* Data, EJGMapType MapType, ERegisterType RegisterType);
	/*
	Exp : 버퍼 매니저를 가져온다. */
	JGBufferManager* GetBufferManager();
	/*
	Exp : JGDevice장치를 가져온다. */
	JGDeviceD* GetJGDevice();
	/*
	Exp : 버텍스셰이더포인터를 가져온다. */
	ID3D11VertexShader* GetVertexShader();
	/*
	Exp : 버텍스셰이더포인터 주소를 가져온다. */
	ID3D11VertexShader** GetVertexShaderAddress();
	/*
	Exp : 픽셀 셰이더 포인터를 가져온다. */
	ID3D11PixelShader* GetPixelShader();
	/*
	Exp : 픽셀 셰이더 포인터 주소를 가져온다. */
	ID3D11PixelShader** GetPixelShaderAddress();
	/*
	Exp :  입력 레이아웃 포인터를 가져온다. */
	ID3D11InputLayout* GetInputLayout();
	/*
	Exp : 입력 레이아웃 포인터주소를 가져온다. */
	ID3D11InputLayout** GetInputLayoutAddress();
	/*
	Exp : 요소 목록을 가져온다. */
	D3D11_INPUT_ELEMENT_DESC* GetInputElements();
	/*
	Exp : 추가한 요소 갯수를 가져온다. */
	UINT GetElementCount();
	/*
	Exp : 샘플러스테이트 포인터를 가져온다. */
	ID3D11SamplerState* GetSamplerState(const std::wstring& name);
	/*
	Exp : 샘플러 스테이트 포인터 주소를 가져온다. */
	ID3D11SamplerState** GetSamplerStateAddress(const std::wstring& name);
};