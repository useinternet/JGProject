#pragma once
#include"../../EngineStatics/Engine.h"

class JGDeviceD;
/*
EnumClass : EJGBufferType
@e ConstantBuffer :  상수 버퍼
@e VertexBuffer   :  정점 버퍼
@e IndexBuffer    :  인덱스 버퍼
@e InstanceBuffer :  인스턴스 버퍼
*/
enum class ENGINE_EXPORT EJGBufferType
{
	ConstantBuffer  = 0,
	VertexBuffer   = 1,
	IndexBuffer    = 2,
	InstanceBuffer = 3
};
/*
EnumClass : EJGUsageType
@e Static  : 정적인 타입
@e Dynamic : 동적인 타입
*/
enum class ENGINE_EXPORT EJGUsageType
{
	Static  = 0,
	Dynamic = 1
};
/*
EnumClass : EJGCPUType
@e Access_Write : 쓰기로 접근
@e Access_Read  : 읽기로 접근
@e None         : 접근X
*/
enum class ENGINE_EXPORT EJGCPUType
{
	Access_Write = 0,
	Access_Read  = 1,
	None         = 2
};
/*
EnumClass : EJGMapType
@e Read  :  읽기 허용
@e Write :  쓰기 허용
@e Read_Write : 읽기 쓰기 모두 허용
@e Write_Discard : 쓰기 허용 ( 전의 데이터를 모두 지운다.)
@e Write_NoOverWrite : 쓰기 허용( 전의 데이터를 지우지 않는다.)
*/
enum class ENGINE_EXPORT EJGMapType
{
	Read              = 0,
	Write             = 1,
	Read_Write        = 2,
	Write_Discard     = 3,
	Write_NoOverWrite = 4
};
/*
Class : JGBuffer
@m ComPtr<ID3D11Buffer> m_Buffer     : 버퍼
@m D3D11_BUFFER_DESC    m_BufferDesc : 버퍼 설정 목록
@m EJGBufferType        m_BufferType : 버퍼 타입
@m size_t m_BindDataSize : 바인딩된 데이터 크기
*/
class ENGINE_EXPORT JGBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
	D3D11_BUFFER_DESC m_BufferDesc;
	EJGBufferType     m_BufferType;


	size_t m_BindDataSize;
	JGDeviceD* m_pDevice = nullptr;
public:
	JGBuffer();
	~JGBuffer();
	/*
	Exp : 버퍼 생성
	@param JGDeviceD* Device : DirectX 장치
	@param EJGBufferType bufferType : 버퍼 타입
	@param EJGUsageType usage : 버퍼 용도
	@param EJGCPUType CPUtype : CPU 허용 범위 (읽기, 쓰기)
	@param void* data : 바인딩할 데이터
	@param UINT size : 데이터 사이즈 ( sizeof(TYPE) * 요소 갯수 )  */
	bool CreateBuffer(JGDeviceD* Device, EJGBufferType bufferType, EJGUsageType usage, EJGCPUType CPUtype,
		void* data, size_t size);
	/*
	Exp : 데이터 쓰기 
	@param : EJGMapType type : 락 타입 
	@param : void* InData : 대체할 데이터 */
	bool Write(EJGMapType type,void* InData);
	/*
	Exp : 미구현 */
	bool Read(EJGMapType type, void* OutData);
	/*
	Exp : 버퍼의 포인터를 취득한다. */
	ID3D11Buffer* Get();
	/*
	Exp : 버퍼의 포인터 주소를 취득한다. */
	ID3D11Buffer** GetAddress();
	/*
	Exp : 버퍼타입을 얻어온다.*/
	EJGBufferType GetType();
	/*
	Exp : 버퍼설정목록을 가져온다. */
	void GetDesc(D3D11_BUFFER_DESC& Desc);
	/*
	Exp : 버퍼에 바인딩된 데이터의 사이즈를 가져온다. */
	size_t GetBindingDataSize();
};


