#pragma once
#include"../../EngineStatics/Engine.h"



class JGBuffer;
class JGDeviceD;
enum class EJGBufferType;
enum class EJGUsageType;
enum class EJGCPUType;
enum class EJGMapType;


/*
Class : JGBufferManager 
@m BufferArray m_JGBuffers : 버퍼 배열(std::map)  */
class ENGINE_EXPORT JGBufferManager
{
private:
	typedef std::map<const std::wstring, std::shared_ptr<JGBuffer>> BufferArray;
private:
	BufferArray m_JGBuffers;

	JGDeviceD* m_pDevice = nullptr;
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
	@param EJGBufferType bufferType : 버퍼 타입
	@param EJGUsageType usage : 버퍼 용도
	@param EJGCPUType CPUtype : CPU 허용 범위 (읽기, 쓰기)
	@param void* data : 바인딩할 데이터
	@param size_t size : 데이터 사이즈 ( sizeof(TYPE) * 요소 갯수 )  */
	bool AddBuffer(const std::wstring& name, EJGBufferType bufferType, EJGUsageType usage, EJGCPUType CPUtype,
		void* data, size_t size);
	/*
	Exp : 버퍼 쓰기
	@param const std::wstring& name : 버퍼 이름
	@param : EJGMapType type : 락 타입
	@param : void* InData : 대체할 데이터 */
	bool Write(const std::wstring& name, EJGMapType type, void* InData);
	/*
	Exp : 해당 이름의 버퍼를 얻어온다.
	@param const std::wstring& name : 버퍼 이름 */
	JGBuffer* GetBuffer(const std::wstring& name);
	/*
	Exp : 바인딩된 데이터의 크기를 가져온다. */
	size_t GetBindingDataSize(const std::wstring& name);
	/*
	Exp : 현제 버퍼 매니져에 등록되어져있는 버퍼 이름 인지 체크한다. */
	bool DuplicateCheck(const std::wstring& name);

	/*
	Exp : 해당 버퍼를 삭제한다. */
	void DestroyBuffer(const std::wstring& BufferName);
};