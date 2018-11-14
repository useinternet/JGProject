#pragma once
#include"DirectX/JGRCObject.h"
#include"JGBufferEnum.h"
namespace JGRC
{
	/*
	Class : JGBuffer
	@m ComPtr<ID3D11Buffer> m_Buffer     : 버퍼
	@m D3D11_BUFFER_DESC    m_BufferDesc : 버퍼 설정 목록
	@m EJGBufferType        m_BufferType : 버퍼 타입
	@m EJGBufferState       m_BufferCurrentState : 현제 버퍼 상태
	@m size_t m_BindDataSize : 바인딩된 데이터 크기
	*/
	class DirectX;
	class CORE_EXPORT JGBuffer : public JGRCObject
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
		D3D11_BUFFER_DESC m_BufferDesc;
		EBufferType     m_BufferType;

		size_t m_BindDataSize;
		static DirectX* m_pDirectX;
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
		bool CreateBuffer(EBufferType bufferType, EUsageType usage, ECPUType CPUtype,
			void* data, size_t size);
		/*
		Exp : 데이터 쓰기
		@param : EJGMapType type : 락 타입
		@param : void* InData : 대체할 데이터 */
		bool Write(EMapType type, void* InData);

		/*
		Exp : 미구현 */
		bool Read(EMapType type, void* OutData);
		/*
		Exp : 버퍼의 포인터를 취득한다. */
		ID3D11Buffer* Get();
		/*
		Exp : 버퍼의 포인터 주소를 취득한다. */
		ID3D11Buffer** GetAddress();
		/*
		Exp : 버퍼타입을 얻어온다.*/
		EBufferType GetType();
		/*
		Exp : 버퍼설정목록을 가져온다. */
		void GetDesc(D3D11_BUFFER_DESC& Desc);
		/*
		Exp : 버퍼에 바인딩된 데이터의 사이즈를 가져온다. */
		size_t GetBindingDataSize();
	};



}
