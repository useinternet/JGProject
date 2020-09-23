#include "pch.h"
#include "GraphicsEngine.h"
#include "GraphicsIF.h"

using namespace std;
namespace GE
{
	static GraphicsEngine* g_GE = nullptr;



	void GraphicsEngine::Awake()
	{
		if (g_GE == nullptr) g_GE = this;
		else assert("GraphicsEngine::Awake");
		GraphicsIF::Create();


		GlobalSharedData::GetEventManager()->RegisterEvent(
			TT("GE::GraphicsEngine::GraphicsIF::BeginDraw"), Event<void>(
				[&]()
		{
			GraphicsIF::BeginDraw();
		}));

		GlobalSharedData::GetEventManager()->RegisterEvent(
			TT("GE::GraphicsEngine::GraphicsIF::EndDraw"), Event<void>(
				[&]()
		{
			GraphicsIF::EndDraw();
		}));



	}

	void GraphicsEngine::Start()
	{

	}

	void GraphicsEngine::Update()
	{

	}

	void GraphicsEngine::Destroy()
	{
		{
			for (auto& mat : m_MaterialPool)
			{
				delete mat.second;
				mat.second = nullptr;
			}

			for (auto& t : m_TexturePool)
			{
				delete t.second;
				t.second = nullptr;
			}

			if (m_GraphicsRenderer)
			{
				delete m_GraphicsRenderer;
				m_GraphicsRenderer = nullptr;
			}

			if (m_GraphicsDevice)
			{
				m_GraphicsDevice->Destroy();
				delete m_GraphicsDevice;
				m_GraphicsDevice = nullptr;
			}

		}

		GE::g_GE->m_GraphicsAPIPlugin.reset();
		GE::g_GE->m_GraphicsAPIPlugin = nullptr;
	}

	void GraphicsEngine::ReceiveMessage(const Message& msg)
	{
		switch (msg.msgID)
		{
		case GraphicsMessage::Msg_BeginDraw:
			GraphicsIF::BeginDraw();

			break;


		case GraphicsMessage::Msg_EndDraw:
			GraphicsIF::EndDraw();
			break;
		}

	}

}


/* API 

*/
JVector2      GraphicsSetting::ShadowResolution = JVector2(1024, 1024);
EShadowFilter GraphicsSetting::ShadowFilter     = EShadowFilter::PCF3X3;
JVector4 GraphicsSetting::CasCadeRange     = JVector4(100.0f, 250.0f, 500.0f, 10000.0f);
JVector4 GraphicsSetting::CasCadeDepthBias = JVector4(0.000000125f, 0.00000017f, 0.0000007f, 0.000001f);

bool GraphicsIF::Create(GraphicsAPI api)
{
	std::string dllName;

	switch (api)
	{
	case GraphicsAPI::DirectX12:
		dllName = "DirectX12.dll";
		break;
	case GraphicsAPI::DirectX11:
	case GraphicsAPI::OpenGL:
	case GraphicsAPI::Vulkan:
		assert("not supported GraphicsAPI");
	}
#ifdef INDEPENDENT_GRAPHICS_INTERFACE    
	GE::g_GE = new GE::GraphicsEngine;
#endif // !1
	// 
	GE::g_GE->m_GraphicsAPIPlugin = make_unique<Plugin>();
	GE::g_GE->m_GraphicsAPIPlugin->Load(dllName);


	if (!GE::g_GE->m_GraphicsAPIPlugin->IsValid())
	{
		assert("failed Create GraphicsInterface");
	}
#ifndef INDEPENDENT_GRAPHICS_INTERFACE    
	LINK_GLOBAL_SHARED_DATA(GE::g_GE->m_GraphicsAPIPlugin);
#endif // !1




	GE::g_GE->m_GraphicsDevice   = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, GraphicsDevice);
	GE::g_GE->m_GraphicsDevice->Create(nullptr);


	GE::g_GE->m_GraphicsRenderer = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, GraphicsRenderer);

	GE::MaterialProperty ppt;
	ppt.rendererMode = GE::RendererMode::_3D;
	GraphicsIF::PreCompileMaterial(RequestMaterial(EMPTY_MATERIAL_3D, ppt).Get());

	ppt.rendererMode = GE::RendererMode::Paper;
	GraphicsIF::PreCompileMaterial(RequestMaterial(EMPTY_MATERIAL_PAPER, ppt).Get());

	ppt.rendererMode = GE::RendererMode::GUI;
	GraphicsIF::PreCompileMaterial(RequestMaterial(EMPTY_MATERIAL_GUI, ppt).Get());

	return true;
}

bool GraphicsIF::RegisterInVaildTexture(const std::wstring& path)
{
	fs::path p(path);
	if (!fs::exists(p)) return false;


	GE::g_GE->m_InVaildTexture = LoadTexture(path).Get();


	return true;
}

TextureRef GraphicsIF::GetInValidTexture()
{
	return move(TextureRef(GE::g_GE->m_InVaildTexture, GE::g_GE, false));
}

ScreenRef GraphicsIF::RequestScreen(HWND hWnd, int width, int height)
{
	GE::Screen* screen = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, Screen);
	screen->Init(hWnd, width, height);
	ScreenRef ref(screen, GE::g_GE);
	return move(ref);
}

SceneObjectRef GraphicsIF::RequestSceneObject()
{
	GE::SceneObject* obj = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, SceneObject);
	//TODO( Defulat)

	SceneObjectRef ref(obj, GE::g_GE);
	return move(ref);
}
DirectionalLightRef GraphicsIF::RequestDirectionalLight()
{
	GE::DirectionalLight* light = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, DirectionalLight);

	DirectionalLightRef ref(light, GE::g_GE);
	return move(ref);
}
PointLightRef       GraphicsIF::RequestPointLight()
{
	GE::PointLight* light = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, PointLight);

	PointLightRef ref(light, GE::g_GE);
	return move(ref);
}
SpotLightRef        GraphicsIF::RequestSpotLight()
{
	GE::SpotLight* light = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, SpotLight);

	SpotLightRef ref(light, GE::g_GE);
	return move(ref);
}
SceneRef GraphicsIF::RequestScene(GE::RendererMode mode)
{
	GE::Scene* obj = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, Scene);
	//TODO( Defulat)
	SceneRef ref(obj, GE::g_GE);
	obj->m_RendererMode = mode;
	return move(ref);
}

MaterialRef GraphicsIF::RequestMaterial(const std::wstring& matName, const GE::MaterialProperty& ppt)
{
	
	// 이미 존재하는 머터리얼이라면 생성 X
	{
		std::shared_lock<std::shared_mutex> lock(GE::g_GE->m_MaterialMutex);
		auto iter = GE::g_GE->m_MaterialPool.find(matName);
		if (iter != GE::g_GE->m_MaterialPool.end())
		{
			return move(MaterialRef());
		}
	}


	GE::Material* mat = LOAD_GRAPHICS_INTERFACE(GE::g_GE->m_GraphicsAPIPlugin, Material);
	MaterialRef ref(mat, GE::g_GE, false);
	
	{
		lock_guard<std::shared_mutex> lock(GE::g_GE->m_MaterialMutex);
		GE::g_GE->m_MaterialPool.emplace(matName, mat);
	}
	ref->m_Name = matName;
	ref->SetMaterialProperty(ppt);
	return move(ref);
}

MaterialRef GraphicsIF::GetMaterial(const std::wstring& matName)
{
	bool is_find = false;
	shared_lock<shared_mutex> lock(GE::g_GE->m_MaterialMutex);
	auto iter = GE::g_GE->m_MaterialPool.find(matName);
	is_find = iter != GE::g_GE->m_MaterialPool.end();


	if (is_find)
	{
		MaterialRef ref(iter->second, GE::g_GE, false);
		return move(ref);
	}
	else
	{
		return MaterialRef();
	}
}

bool GraphicsIF::PreCompileMaterial(GE::Material* m)
{
	return GE::g_GE->m_GraphicsRenderer->CompileMaterial(m);
}
void GraphicsIF::LoadTextures(const vector<std::wstring>& paths)
{
	vector<std::wstring> pathList;
	{
		shared_lock<shared_mutex> lock(GE::g_GE->m_TextureMutex);
		for (size_t i = 0; i < paths.size(); ++i)
		{
			auto iter = GE::g_GE->m_TexturePool.find(paths[i]);
			if (iter == GE::g_GE->m_TexturePool.end()) pathList.push_back(paths[i]);
		}
	}


	vector<GE::Texture*> textureArray;
	GE::g_GE->m_GraphicsDevice->LoadTextures(pathList, textureArray);

	{
		lock_guard<shared_mutex> lock(GE::g_GE->m_TextureMutex);
		
		for (size_t i = 0; i < textureArray.size(); ++i)
		{
			if (textureArray[i])
			{
				GE::g_GE->m_TexturePool[paths[i]] = textureArray[i];
			}
		
		}
	}

}
TextureRef GraphicsIF::LoadTexture(const std::wstring& path)
{
	bool is_find = false;
	{
		// 이미 있는 텍스쳐라면 그대로 Return
		shared_lock<shared_mutex> lock(GE::g_GE->m_TextureMutex);
		auto iter = GE::g_GE->m_TexturePool.find(path);
		is_find = iter != GE::g_GE->m_TexturePool.end();


		if (is_find)
		{
			TextureRef ref(iter->second, GE::g_GE, false);
			return move(ref);
		}
	}

	vector<GE::Texture*> textureArray;
	GE::g_GE->m_GraphicsDevice->LoadTextures({ path }, textureArray);
	if (textureArray[0] == nullptr)
	{
		return TextureRef();
	}
	else
	{
		{
			lock_guard<shared_mutex> lock(GE::g_GE->m_TextureMutex);
			GE::g_GE->m_TexturePool[path] = textureArray[0];
		}
	



		TextureRef ref(textureArray[0], GE::g_GE, false);
		return move(ref);
	}
}

TextureRef GraphicsIF::GetTexture(const std::wstring& path)
{
	bool is_find = false;
	shared_lock<shared_mutex> lock(GE::g_GE->m_TextureMutex);
	auto iter = GE::g_GE->m_TexturePool.find(path);
	is_find = iter != GE::g_GE->m_TexturePool.end();


	if (is_find)
	{
		TextureRef ref(iter->second, GE::g_GE, false);
		return move(ref);
	}
	else
	{
		return TextureRef();
	}
}

void GraphicsIF::DrawCall(SceneRef& sceneRef)
{
	DrawCall(sceneRef.Get());
}

void GraphicsIF::DrawCall(GE::Scene* scene)
{
	if (GE::g_GE->m_GraphicsRenderer == nullptr)
	{
		GELOG_FATAL("그래픽 렌더러가 nullptr 입니다.");
		return;
	}
	if (!GE::g_GE->m_GraphicsRenderer->DrawCall(scene))
	{
		GELOG_ERROR("Scene 드로우 콜이 실패했습니다.");
		return;
	}
}

void GraphicsIF::BeginDraw()
{
	GE::g_GE->m_GraphicsDevice->BeginFrame();
	GE::g_GE->m_GraphicsRenderer->BeginFrame();
}
void GraphicsIF::EndDraw()
{
	GE::g_GE->m_GraphicsRenderer->EndFrame();
	GE::g_GE->m_GraphicsDevice->EndFrame();
}

GraphicsAPI GraphicsIF::GetGraphicsAPI()
{
	return GraphicsAPI::DirectX12;
}


#ifdef INDEPENDENT_GRAPHICS_INTERFACE  
void GraphicsIF::Destroy()
{
	GE::g_GE->Destroy();
	delete GE::g_GE;
	GE::g_GE = nullptr;
}
#endif // !1






