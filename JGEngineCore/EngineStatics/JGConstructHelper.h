#pragma once
#include"Engine.h"
#include"../RenderSystem/JGMaterial/JG2DMesh.h"
class StaticMesh2DComponent;
class JGTexture;
class JGDeviceD;
class JGBufferManager;
class JGFontLoader;
class JGSound;
class JGVector2D;
enum class EPivot;


class ENGINE_EXPORT ConstructObject
{
public:
	virtual ~ConstructObject();
};
/*
Class : StaticMesh2DObject 
Exp : 정적 2DMesh / 컴포넌트에 전달하기위한 클래스.
@m wstring               ShaderName : 적용할 셰이더 이름
@m shared_ptr<JGTexture> Texture    : 텍스쳐 
@m shared_ptr<JG2DMesh>  Mesh       : 메쉬 */
class ENGINE_EXPORT StaticMesh2DObject : public ConstructObject
{
public:
	std::wstring ShaderName = TT("None");
	std::shared_ptr<JGTexture> Texture;
	std::shared_ptr<JG2DMesh>  Mesh;
	std::unique_ptr<EPivot> Pivot;
	virtual ~StaticMesh2DObject();
};
class ENGINE_EXPORT AnimationMesh2DObject : public StaticMesh2DObject
{
public:
	size_t TotalFrame;
	size_t WidthFrame;
	size_t HeightFrame;
	float IncreaseWidth;
	float IncreaseHeight;
	EReverse ReverseType;
	virtual ~AnimationMesh2DObject();
};
/*
Class : TextObject 
Exp : 텍스트 컴포넌트에게 전달하기위한 오브젝트 
@m wstring ShaderName : 적용할 셰이더 이름
@m shared_ptr<JGTexture> Texture : 적용할 폰트 텍스쳐
@m string FontPath : 적용할 폰트 정보 경로 */
class ENGINE_EXPORT TextObject : public ConstructObject
{
public:
	std::wstring ShaderName = TT("None");
	std::shared_ptr<JGTexture> Texture;
	std::string FontPath = "None";
	virtual ~TextObject();
};

class ENGINE_EXPORT SoundObject : public ConstructObject
{
public:
	virtual ~SoundObject();
};


/*
Class : JGConstructHelper 
@m vector<shared_ptr<StaticMesh2DObject>> m_vStaticMesh2DObject : 스테틱 2DMesh의 주소값을 보관하기 위한 배열 */
class ENGINE_EXPORT JGConstructHelper
{
private:
	static JGDeviceD* m_pDevice;
	static JGBufferManager* m_BufferManager;

public:
	std::vector<std::shared_ptr<ConstructObject>> m_vConstructObject;
public:
	JGConstructHelper() = delete;
	JGConstructHelper(JGDeviceD* Device, JGBufferManager* BufferManager);
	static JGConstructHelper* GetInstance();
public:
	class ENGINE_EXPORT StaticMesh2D
	{
	private:
		friend JGConstructHelper;
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
		StaticMesh2D(const std::wstring& ComponentName, EPivot pivot, const std::wstring& TexturePath, EReverse Reverse = EReverse::Default,
			const EJGUsageType usageType = EJGUsageType::Static,const std::wstring& ShaderName = TT("2DSpriteShader"));
	};

	class ENGINE_EXPORT AnimationMesh2D
	{
	private:
		friend JGConstructHelper;
	public:
		AnimationMesh2DObject * Object;
		bool Success = true;
	public:
		AnimationMesh2D() = delete;
		AnimationMesh2D(const AnimationMesh2D& copy) = delete;
		/*
		@param JGDevice* Device : GetDevice() 호출
		@param JGBufferManager : GetBufferManager() 호출
		@param const wstring& ComponentName : GetComponentName() 호출
		@param EPivot pivot : 기준점
		@param const size_t TotalFrame : 총 프레임 수
		@param const size_t WidthFrame : 가로 프레임 수
		@param const size_t HeightFrame :  세로 프레임 수
		@param const std::wstring& TexturePath : 텍스쳐 경로
		@param const std::wstring& ShaderName = 적용할 셰이더 이름(기본적으로 각각에 맞게 설정되어있음) */
		AnimationMesh2D(const std::wstring& ComponentName, EPivot pivot,const size_t TotalFrame, const size_t WidthFrame, const size_t HeightFrame,
			const std::wstring& TexturePath, EReverse Reverse = EReverse::Default,const EJGUsageType usageType = EJGUsageType::Static, const std::wstring& ShaderName = TT("2DAnimationShader"));
	};
	class ENGINE_EXPORT TextFont
	{
	private:
		friend JGConstructHelper;
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
		TextFont(const std::string& FontPath, const std::wstring& FontTexturePath, const std::wstring& ShaderName = TT("TextShader"));
	};
};