#include "pch.h"
#include "Shader.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/RayTracing/RayTracingPipeline.h"
#include "Graphics/RayTracing/Raytracer.h"
#include "GraphicsAPI.h"

namespace JG
{
	SharedPtr<IGraphicsShader> IGraphicsShader::Create(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateGraphicsShader(name, sourceCode, flags, scriptList);
	}

	SharedPtr<IComputeShader> IComputeShader::Create(const String& name, const String& sourceCode)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateComputeShader(name, sourceCode);
	}
	SharedPtr<IClosestHitShader> IClosestHitShader::Create(const String& name, const String& sourceCode, SharedPtr<IShaderScript> script)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateClosestHitShader(name, sourceCode, script);
	}


	SharedPtr<IShaderScript> IShaderScript::CreateSurfaceScript(const String& name, const String& code)
	{
		auto script = CreateSharedPtr<ShaderScript>(name, code, EShaderScriptType::Surface);
		return script;
	}
	SharedPtr<IShaderScript> IShaderScript::CreateSceneScript(const String& name, const String& code)
	{
		auto script = CreateSharedPtr<ShaderScript>(name, code, EShaderScriptType::Scene);
		return script;
	}
	void ShaderLibrary::RegisterGraphicsShader(const String& name, SharedPtr<IGraphicsShader> shader)
	{
		if (shader == nullptr || shader->IsSuccessed() == false)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mGraphicsMutex);

		auto iter = mGraphicsShaderDic.find(name);
		if (iter == mGraphicsShaderDic.end())
		{
			mGraphicsShaderDic.emplace(name, shader);
		}
	}

	void ShaderLibrary::RegisterComputeShader(const String& name, SharedPtr<IComputeShader> shader)
	{
		if (shader == nullptr || shader->IsSuccessed() == false)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mComputeMutex);

		auto iter = mComputeShaderDic.find(name);
		if (iter == mComputeShaderDic.end())
		{
			mComputeShaderDic.emplace(name, shader);
		}
	}

	void ShaderLibrary::RegisterShaderScript(const String& name, SharedPtr<IShaderScript> script)
	{

		if (script == nullptr)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mScriptMutex);

		auto iter = mShaderScriptDic.find(name);
		if (iter == mShaderScriptDic.end())
		{
			mShaderScriptDic.emplace(name, script);
			mShaderScriptDicByType[script->GetScriptType()].push_back(script);
		}
	}

	void ShaderLibrary::RegisterClosestHitShader(const String& name, SharedPtr<IClosestHitShader> shader)
	{
		if (shader == nullptr)
		{
			return;
		}

		std::lock_guard<std::shared_mutex> lock(mClosestHitMutex);

		auto iter = mClosestHitShaderDic.find(name);
		if (iter == mClosestHitShaderDic.end())
		{
			mClosestHitShaderDic.emplace(name, shader);
		}
	}

	void ShaderLibrary::RegisterRayTracingPipeline(const String& name, SharedPtr<IRayTracingPipeline> pipeline)
	{
		if (pipeline == nullptr)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mRayTracingPipelineMutex);

		auto iter = mRayTracingPipelineDic.find(name);
		if (iter == mRayTracingPipelineDic.end())
		{
			mRayTracingPipelineDic.emplace(name, pipeline);
		}
		
	}

	SharedPtr<IGraphicsShader> ShaderLibrary::FindGraphicsShader(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(mGraphicsMutex);
		auto iter = mGraphicsShaderDic.find(name);
		if (iter != mGraphicsShaderDic.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	SharedPtr<IGraphicsShader> ShaderLibrary::FindGraphicsShader(const String& name, const List<String>& scriptNameList)
	{
		String code = "";
		auto originShader = FindGraphicsShader(name);
		if (originShader == nullptr)
		{
			return nullptr;
		}
		code += name;

		List<SharedPtr<IShaderScript>> scriptList;
		for (auto& scriptName : scriptNameList)
		{
			auto script = FindScript(scriptName);
			if (script == nullptr)
			{
				continue;
			}
			code += "/" + script->GetName();
			scriptList.push_back(script);
		}

		auto result = FindGraphicsShader(code);
		if (result == nullptr)
		{
			result = IGraphicsShader::Create(code, originShader->GetShaderCode(), originShader->GetFlags(), scriptList);
			if (result == nullptr)
			{
				return nullptr;
			}
			RegisterGraphicsShader(code, result);
		}
		return result;
	}

	SharedPtr<IComputeShader> ShaderLibrary::FindComputeShader(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(mComputeMutex);
		auto iter = mComputeShaderDic.find(name);
		if (iter != mComputeShaderDic.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	SharedPtr<IClosestHitShader> ShaderLibrary::FindClosestHitShader(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(mClosestHitMutex);
		auto iter = mClosestHitShaderDic.find(name);
		if (iter != mClosestHitShaderDic.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	SharedPtr<IClosestHitShader> ShaderLibrary::FindClosestHitShader(const String& name, const String& scriptName)
	{
		String code = "";
		auto originShader = FindClosestHitShader(name);
		if (originShader == nullptr)
		{
			return nullptr;
		}
		code += name;

		auto script = FindScript(scriptName);
		code += "/" + script->GetName();

		auto result = FindClosestHitShader(code);
		if (result == nullptr)
		{
			result = IClosestHitShader::Create(code, originShader->GetShaderCode(), script);
			if (result == nullptr)
			{
				return nullptr;
			}
			RegisterClosestHitShader(code, result);
			AddRayTracingLibrary(RayTracer::GetDefaultRayTracingPipelineName(), result);
		}
		return result;
	}



	SharedPtr<IShaderScript> ShaderLibrary::FindScript(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(mScriptMutex);
		auto iter = mShaderScriptDic.find(name);
		if (iter != mShaderScriptDic.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	SharedPtr<IRayTracingPipeline> ShaderLibrary::FindRayTracingPipeline(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(mRayTracingPipelineMutex);
		auto iter = mRayTracingPipelineDic.find(name);
		if (iter != mRayTracingPipelineDic.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	void ShaderLibrary::AddRayTracingLibrary(const String& name, SharedPtr<IClosestHitShader> shader)
	{
		SharedPtr<IRayTracingPipeline> pipeline = FindRayTracingPipeline(name);
		if (pipeline == nullptr)
		{
			JG_LOG_ERROR("Fail Add RayTracing Library : {0} -> {1}", name, shader->GetName());
			return;
		}
		pipeline->AddLibrary(shader);
	}

	bool ShaderLibrary::LoadGlobalShaderLib(const String& path)
	{
		auto globalLibPath = PathHelper::CombinePath(path, "GlobalShaderLibrary.shaderLib");
		FileHelper::ReadAllText(globalLibPath, &mGlobalShaderLibCode);

		auto globalGraphicsLibPath = PathHelper::CombinePath(path, "GlobalGraphicsLibrary.shaderLib");
		FileHelper::ReadAllText(globalGraphicsLibPath, &mGlobalGraphicsLibCode);

		auto globalComputeLibPath = PathHelper::CombinePath(path, "GlobalComputeLibrary.shaderLib");
		FileHelper::ReadAllText(globalComputeLibPath, &mGlobalComputeLibCode);


		auto globalRayTracingLibPath = PathHelper::CombinePath(path, "GlobalRayTracingLibrary.shaderLib");
		FileHelper::ReadAllText(globalRayTracingLibPath, &mGlobalRayTracingLibCode);
		return true;
	}

	const String& ShaderLibrary::GetGlobalShaderLibCode() const
	{
		return mGlobalShaderLibCode;
	}
	const String& ShaderLibrary::GetGlobalGraphicsLibCode() const
	{
		return mGlobalGraphicsLibCode;
	}

	const String& ShaderLibrary::GetGlobalComputeLibCode() const
	{
		return mGlobalComputeLibCode;
	}

	const String& ShaderLibrary::GetGlobalRayTracingLibCode() const
	{
		return mGlobalRayTracingLibCode;
	}

	void ShaderLibrary::ForEach(EShaderScriptType scriptType, const std::function<void(SharedPtr<IShaderScript>)>& action)
	{
		if (action == nullptr) return;
		for (auto& script : mShaderScriptDicByType[scriptType])
		{
			action(script);
		}
	}













}