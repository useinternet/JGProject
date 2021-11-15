#pragma once
#include "JGCore.h"
#include <Windows.h>

namespace JG
{
	template<class ReturnType, class ...Args>
	class PluginFunction
	{
		friend class Plugin;
	private:
		ReturnType (*func)(Args...);
	public:
		ReturnType operator()(Args... args)
		{
			return func(args...);
		}
		bool IsVaild() const {
			return func != nullptr;
		}
	};
	class Plugin
	{
		HINSTANCE mDll = nullptr;
	public:
		Plugin(const String& dllPath);
		~Plugin();

		bool IsVaild() const;
	public:
		template<class ReturnType, class ...Args>
		PluginFunction<ReturnType, Args...> LoadFunction(const String& funcName)
		{
			PluginFunction<ReturnType, Args...> result;
			if (IsVaild() == true)
			{
				result.func = (ReturnType(*)(Args...))::GetProcAddress(mDll, funcName.c_str());
			}
	
	
			return result;
		}
	};
}