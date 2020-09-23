#pragma once

#ifdef GRAPHICSENGINE_EXPORT

#define GRAPHICSENGINE_API __declspec(dllexport)

#else

#define GRAPHICSENGINE_API __declspec(dllimport)

#endif
#include "CommonCore.h"
#include "Interface/GEObject.h"
#include "Interface/GraphicsDevice.h"
#include "Interface/GraphicsRenderer.h"
#include "Interface/Material.h"
#include "Interface/Scene.h"
#include "Interface/SceneObject.h"
#include "Interface/Screen.h"
#include "Interface/Material.h"
#include "Interface/Texture.h"
#include "Interface/SceneLight.h"

#define EMPTY_MATERIAL_3D    TT("__3D_NONEMATERIAL__")
#define EMPTY_MATERIAL_PAPER TT("__PAPER_NONEMATERIAL__")
#define EMPTY_MATERIAL_GUI   TT("__GUI_NONEMATERIAL__")


class GRAPHICSENGINE_API GraphicsConfig
{
public:
	std::string   graphics_api_dllPath;
	void* config_user_data = nullptr;
};


enum class EShadowFilter
{
	PCF,
	PCF3X3,
	Count
};
class GRAPHICSENGINE_API GraphicsSetting
{
public:
	static JVector2         ShadowResolution;
	static EShadowFilter    ShadowFilter;
	static const int CasCadeCount = 4;
	static JVector4  CasCadeRange;
	static JVector4  CasCadeDepthBias;

	// static STDLShadowFilter DLShadowFilter;
};







template class GE::GEObjectRef<GE::Screen>;
using ScreenRef =  GE::GEObjectRef<GE::Screen>;

template class GE::GEObjectRef<GE::Scene>;
using SceneRef = GE::GEObjectRef<GE::Scene>;


template class GE::GEObjectRef<GE::SceneObject>;
using SceneObjectRef = GE::GEObjectRef<GE::SceneObject>;

template class GE::GEObjectRef<GE::Material>;
using MaterialRef = GE::GEObjectRef<GE::Material>;


template class GE::GEObjectRef<GE::Texture>;
using TextureRef = GE::GEObjectRef<GE::Texture>;


template class GE::GEObjectRef<GE::DirectionalLight>;
using DirectionalLightRef = GE::GEObjectRef<GE::DirectionalLight>;


template class GE::GEObjectRef<GE::PointLight>;
using PointLightRef = GE::GEObjectRef<GE::PointLight>;

template class GE::GEObjectRef<GE::SpotLight>;
using SpotLightRef = GE::GEObjectRef<GE::SpotLight>;


// Scene 에서 카메라 설정, 렌더러 바인딩, 컬링, 마우스 픽킹 등등, 결과물은 텍스쳐 한장
// SceneObject 메시, 인스턴스, 트랜스폼
// Material 렌더링 플래그 정보, 설정값, PSO 
// GraphicsRenderer는 RootSig, 기본 PSO
// Renderer에 따라 Material 클래스도 틀려짐

// Texture 클래스 추가

enum class GraphicsAPI
{
	DirectX12,
	DirectX11,
	OpenGL,
	Vulkan
};

//#define INDEPENDENT_GRAPHICS_INTERFACE
class GRAPHICSENGINE_API GraphicsIF
{

public:
	//static void 
	static bool Create(GraphicsAPI api = GraphicsAPI::DirectX12);
	static bool RegisterInVaildTexture(const std::wstring& path);
	static TextureRef     GetInValidTexture();
	static ScreenRef      RequestScreen(HWND hWnd, int width, int height);
	static SceneObjectRef RequestSceneObject();
	static DirectionalLightRef RequestDirectionalLight();
	static PointLightRef       RequestPointLight();
	static SpotLightRef        RequestSpotLight();
	static SceneRef       RequestScene(GE::RendererMode mode);
	static MaterialRef    RequestMaterial(const std::wstring& matName, const GE::MaterialProperty& ppt = GE::MaterialProperty());
	static MaterialRef    GetMaterial(const std::wstring& matName);



	static bool PreCompileMaterial(GE::Material* m);
	static void       LoadTextures(const std::vector<std::wstring>& paths);
	static TextureRef LoadTexture(const std::wstring& path);
	static TextureRef GetTexture(const std::wstring& path);


	static void DrawCall(SceneRef& sceneRef);
	static void DrawCall(GE::Scene* scene);
	static void BeginDraw();
	static void EndDraw();

	static GraphicsAPI GetGraphicsAPI();
#ifdef INDEPENDENT_GRAPHICS_INTERFACE   
	static void Destroy();
#endif // !1

};

