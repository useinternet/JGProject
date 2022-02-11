#pragma once

#include "JGCore.h"
#include "GraphicsDefine.h"
#include <shared_mutex>


namespace JG
{
	class IShaderScript;
	class IRayTracingPipeline;
	class IGraphicsShader
	{
	protected:
		virtual bool Compile(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags, String* error) = 0;
	public:
		virtual ~IGraphicsShader() = default;
	public:
		virtual const String& GetName()			  const = 0;
		virtual const String& GetShaderCode()     const = 0;
		virtual const String& GetFullShaderCode() const = 0;
		virtual EShaderFlags  GetFlags()          const = 0;
		virtual const List<std::pair<EShaderDataType, String>>& GetPropertyList() const = 0;
		virtual const List<SharedPtr<IShaderScript>>& GetScriptList() const = 0;
		virtual bool IsSuccessed() const = 0;
	public:
		static SharedPtr<IGraphicsShader> Create(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList = List<SharedPtr<IShaderScript>>());
	};

	class IComputeShader
	{
	protected:
		virtual bool Compile(const String& sourceCode, String* error) = 0;
	public:
		virtual ~IComputeShader() = default;
	public:
		virtual const String& GetName()		  const = 0;
		virtual const String& GetShaderCode() const = 0;
		virtual bool IsSuccessed() const = 0;
	public:
		static SharedPtr<IComputeShader> Create(const String& name, const String& sourceCode);
	};

	class IClosestHitShader
	{
	protected:
		virtual bool Init(const String& sourceCode, SharedPtr<IShaderScript> script) = 0;
	public:
		virtual ~IClosestHitShader() = default;
	public:
		virtual const String& GetName()		    const = 0;
		virtual const String& GetEntryPoint()   const = 0;
		virtual const String& GetHitGroupName() const = 0;
		virtual const String& GetShaderCode()   const = 0;
		virtual const String& GetFullShaderCode() const = 0;
	public:
		static SharedPtr<IClosestHitShader> Create(const String& name, const String& sourceCode, SharedPtr<IShaderScript> script);
	};



	enum class EShaderScriptType
	{
		Surface,
		Scene
	};
	class IShaderScript
	{
	public:
		virtual const String& GetCode() const = 0;
		virtual const String& GetName() const = 0;
		virtual const EShaderScriptType GetScriptType() const = 0;
	public:
		static SharedPtr<IShaderScript> CreateSurfaceScript(const String& name, const String& code);
		static SharedPtr<IShaderScript> CreateSceneScript(const String& name, const String& code);
	};

	class ShaderScript : public IShaderScript
	{
	private:
		String mName;
		String mCode;
		EShaderScriptType mScriptType;
	public:
		virtual ~ShaderScript() = default;
	public:
		ShaderScript(const String& name, const String& code, EShaderScriptType type)
			: mName(name), mCode(code), mScriptType(type) {}
	public:
		virtual const String& GetCode() const override 
		{
			return mCode;
		}
		virtual const String& GetName() const override
		{
			return mName;
		}
		virtual const EShaderScriptType GetScriptType() const override
		{
			return mScriptType;
		}
	};


	class ShaderLibrary : public GlobalSingleton<ShaderLibrary>
	{
		friend class JGGraphics;
	private:
		Dictionary<String, SharedPtr<IGraphicsShader>> mGraphicsShaderDic;
		Dictionary<String, SharedPtr<IComputeShader>>  mComputeShaderDic;
		Dictionary<String, SharedPtr<IShaderScript>>   mShaderScriptDic;
		Dictionary<String, SharedPtr<IClosestHitShader>> mClosestHitShaderDic;
		Dictionary<String, SharedPtr<IRayTracingPipeline>> mRayTracingPipelineDic;
		Dictionary<EShaderScriptType, List<SharedPtr<IShaderScript>>> mShaderScriptDicByType;
		String mGlobalShaderLibCode;
		String mGlobalGraphicsLibCode;
		String mGlobalComputeLibCode;
		String mGlobalRayTracingLibCode;

		std::shared_mutex mGraphicsMutex;
		std::shared_mutex mComputeMutex;
		std::shared_mutex mScriptMutex;
		std::shared_mutex mClosestHitMutex;
		std::shared_mutex mRayTracingPipelineMutex;
	public:
		void RegisterGraphicsShader(const String& name, SharedPtr<IGraphicsShader> shader);
		void RegisterComputeShader(const String& name, SharedPtr<IComputeShader> shader);
		void RegisterShaderScript(const String& name, SharedPtr<IShaderScript> script);
		void RegisterClosestHitShader(const String& name, SharedPtr<IClosestHitShader> shader);
		void RegisterRayTracingPipeline(const String& name, SharedPtr<IRayTracingPipeline> pipeline);




		SharedPtr<IGraphicsShader> FindGraphicsShader(const String& name);
		SharedPtr<IGraphicsShader> FindGraphicsShader(const String& name, const List<String>& scriptNameList);
		SharedPtr<IComputeShader>  FindComputeShader(const String& name);
		SharedPtr<IClosestHitShader> FindClosestHitShader(const String& name);
		SharedPtr<IClosestHitShader> FindClosestHitShader(const String& name, const String& scriptName);
		SharedPtr<IShaderScript>   FindScript(const String& name);
		SharedPtr< IRayTracingPipeline> FindRayTracingPipeline(const String& name);

		void AddRayTracingLibrary(const String& name, SharedPtr<IClosestHitShader> shader);



		bool   LoadGlobalShaderLib(const String& path);
		const String& GetGlobalShaderLibCode()   const;
		const String& GetGlobalGraphicsLibCode() const;
		const String& GetGlobalComputeLibCode() const;
		const String& GetGlobalRayTracingLibCode() const;
	public:
		void ForEach(EShaderScriptType scriptType, const std::function<void(SharedPtr<IShaderScript>)>& action);
	};



	
	
}


