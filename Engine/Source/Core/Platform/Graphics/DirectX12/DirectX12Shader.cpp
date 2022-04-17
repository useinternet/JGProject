#include "pch.h"
#include "DirectX12Shader.h"
#include "Utill/PipelineState.h"
#include "Utill/RootSignature.h"
#include "Utill/CommandList.h"
#include "Utill/ShaderDataForm.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"

namespace JG
{
	const String& DirectX12GraphicsShader::GetName() const
	{
		return mName;
	}
	const String& DirectX12GraphicsShader::GetShaderCode() const
	{
		return mSourceCode;
	}
	const String& DirectX12GraphicsShader::GetFullShaderCode() const
	{
		return mFullSourceCode;
	}

	const JG::String& DirectX12GraphicsShader::GetErrorMessage() const
	{
		return mErrorMessage;
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
		return mScriptCodeAnalyzer->GetPropertyList();
	}
	const List<SharedPtr<IShaderScript>>& DirectX12GraphicsShader::GetScriptList() const
	{
		return mShaderScriptList;
	}


	void DirectX12GraphicsShader::SetName(const String& name)
	{
		mName = name;
	}
	bool DirectX12GraphicsShader::Init(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags)
	{
		mFlags = flags;
		mShaderScriptList = scriptList;
		mScriptCodeAnalyzer = CreateUniquePtr<ShaderScriptCodeAnalyzer>(
			HLSL::RegisterNumber::MaterialRegisterNumber, 0,
			0, HLSL::RegisterSpace::Texture2DRegisterSpace);
		// SourceCode
		String libCode = ShaderLibrary::GetInstance().GetGlobalShaderLibCode() + "\n";
		libCode += ShaderLibrary::GetInstance().GetGlobalGraphicsLibCode() + "\n";
		mSourceCode = sourceCode;

		String code = libCode + sourceCode;

		if (scriptList.empty())
		{
			mScriptCodeAnalyzer->InsertScript(code, nullptr);
		}
		else
		{
			for (auto& script : scriptList)
			{
				mScriptCodeAnalyzer->InsertScript(code, script);
			}
		}


		mFullSourceCode = code;
		mErrorMessage = "";
		mIsCompileSuccess = Compile(code, &mErrorMessage);
		return mIsCompileSuccess;
	}
	void DirectX12GraphicsShader::ForEach_TextureSlot(const std::function<void(const String&)>& action)
	{
		if (action == nullptr)
		{
			return;
		}
		for (auto& _pair : mScriptCodeAnalyzer->GetSortedTextureMap())
		{
			action(_pair.second);
		}
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
			*error += (char*)errorData->GetBufferPointer();
			*error += "\n";
			return false;
		}
		mIsCompileSuccess = true;
		return true;
	}

	const String& DirectX12ComputeShader::GetName() const
	{
		return mName;
	}

	const JG::String& DirectX12ComputeShader::GetErrorMessage() const
	{
		return mErrorMessage;
	}

	const String& DirectX12ComputeShader::GetShaderCode() const
	{
		return mSourceCode;
	}
	bool DirectX12ComputeShader::IsSuccessed() const
	{
		return mIsCompileSuccess;
	}
	void DirectX12ComputeShader::SetName(const String& name)
	{
		mName = name;
	}
	bool DirectX12ComputeShader::Init(const String& sourceCode)
	{
		String libCode = ShaderLibrary::GetInstance().GetGlobalShaderLibCode() + "\n";
		mSourceCode = libCode + sourceCode;
		mErrorMessage = "";
		mIsCompileSuccess = Compile(mCSData, mSourceCode, CompileConfig(HLSL::CSEntry, HLSL::CSTarget), &mErrorMessage);
		return mIsCompileSuccess;
	}


	bool DirectX12ComputeShader::Compile(ComPtr<IDxcBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error)
	{
		blob = CompileShader(GetName(), sourceCode, config.Entry, config.Target);

		if (blob != nullptr)
		{
			mIsCompileSuccess = true;
		}
		return mIsCompileSuccess;
	}
	const String& DirectX12ClosestHitShader::GetName() const
	{
		return mName;
	}
	const String& DirectX12ClosestHitShader::GetEntryPoint() const
	{
		return mEntryPoint;
	}
	const String& DirectX12ClosestHitShader::GetHitGroupName() const
	{
		return mHitGroupName;
	}
	const String& DirectX12ClosestHitShader::GetShaderCode() const
	{
		return mSourceCode;
	}
	const String& DirectX12ClosestHitShader::GetFullShaderCode() const
	{
		return mFullSourceCode;
	}
	const String& DirectX12ClosestHitShader::GetErrorMessage() const
	{
		return mErrorMessage;
	}
	bool DirectX12ClosestHitShader::IsSuccessed() const
	{
		return mIsCompileSuccess;
	}

	bool DirectX12ClosestHitShader::Init(const String& sourceCode, SharedPtr<IShaderScript> script)
	{
		mScriptCodeAnalyzer = CreateUniquePtr< ShaderScriptCodeAnalyzer>(
			0, 1,
			3, 1);
		String Name = StringHelper::ReplaceAll(GetName(), "/", "_");
		mHitGroupName = Name + "_HitGroup";
		mEntryPoint   = Name + "_ClosestHit";
		mSourceCode   = sourceCode;
		mFullSourceCode = StringHelper::ReplaceAll(mSourceCode, HLSL::ClosestHitEntry, mEntryPoint);
		mScriptCodeAnalyzer->InsertScript(mFullSourceCode, script);

		String shaderLibCode = ShaderLibrary::GetInstance().GetGlobalShaderLibCode() + ShaderLibrary::GetInstance().GetGlobalRayTracingLibCode();;
		mFullSourceCode = shaderLibCode + mFullSourceCode;
		mErrorMessage.clear();

		mBlobData = CompileShader(GetName(), mFullSourceCode, mEntryPoint, HLSL::RTTarget, &mErrorMessage);
		mIsCompileSuccess = mErrorMessage.length() == 0;
		return mIsCompileSuccess;
	}

	void DirectX12ClosestHitShader::SetName(const String& name)
	{
		mName = name;
	}
	void ShaderScriptCodeAnalyzer::InsertScript(String& code, SharedPtr<IShaderScript> script)
	{
		if (script == nullptr)
		{
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceResources, "");
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceVariables, "");
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceFunction, "");
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceContents, "");

			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneResources, "");
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneVariables, "");
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneFunction, "");
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneContents, "");
		}
		else
		{
			if (InsertScriptInternal(code, script) == false)
			{
				JG_LOG_ERROR("Fail Insert ShaderScript : {0}", script->GetName());
			}
		}
	}
	const List<std::pair<EShaderDataType, String>>& ShaderScriptCodeAnalyzer::GetPropertyList() const
	{
		return mPropertyList;
	}
	const SortedDictionary<u64, String>& ShaderScriptCodeAnalyzer::GetSortedTextureMap() const
	{
		return mSortedTextureMap;
	}
	bool ShaderScriptCodeAnalyzer::InsertScriptInternal(String& code, SharedPtr<IShaderScript> script)
	{
		String scriptCode = script->GetCode();
		String resourcesCode;
		if (ExtractScriptContents(scriptCode, ShaderDefine::Type::Resources, resourcesCode) == true)
		{
			//
			u64 lineStart = 0;
			u64 lineEnd = 0;
			u32 texture2dCnt = mStartTexRegisterNum;
			//u32 textureCubeCnt = mStartTexCubeReigsterNum;
			while (true)
			{
				lineEnd = resourcesCode.find(";", lineStart);
				if (lineEnd == String::npos) break;
				auto line = resourcesCode.substr(lineStart, lineEnd - lineStart);
				auto mid = line.find_last_of(" ");
				auto type = line.substr(0, mid); type = StringHelper::ReplaceAll(type, "\n", "");  type = StringHelper::ReplaceAll(type, "\t", ""); type = StringHelper::ReplaceAll(type, " ", "");
				auto name = line.substr(mid + 1, line.length() - mid - 1);
				name = StringHelper::ReplaceAll(name, "\n", "");  name = StringHelper::ReplaceAll(name, "\t", "");
				mPropertyList.push_back(std::pair<EShaderDataType, String>(StringToShaderDataType(type), name));
				if (type == HLSL::Token::Texture2D || type == HLSL::Token::TextureCube)
				{
					u64 resNum = texture2dCnt;
					String registerSpace = std::to_string(mStartTexRegisterSpace);
					String registerNum = std::to_string(texture2dCnt++);
					String registerStr = " : register(t" + registerNum + ", space" + registerSpace + ")";
					resourcesCode.insert(lineEnd, registerStr);
					lineEnd += registerStr.length();
					mSortedTextureMap[resNum] = name;
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
				auto type = line.substr(0, mid); type = StringHelper::ReplaceAll(type, "\n", "");  type = StringHelper::ReplaceAll(type, "\t", ""); type = StringHelper::ReplaceAll(type, " ", "");
				auto name = line.substr(mid + 1, line.length() - mid - 1);
				name = StringHelper::ReplaceAll(name, "\n", "");  name = StringHelper::ReplaceAll(name, "\t", "");
				mPropertyList.push_back(std::pair<EShaderDataType, String>(StringToShaderDataType(type), name));
				lineStart = lineEnd + 1;
			}
			String registerStr = " : register(b" + std::to_string(mStartCBReigsterNum) + ", space" + std::to_string(mStartCBRegisterSpace) + ")";
			variablesCode = String(HLSL::Token::CBuffer) + "__ScriptConstantBuffer__" + registerStr + "\n { \n" + variablesCode;
			variablesCode += "\n};";
		}

		// Function 코드를 마지막에
		String functionCode;
		if (ExtractScriptContents(scriptCode, ShaderDefine::Type::Function, functionCode) == true)
		{

		}


		String contentsCode;
		EShaderScriptType type = EShaderScriptType::Surface;
		if (ExtractScriptContents(scriptCode, ShaderDefine::Type::Surface, contentsCode) == true)
		{
			type = EShaderScriptType::Surface;
		}
		else if (ExtractScriptContents(scriptCode, ShaderDefine::Type::Scene, contentsCode) == true)
		{
			type = EShaderScriptType::Scene;
		}



		switch (type)
		{
		case EShaderScriptType::Surface:
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceResources, resourcesCode);
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceVariables, variablesCode);
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceFunction, functionCode);
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SurfaceContents, contentsCode);
			break;
		case EShaderScriptType::Scene:
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneResources, resourcesCode);
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneVariables, variablesCode);
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneFunction, functionCode);
			code = StringHelper::ReplaceAll(code, ShaderDefine::Location::SceneContents, contentsCode);
			break;
		default:
			return false;
		}
		return true;
	}
	bool ShaderScriptCodeAnalyzer::ExtractScriptContents(const String& code, const String& key, String& out_code)
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
}

