#pragma once
#include"../../EngineStatics/Engine.h"



class JGBuffer;
class JGDeviceD;
enum class JGBufferType;
enum class JGUsageType;
enum class JGCPUType;
enum class JGMapType;


/*
Class : JGBufferManager 
@m BufferArray m_JGBuffers : 버퍼 배열(std::map)  */
class ENGINE_EXPORT JGBufferManager
{
private:
	typedef std::map<const std::wstring, std::shared_ptr<JGBuffer>> BufferArray;
private:
	BufferArray m_JGBuffers;
	JGDeviceD* m_pDevice;

public:
	JGBufferManager();
	JGBufferManager(JGBufferManager& copy) = delete;
	~JGBufferManager();
	/*
	Exp : 버퍼 매니저 초기화 
	@param JGDeviceD* Device : DirectX 장치 */
	void InitBufferManager(JGDeviceD* Device);
	/*
	Exp : 버퍼 추가 
	@param const std::wstring& name : 버퍼 이름
	@param JGBufferType bufferType : 버퍼 타입
	@param JGUsageType usage : 버퍼 용도
	@param JGCPUType CPUtype : CPU 허용 범위 (읽기, 쓰기)
	@param void* data : 바인딩할 데이터
	@param UINT size : 데이터 사이즈 ( sizeof(TYPE) * 요소 갯수 )  */
	bool AddBuffer(const std::wstring& name, JGBufferType bufferType, JGUsageType usage, JGCPUType CPUtype,
		void* data, UINT size);
	/*
	Exp : 버퍼 쓰기
	@param const std::wstring& name : 버퍼 이름
	@param : JGMapType type : 락 타입
	@param : void* InData : 대체할 데이터 */
	bool Write(const std::wstring& name, JGMapType type, void* InData);

	/*
	Exp : 해당 이름의 버퍼를 얻어온다.
	@param const std::wstring& name : 버퍼 이름 */
	JGBuffer* GetBuffer(const std::wstring& name);
};