﻿#pragma once

#include "JGCore.h"
#include "GraphicsDefine.h"
#include <shared_mutex>
namespace JG
{
	class IComputeBuffer;
	class IShaderScript;
	class ITexture;
	class IShader : public GraphicsCommandable
	{
	protected:
		virtual bool Compile(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags, String* error) = 0;
		virtual bool Bind() = 0;
	public:
		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() const    = 0;
		virtual const String& GetOriginCode() const	 = 0;
		virtual EShaderFlags  GetFlags() const   = 0;
	public:
		static SharedPtr<IShader> Create(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList = List<SharedPtr<IShaderScript>>());
	};

	class IShaderScript
	{
	public:
		enum : u64
		{
			Surface,
		};
	public:
		virtual const String& GetCode() const = 0;
		virtual const String& GetName() const = 0;

	public:
		static SharedPtr<IShaderScript> CreateMaterialScript(const String& name, const String& code);
	};

	class MaterialScript : public IShaderScript
	{
	private:
		String mName;
		String mCode;
	public:
		MaterialScript(const String& name, const String& code) : mName(name), mCode(code) {}
	public:
		virtual const String& GetCode() const override 
		{
			return mCode;
		}
		virtual const String& GetName() const override
		{
			return mName;
		}
	};


	class ShaderLibrary : public GlobalSingleton<ShaderLibrary>
	{
		friend class Application;
	private:
		Dictionary<String, SharedPtr<IShader>> mShaders;
		Dictionary<String, SharedPtr<IShaderScript>> mMaterialScirpts;
		std::shared_mutex mMutex;
	public:
		void RegisterShader(SharedPtr<IShader> shader);
		void RegisterScirpt(SharedPtr<IShaderScript> script);




		SharedPtr<IShader> GetShader(const String& name);
		SharedPtr<IShader> GetShader(const String& name, const List<String>& scriptNameList);
		SharedPtr<IShaderScript> GetScript(const String& name);
	};



	
	
}


