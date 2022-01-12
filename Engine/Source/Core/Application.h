#pragma once
#include "JGCore.h"
#include "Platform/Window/Window.h"
#include "Class/SystemLayerStack.h"
#include "Graphics/GraphicsAPI.h"
#include "Physcis/PhysicsAPI.h"
int main(int argc, char** argv);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow);

namespace JG
{
	class Timer;
	struct ApplicaionProps
	{
		EWindowPlatform WindowPlatform;
		WindowProps		WindowProps;
	};
	class Application
	{
	public:
		static const String& GetAssetPath();
		static const String& GetEnginePath();
		static const String& GetIconPath();
		static const String& GetTexturePath();
		static const String& GetMeshPath();
		static const String& GetShaderGlobalLibPath();
		static const String& GetShaderScriptPath();
		static const String& GetComputeShaderScriptPath();
		static const String& GetShaderTemplatePath();
		static const String& GetConfigPath();
		static const String& GetCachePath();
	private:
		static Application* smInstance;
	private:
		ApplicaionProps         mProps;
		UniquePtr<IWindow>		mWindow;
		UniquePtr<SystemLayerStack>	mLayerStack;
		Queue<UniquePtr<IEvent>>    mEventQueue;

		SharedPtr<Timer> mAppTimer;

		JVector2Uint mDisplaySize;
		bool mIsRunning = true;
		bool mMinimized = false;
	public:
		virtual ~Application();
	private:
		bool Create(const ApplicaionProps& prop);
		void Run();
		void Destroy();
		void OnEvent(IEvent& e);
		bool Open(AppOpenEvent& e);
		bool Resize(AppResizeEvent& e);
		bool Close(AppCloseEvent& e);
	protected:
		virtual void LoadImpl() {}
		virtual void OpenImpl() {}
		virtual void CloseImpl(){}

	protected:
		void PushLayer(const SharedPtr<ISystemLayer>& layer);
		void PushOverlay(const SharedPtr<ISystemLayer>& layer);
	public:
		static Application& GetInstance()
		{
			return *smInstance;
		}
	public:
		IWindow*	  GetWindow() const;
		const Timer*  GetAppTimer() const;
		JVector2Uint  GetDisplaySize() const;
	public:
		template<class T>
		void SendEvent(T& e)
		{
			auto pE = CreateUniquePtr<T>();
			*pE = e;
			mEventQueue.push(std::move(pE));
		}
		template<class T>
		void SendEventImmediate(T& e)
		{
			OnEvent(e);
		}
	private:
		friend int ::main(int argc, char** argv);
		friend int APIENTRY ::WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow);

	};

}
