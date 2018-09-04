#pragma once
#include"Engine.h"


class StaticMesh2DComponent;
class JGTexture;
class JG2DMesh;
class JGDeviceD;
class JGBufferManager;
class JGFontLoader;
enum class EPivot;


class ENGINE_EXPORT ConsructObject
{
public:
	virtual ~ConsructObject();
};
/*
Class : StaticMesh2DObject 
Exp : 정적 2DMesh / 컴포넌트에 전달하기위한 클래스.
@m wstring               ShaderName : 적용할 셰이더 이름
@m shared_ptr<JGTexture> Texture    : 텍스쳐 
@m shared_ptr<JG2DMesh>  Mesh       : 메쉬 */
class ENGINE_EXPORT StaticMesh2DObject : public ConsructObject
{
public:
	std::wstring ShaderName = TT("None");
	std::shared_ptr<JGTexture> Texture;
	std::shared_ptr<JG2DMesh>  Mesh;
	virtual ~StaticMesh2DObject();
};
/*
Class : TextObject 
Exp : 텍스트 컴포넌트에게 전달하기위한 오브젝트 
@m wstring ShaderName : 적용할 셰이더 이름
@m shared_ptr<JGTexture> Texture : 적용할 폰트 텍스쳐
@m string FontPath : 적용할 폰트 정보 경로 */
class ENGINE_EXPORT TextObject : public ConsructObject
{
public:
	std::wstring ShaderName = TT("None");
	std::shared_ptr<JGTexture> Texture;
	std::string FontPath = "None";
	virtual ~TextObject();
};

/*
Class : JGConstructHelper 
@m vector<shared_ptr<StaticMesh2DObject>> m_vStaticMesh2DObject : 스테틱 2DMesh의 주소값을 보관하기 위한 배열 */
class ENGINE_EXPORT JGConstructHelper
{
public:
	std::vector<std::shared_ptr<ConsructObject>> m_vStaticMesh2DObject;
public:
	JGConstructHelper();
	static JGConstructHelper* GetInstance();
public:
	class StaticMesh2D
	{
	public:
		StaticMesh2DObject* Object;
		bool Success = true;
	public:
		StaticMesh2D() = delete;
		StaticMesh2D(const StaticMesh2D& copy) = delete;
	
		/*
		@param JGDevice* Device : GetDevice() 호출 
		@param JGBufferManager : GetBufferManager() 호출 
		@param const wstring& ComponentName : GetComponentName() 호출 
		@param EPivot pivot : 기준점
		@param const std::wstring& TexturePath : 텍스쳐 경로
		@param const std::wstring& ShaderName = 적용할 셰이더 이름(기본적으로 각각에 맞게 설정되어있음) */
		StaticMesh2D(JGDeviceD* Device, JGBufferManager* BufferManager, const std::wstring& ComponentName,
			EPivot pivot, const std::wstring& TexturePath, const std::wstring& ShaderName = TT("2DSpriteShader"));
	};
	class TextFont
	{
	public:
		TextObject* Object;
		bool Success = true;
	public:
		TextFont() = delete;
		TextFont(const TextFont& copy) = delete;
		/*
		@param JGDevice* Device : GetDevice() 호출
		@param const string& FontPath :  폰트 정보 경로 
		@param const wstring& FontTexturePath : 폰트 텍스쳐 경로 
		@param const wstring& ShaderName : 폰트에 적용할 셰이더 이름. */
		TextFont(JGDeviceD* Device, const std::string& FontPath, const std::wstring& FontTexturePath,
			const std::wstring& ShaderName = TT("TextShader"));
	};
};