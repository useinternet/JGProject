#include "pch.h"
#include "Shader.h"
#include "Application.h"
#include "Graphics/JGGraphics.h"
#include "GraphicsAPI.h"

namespace JG
{
	SharedPtr<IShader> IShader::Create(const String& name, const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateShader(name, sourceCode, flags, scriptList);
	}

	SharedPtr<IGraphicsShader> IGraphicsShader::Create(const String& sourceCode, EShaderFlags flags, const List<SharedPtr<IShaderScript>>& scriptList)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateGraphicsShader(sourceCode, flags, scriptList);
	}

	SharedPtr<IComputeShader> IComputeShader::Create(const String& sourceCode)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateComputeShader(sourceCode);
	}


	SharedPtr<IShaderScript> IShaderScript::CreateShaderScript(const String& name, const String& code)
	{
		auto script = CreateSharedPtr<ShaderScript>(name, code);
		return script;
	}
	void ShaderLibrary::RegisterShader(SharedPtr<IShader> shader)
	{
		if (shader == nullptr)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mMutex);

		auto iter = mShaders.find(shader->GetName());
		if (iter == mShaders.end())
		{
			mShaders.emplace(shader->GetName(), shader);
		}
	}
	void ShaderLibrary::RegisterScirpt(SharedPtr<IShaderScript> script)
	{
		if (script == nullptr)
		{
			return;
		}
		std::lock_guard<std::shared_mutex> lock(mMutex);

		auto iter = mMaterialScirpts.find(script->GetName());
		if (iter == mMaterialScirpts.end())
		{
			mMaterialScirpts.emplace(script->GetName(), script);
		}
	}
	SharedPtr<IShader> ShaderLibrary::GetShader(const String& name)
	{
		std::shared_lock<std::shared_mutex> lock(mMutex);
		auto iter = mShaders.find(name);
		if (iter != mShaders.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	SharedPtr<IShader> ShaderLibrary::GetShader(const String& name,const List<String>& scriptNameList)
	{
		String code = "";
		auto originShader = GetShader(name);
		if (originShader == nullptr)
		{
			return nullptr;
		}
		code += originShader->GetName();

		List<SharedPtr<IShaderScript>> scriptList;
		for (auto& scriptName : scriptNameList)
		{
			auto script = GetScript(scriptName);
			if (script == nullptr)
			{
				continue;
			}
			code += script->GetName();
			scriptList.push_back(script);
		}

		auto result = GetShader(code);
		if (result == nullptr)
		{
			result = IShader::Create(code, originShader->GetOriginCode(), originShader->GetFlags(), scriptList);
			if (result == nullptr)
			{
				return nullptr;
			}
			RegisterShader(result);
		}
		return result;
	}

	SharedPtr<IShaderScript> ShaderLibrary::GetScript(const String& name)
	{

		std::shared_lock<std::shared_mutex> lock(mMutex);
		auto iter = mMaterialScirpts.find(name);
		if (iter != mMaterialScirpts.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	bool ShaderLibrary::LoadGlobalShaderLib(const String& path)
	{
		auto globalLibPath = PathExtend::CombinePath(path, "GlobalShaderLibrary.shaderLib");
		FileExtend::GetReadAllText(globalLibPath, &mGlobalShaderLibCode);

		auto globalGraphicsLibPath = PathExtend::CombinePath(path, "GlobalGraphicsLibrary.shaderLib");
		FileExtend::GetReadAllText(globalGraphicsLibPath, &mGlobalGraphicsLibCode);

		return true;
	}
	String ShaderLibrary::GetGlobalGraphicsLibCode() const
	{
		return mGlobalGraphicsLibCode;
	}

	String ShaderLibrary::GetGlobalShaderLibCode() const
	{
		return mGlobalShaderLibCode;
	}










}