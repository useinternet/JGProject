#include "pch.h"
#include "Shader.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"
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


	SharedPtr<IShaderScript> IShaderScript::CreateSurfaceScript(const String& name, const String& code)
	{
		auto script = CreateSharedPtr<ShaderScript>(name, code, EShaderScriptType::Surface);
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

	bool ShaderLibrary::LoadGlobalShaderLib(const String& path)
	{
		auto globalLibPath = PathExtend::CombinePath(path, "GlobalShaderLibrary.shaderLib");
		FileExtend::ReadAllText(globalLibPath, &mGlobalShaderLibCode);

		auto globalGraphicsLibPath = PathExtend::CombinePath(path, "GlobalGraphicsLibrary.shaderLib");
		FileExtend::ReadAllText(globalGraphicsLibPath, &mGlobalGraphicsLibCode);

		auto globalComputeLibPath = PathExtend::CombinePath(path, "GlobalComputeLibrary.shaderLib");
		FileExtend::ReadAllText(globalComputeLibPath, &mGlobalComputeLibCode);
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

	void ShaderLibrary::ForEach(EShaderScriptType scriptType, const std::function<void(SharedPtr<IShaderScript>)>& action)
	{
		if (action == nullptr) return;
		for (auto& script : mShaderScriptDicByType[scriptType])
		{
			action(script);
		}
	}













}