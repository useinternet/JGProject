#include "pch.h"
#include "DirectX12Shader.h"
#include "Utill/PipelineState.h"
#include "Utill/RootSignature.h"
#include "Utill/CommandList.h"
#include "Utill/ShaderDataForm.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"

namespace JG
{
	bool DirectX12GraphicsShader::Compile(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags, String* error)
	{
		mFlags = flags;
		// SourceCode
		String libCode = ShaderLibrary::GetInstance().GetGlobalShaderLibCode()   + "\n";
		libCode       += ShaderLibrary::GetInstance().GetGlobalGraphicsLibCode() + "\n";
		mSourceCode = sourceCode;

		String code = libCode + sourceCode;
		InsertScript(code, scriptList);
		mFullSourceCode = code;
		if (Compile(code, error) == true)
		{
			return true;
		}
		return false;
	}
	const String& DirectX12GraphicsShader::GetShaderCode() const
	{
		return mSourceCode;
	}
	const String& DirectX12GraphicsShader::GetFullShaderCode() const
	{
		return mFullSourceCode;
	}
	EShaderFlags DirectX12GraphicsShader::GetFlags() const
	{
		return mFlags;
	}
	bool DirectX12GraphicsShader::IsSuccessed() const
	{
		return mIsCompileSuccess;
	}
	const List<std::pair<EShaderDataType, String>>& DirectX12GraphicsShader::GetPropertyList() const
	{
		return mPropertyList;
	}
	bool DirectX12GraphicsShader::Compile(const String& code, String* error)
	{
		if (mFlags & EShaderFlags::Allow_VertexShader)
		{
			if (Compile(mVSData, code, CompileConfig(HLSL::VSEntry, HLSL::VSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_DomainShader)
		{
			if (Compile(mDSData, code, CompileConfig(HLSL::DSEntry, HLSL::DSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_HullShader)
		{
			if (Compile(mHSData, code, CompileConfig(HLSL::HSEntry, HLSL::HSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_GeometryShader)
		{
			if (Compile(mGSData, code, CompileConfig(HLSL::GSEntry, HLSL::GSTarget), error) == false)
			{
				return false;
			}
		}
		if (mFlags & EShaderFlags::Allow_PixelShader)
		{
			if (Compile(mPSData, code, CompileConfig(HLSL::PSEntry, HLSL::PSTarget), error) == false)
			{
				return false;
			}
		}

		return true;
	}
	bool DirectX12GraphicsShader::Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error)
	{
		ComPtr<ID3DBlob> errorData;
		HRESULT hr = D3DCompile2(
			sourceCode.data(),
			sourceCode.size(),
			nullptr,
			nullptr,
			nullptr,
			config.Entry.c_str(),
			config.Target.c_str(),
			0, 0, 0, nullptr, 0,
			blob.GetAddressOf(),
			errorData.GetAddressOf());

		if (FAILED(hr) && error != nullptr)
		{
			*error = (char*)errorData->GetBufferPointer();
			return false;
		}
		mIsCompileSuccess = true;
		return true;
	}
	void DirectX12GraphicsShader::InsertScript(String& code, const List<SharedPtr<IShaderScript>>& scriptList)
	{
		if (scriptList.empty() == true)
		{
			code = StringExtend::ReplaceAll(code, ShaderDefine::Location::SurfaceResources, "");
			code = StringExtend::ReplaceAll(code, ShaderDefine::Location::SurfaceVariables, "");
			code = StringExtend::ReplaceAll(code, ShaderDefine::Location::SurfaceContents, "");
		}
		else
		{
			for (auto& script : scriptList)
			{
				if (InsertScriptInternal(code, script) == false)
				{
					JG_CORE_ERROR("Fail Insert ShaderScript : {0}", script->GetName());
				}
			}
		}
	}
	bool DirectX12GraphicsShader::InsertScriptInternal(String& code, SharedPtr<IShaderScript> script)
	{
		String scriptCode = script->GetCode();
		String resourcesCode;
		if (ExtractScriptContents(scriptCode, ShaderDefine::Type::Resources, resourcesCode) == true)
		{
			//
			u64 lineStart    = 0;
			u64 lineEnd      = 0;
			u32 texture2dCnt = 0;
			while (true)
			{
				lineEnd = resourcesCode.find(";", lineStart);
				if (lineEnd == String::npos) break;
				auto line = resourcesCode.substr(lineStart, lineEnd - lineStart);
				auto mid  = line.find_last_of(" ");
				auto type = line.substr(0, mid); type = StringExtend::ReplaceAll(type, "\n", "");  type = StringExtend::ReplaceAll(type, "\t", ""); type = StringExtend::ReplaceAll(type, " ", "");
				auto name = line.substr(mid + 1, line.length() - mid - 1);
				name = StringExtend::ReplaceAll(name, "\n", "");  name = StringExtend::ReplaceAll(name, "\t", "");
				mPropertyList.push_back(std::pair<EShaderDataType, String>(StringToShaderDataType(type), name));
				if (type + " "== HLSL::Token::Texture2D)
				{
					String registerSpace = std::to_string(HLSL::RegisterSpace::Texture2DRegisterSpace);
					String registerNum   = std::to_string(texture2dCnt++);
					String registerStr = " : register(t" + registerNum + ", space" + registerSpace + ")";
					resourcesCode.insert(lineEnd, registerStr);
					lineEnd += registerStr.length();
				}
				lineStart = lineEnd + 1;
			}
		}

		String variablesCode;
		if (ExtractScriptContents(scriptCode, ShaderDefine::Type::Variables, variablesCode) == true)
		{
			u64 lineStart = 0;
			u64 lineEnd = 0;
			while (true)
			{
				lineEnd = variablesCode.find(";", lineStart);
				if (lineEnd == String::npos) break;
				auto line = variablesCode.substr(lineStart, lineEnd - lineStart);

				auto mid = line.find_last_of(" ");
				auto type = line.substr(0, mid); type = StringExtend::ReplaceAll(type, "\n", "");  type = StringExtend::ReplaceAll(type, "\t", ""); type = StringExtend::ReplaceAll(type, " ", "");
				auto name = line.substr(mid + 1, line.length() - mid - 1);
				name = StringExtend::ReplaceAll(name, "\n", "");  name = StringExtend::ReplaceAll(name, "\t", "");
				mPropertyList.push_back(std::pair<EShaderDataType, String>(StringToShaderDataType(type), name));
				lineStart = lineEnd + 1;
			}
			String registerStr = " : register(b" + std::to_string(HLSL::RegisterNumber::MaterialRegisterNumber) + ")";
			variablesCode = String(HLSL::Token::CBuffer) + ShaderDefine::Variables::Surface + registerStr + "\n { \n" + variablesCode;
			variablesCode += "\n};";
		}

		String surfaceCode;
		i32 type = -1;
		if (ExtractScriptContents(scriptCode, ShaderDefine::Type::Surface, surfaceCode) == true)
		{
			type = IShaderScript::Surface;
		}


		switch (type)
		{
		case IShaderScript::Surface:
			code = StringExtend::ReplaceAll(code, ShaderDefine::Location::SurfaceResources, resourcesCode);
			code = StringExtend::ReplaceAll(code, ShaderDefine::Location::SurfaceVariables, variablesCode);
			code = StringExtend::ReplaceAll(code, ShaderDefine::Location::SurfaceContents, surfaceCode);
			break;
		case IShaderScript::Compute:
			break;
		default:
			return false;
		}
		return true;
	}
	bool DirectX12GraphicsShader::ExtractScriptContents(const String& code, const String& key, String& out_code)
	{
		u64 start = code.find(key);
		if (start == String::npos)
		{
			return false;
		}


		u64 end = code.find("};", start);
		if (start == String::npos)
		{
			return false;
		}

		start = code.find("{", start);
		if (start == String::npos)
		{
			return false;
		}

		out_code = code.substr(start + 1, end - start - 1);
		return true;
	}
	bool DirectX12ComputeShader::Compile(const String& sourceCode, String* error)
	{
		String libCode = ShaderLibrary::GetInstance().GetGlobalShaderLibCode() + "\n";
		mSourceCode     = libCode + sourceCode;

		mShaderDataForm = CreateSharedPtr<ShaderDataForm>();
		if (mShaderDataForm->Set(mSourceCode) == false)
		{
			return false;
		}

		if (Compile(mCSData, mSourceCode, CompileConfig(HLSL::CSEntry, HLSL::CSTarget), error) == true)
		{
			return true;
		}
		return false;
	}
	const String& DirectX12ComputeShader::GetShaderCode() const
	{
		return mSourceCode;
	}
	bool DirectX12ComputeShader::IsSuccessed() const
	{
		return mIsCompileSuccess;
	}
	bool DirectX12ComputeShader::Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error)
	{
		ComPtr<ID3DBlob> errorData;
		HRESULT hr = D3DCompile2(
			sourceCode.data(),
			sourceCode.size(),
			nullptr,
			nullptr,
			nullptr,
			config.Entry.c_str(),
			config.Target.c_str(),
			0, 0, 0, nullptr, 0,
			blob.GetAddressOf(),
			errorData.GetAddressOf());

		if (FAILED(hr) && error != nullptr)
		{
			*error = (char*)errorData->GetBufferPointer();
			return false;
		}
		mIsCompileSuccess = true;
		return true;
	}
}

