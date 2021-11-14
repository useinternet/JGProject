#include "pch.h"
#include "Application.h"
#include "Common/Assert.h"
#include "Class/Asset/Asset.h"


#include "Platform/Window/WindowsWindow.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/DebugGeometryDrawer.h"
using namespace std;
namespace JG
{
	Application* Application::smInstance = nullptr;

	static String gAssetPath   = "../../../GameProject/Project_C/Asset";
	static String gEnginePath  = CombinePath(Application::GetAssetPath(), "Engine");
	static String gIconPath    = CombinePath(Application::GetEnginePath(), "Icon");
	static String gTexturePath = CombinePath(Application::GetEnginePath(), "Texture");
	static String gMeshPath    = CombinePath(Application::GetEnginePath(), "Mesh");
	static String gConfigPath = CombinePath(Application::GetEnginePath(), "Config");
	static String gShaderGlobalLibPath = CombinePath(Application::GetEnginePath(), "Shader/GlobalLib");
	static String gShaderScriptPath   = CombinePath(Application::GetEnginePath(), "Shader/Script");
	static String gShaderTemplatePath = CombinePath(Application::GetEnginePath(), "Shader/Template");


	const String& Application::GetAssetPath()
	{
		return gAssetPath;
	}

	const String& Application::GetEnginePath()
	{
		return gEnginePath;
	}

	const String& Application::GetIconPath()
	{
		return gIconPath;
	}

	const String& Application::GetTexturePath()
	{
		return gTexturePath;
	}

	const String& Application::GetMeshPath()
	{
		return gMeshPath;
	}

	const String& Application::GetShaderGlobalLibPath()
	{
		return gShaderGlobalLibPath;
	}

	const String& Application::GetShaderScriptPath()
	{
		return gShaderScriptPath;
	}

	const String& Application::GetShaderTemplatePath()
	{
		return gShaderTemplatePath;
	}

	const String& Application::GetConfigPath()
	{
		return gConfigPath;
	}



	Application::~Application() {}

	bool Application::Create(const ApplicaionProps& prop)
	{
		// NOTE
		// 인스턴스 생성
		if(smInstance != nullptr)
		{
			JGASSERT("Already exist application instance");
		}
		smInstance = this;
		mProps = prop;


		
		// TODO
		// 필요한 전역 클래스 생성
		Log::Create();
		TimerManager::Create();
		Scheduler::Create();
		AssetDataBase::Create();
		ProjectSetting::Create();

		// TODO
		// 필요한 멤버 클래스 생성
		mLayerStack  = CreateUniquePtr<SystemLayerStack>();
		// NOTE
		// Window 생성
		switch(prop.WindowPlatform)
		{
		case EWindowPlatform::Windows:
			mWindow = CreateUniquePtr<WindowsWindow>();
		
			break;
		default:
			JGASSERT("Do not supported platform");
			return false;
		}
		mWindow->BindCallBackEventFn(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		
		
		if(!mWindow->Create(prop.WindowProps))
		{
			JG_CORE_CRITICAL("Failed Create Window");
			return false;
		}
		else
		{
			JG_CORE_INFO("Successed Create Window");
			JGGraphicsDesc desc;
			desc.GraphicsAPI = EGraphicsAPI::DirectX12;
			desc.GlobalShaderLibPath = Application::GetShaderGlobalLibPath();
			desc.ShaderScriptPath    = Application::GetShaderScriptPath();
			desc.ShaderTemplatePath  = Application::GetShaderTemplatePath();
			JGGraphics::Create(desc);
			JGGraphics::GetInstance().LoadShader();
			mIsRunning = true;
		}
		PhysicsManager::Create();



		mAppTimer = Timer::Create();
		mAppTimer->Start();
		ITexture::CreateNullTexture();
		DebugGeometryDrawer::Create();

		return true;
	}
	void Application::Run()
	{
		LoadImpl();
		mLayerStack->ForEach([&](ISystemLayer* layer)
		{
			layer->Begin();
		});
		Scheduler::GetInstance().FlushAsyncTask(true);

		while(mIsRunning)
		{
			if (mWindow->Update() == false)
			{
				break;
			}
			if (TimerManager::IsValid())
			{
				TimerManager::GetInstance().Update();
			}

			//mGraphcisAPI->Begin();


			while (mEventQueue.empty() == false)
			{
				auto e = std::move(mEventQueue.front()); mEventQueue.pop();
				OnEvent(*e);
			}

			if (Scheduler::IsValid())
			{
				Scheduler::GetInstance().Update();
			}

			//mGraphcisAPI->End();
		}

		
		mLayerStack->ForEach([&](ISystemLayer* layer)
		{
			layer->Destroy();
		});
	}

	void Application::Destroy()
	{
		while (!mEventQueue.empty()) { mEventQueue.pop(); }
		Scheduler::GetInstance().FlushAsyncTask(false);

		DebugGeometryDrawer::Destroy();
		ITexture::DestroyNullTexture();
		PhysicsManager::Destroy();
		JGGraphics::GetInstance().Flush();
		mLayerStack.reset();
		AssetDataBase::Destroy();
		JGGraphics::Destroy();
		ProjectSetting::Destroy();
		mWindow->Destroy();
		mWindow.reset();
		Scheduler::Destroy();
		TimerManager::Destroy();
		Log::Destroy();
	}

	void Application::OnEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<AppOpenEvent>(EVENT_BIND_FN(&Application::Open));
		dispatcher.Dispatch<AppCloseEvent>(EVENT_BIND_FN(&Application::Close));
		dispatcher.Dispatch<AppResizeEvent>(EVENT_BIND_FN(&Application::Resize));

		mLayerStack->ForEach([&](ISystemLayer* layer)
		{
			if(e.Handled == false)
			{
				layer->OnEvent(e);
			}
		});	
	}
	bool Application::Open(AppOpenEvent& e)
	{
		JG_CORE_INFO(e.ToString());
		OpenImpl();
		return true;
	}
	bool Application::Resize(AppResizeEvent& e)
	{
		if(e.Width == 0 || e.Height == 0)
		{
			mMinimized = true;
			JG_CORE_INFO("최소화 상태");
		}
		else
		{
			mDisplaySize = e.Width;
			mDisplaySize = e.Height;
			mMinimized = false;
		}
		

		return mMinimized;
	}
	bool Application::Close(AppCloseEvent& e)
	{
		JG_CORE_INFO(e.ToString());
		CloseImpl();
		mIsRunning = false;
		return true;
	}

	void Application::PushLayer(const SharedPtr<ISystemLayer>& layer)
	{
		mLayerStack->PushLayer(layer);
	}
	void Application::PushOverlay(const SharedPtr<ISystemLayer>& layer)
	{
		mLayerStack->PushOverlay(layer);
	}

	IWindow* Application::GetWindow() const
	{
		return mWindow.get();
	}
	const Timer* Application::GetAppTimer() const
	{
		return mAppTimer.get();
	}
	JVector2Uint Application::GetDisplaySize() const
	{
		return mDisplaySize;
	}
}
