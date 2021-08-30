#include "pch.h"
#include "Application.h"
#include "Common/Assert.h"
#include "Class/Asset/Asset.h"


#include "Platform/Window/WindowsWindow.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
using namespace std;
namespace JG
{
	Application* Application::smInstance = nullptr;

	static String gAssetPath = "../../../JGGameProject/Project_C/Asset";


	const String& Application::GetAssetPath()
	{
		return gAssetPath;
	}

	const String Application::GetEnginePath()
	{
		return CombinePath(GetAssetPath(), "Engine");
	}

	const String Application::GetIconPath()
	{
		return CombinePath(GetAssetPath(), "Engine/Icon");
	}

	const String Application::GetShaderGlobalLibPath()
	{
		return CombinePath(GetAssetPath(), "Engine/Shader/GlobalLib");
	}

	const String Application::GetShaderScriptPath()
	{
		return CombinePath(GetAssetPath(), "Engine/Shader/Script");
	}

	const String Application::GetShaderTemplatePath()
	{
		return CombinePath(GetAssetPath(), "Engine/Shader/Template");
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
		UIManager::Create();
		GameLayerManager::Create();
		AssetDataBase::Create();


		// TODO
		// 필요한 멤버 클래스 생성
		mLayerStack  = CreateUniquePtr<SystemLayerStack>();
		mGraphcisAPI = IGraphicsAPI::Create(EGraphicsAPI::DirectX12);
		

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
			mGraphcisAPI->Create();
			ShaderLibrary::Create();
			mIsRunning = true;
		}
		mAppTimer = Timer::Create();
		mAppTimer->Start();
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

			mGraphcisAPI->Begin();


			while (mEventQueue.empty() == false)
			{
				auto e = std::move(mEventQueue.front()); mEventQueue.pop();
				OnEvent(*e);
			}

			if (Scheduler::IsValid())
			{
				Scheduler::GetInstance().Update();
			}

			mGraphcisAPI->End();

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
		mGraphcisAPI->Flush();
		UIManager::Destroy();
		mLayerStack.reset();
		GameLayerManager::Destroy();
		AssetDataBase::Destroy();
		ShaderLibrary::Destroy();
		mGraphcisAPI->Destroy();
		mGraphcisAPI.reset();
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

		if (e.Handled == false)
		{
			UIManager::GetInstance().OnEvent(e);
		}
	
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
	IGraphicsAPI* Application::GetGraphicsAPI() const
	{
		return mGraphcisAPI.get();
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
