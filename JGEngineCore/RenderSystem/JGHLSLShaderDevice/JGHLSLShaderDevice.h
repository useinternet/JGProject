#pragma once
#include"../../EngineStatics/Engine.h"
#include"JGShader.h"

class JGDeviceD;
class JGBufferManager;
class JGTexture;

/*
Class : JGHSLSLShaderDevice
@m map<const wstring, shared_ptr<JGShader>> m_mShaders : 셰이더 배열 */
class ENGINE_EXPORT JGHLSLShaderDevice
{
private:
	std::map<const std::wstring, std::shared_ptr<JGShader>> m_mShaders;

	JGDeviceD*       m_pDevice;
	JGBufferManager* m_pBufferManager;
public:
	JGHLSLShaderDevice();
	~JGHLSLShaderDevice();
	/*
	Exp : JGHLSLShaderDevice 를 생성한다. */
	bool CreateDevice(JGDeviceD* Device, JGBufferManager* BufferManager);
	/*
	Exp : 특정 셰이더를 렌더링 시킨다. 
	@param const wstring name       : 동작시킬 셰이더 이름 
	@param void*         Data       : 연동 시킬 파라미터 데이터
	@param JGTexture*    Texture    : 연동시킬 텍스쳐 클래스 
	@param size_t        IndexCount : 렌더링할 점의 갯수   */
	void Render(const std::wstring& name, JGShaderData* Data, JGTexture* Texture, size_t IndexCount);


	/*
	Exp : 셰이더를 추가한다. 
	@template Shadertype : 추가할 셰이더 타입
	@param const wstring name : 추가할 셰이더 이름 */
	template<class Shadertype>
	bool AddShader(const std::wstring& name);

	JGShader* GetShader(const std::wstring& name);


	static JGHLSLShaderDevice* GetInstance();
};


template<class Shadertype>
bool JGHLSLShaderDevice::AddShader(const std::wstring& name)
{
	std::unique_ptr<JGShader> shader = std::make_unique<Shadertype>();

	bool result = shader->Create(m_pDevice, m_pBufferManager);
	if (!result)
	{
		return false;
	}
	m_mShaders.insert(std::pair <const std::wstring,
		std::unique_ptr<JGShader>>(name, move(shader)));
	return true;
}