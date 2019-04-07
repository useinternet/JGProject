#include<PCH.h>
#include"Shader.h"
#include"DxUtil.h"
#include<Util/StringUtil.h>
#include<Util/Hash.h>

using namespace Dx12;
using namespace std;
using namespace Common;
Shader::ShaderMap Shader::ms_GlobalShaders;
map<EShaderStage, string> Shader::ms_EntryPoints;
map<EShaderStage, string> Shader::ms_HlslVersion;


mutex Shader::ms_GlobalShadersMutex;
mutex Shader::ms_EntryPointsMutex;
mutex Shader::ms_HlslVersionMutex;
bool Shader::ms_IsInitShaderConfig = false;


uint32_t D3D_Macro_Hash(const vector<D3D_SHADER_MACRO>& vec, EShaderStage stage, const string& entryFunc)
{
	uint32_t seed = (uint32_t)vec.size() + stage;
	for (auto& i : vec)
	{
		uint32_t pair_hash_code = (uint32_t)Util::pair_hash()(pair< LPCSTR, LPCSTR>(i.Name, i.Definition));
		seed ^= pair_hash_code + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	pair<uint32_t, string> pairSeed = pair<uint32_t, string>(seed, entryFunc);
	seed = (uint32_t)Util::pair_hash()(pairSeed);


	return seed;
}
Shader::Shader(int maxMacroSize)
{
	m_MaxMacroMemSize = maxMacroSize;
	if (!ms_IsInitShaderConfig) {
		SetShaderDefaultConfig();
	}
	m_MacroStrMems.resize(m_MaxMacroMemSize);
}
void Shader::SetShaderDefaultConfig()
{
	SetEntryPoint(ShaderStage::Vertex, "VS");
	SetEntryPoint(ShaderStage::Pixel, "PS");
	SetEntryPoint(ShaderStage::Hull, "HS");
	SetEntryPoint(ShaderStage::Domain, "DS");
	SetEntryPoint(ShaderStage::Geometry, "GS");
	SetHLSLVersion(HLSLVersion::_5_1);
	ms_IsInitShaderConfig = true;
}
void Shader::SetEntryPoint(EShaderStage stage, const string& entryPoint)
{
	if (stage == ShaderStage::Compute)
	{
		DX12_LOG_WARNING("ComputeShader's entry name have to define it yourself. ");
		return;
	}
	lock_guard<mutex> lock(ms_EntryPointsMutex);
	ms_EntryPoints[stage] = entryPoint;
}
void Shader::SetHLSLVersion(EHLSLVersion version)
{
	lock_guard<mutex> lock(ms_HlslVersionMutex);
	std::string versionStr;
	switch (version)
	{
	case HLSLVersion::_4_0:
		versionStr = "_4_0";
		break;
	case HLSLVersion::_5_0:
		versionStr = "_5_0";
		break;
	case HLSLVersion::_5_1:
		versionStr = "_5_1";
		break;
	}
	ms_HlslVersion[ShaderStage::Vertex]   = "vs" + versionStr;
	ms_HlslVersion[ShaderStage::Pixel]    = "ps" + versionStr;
	ms_HlslVersion[ShaderStage::Hull]     = "hs" + versionStr;
	ms_HlslVersion[ShaderStage::Domain]   = "ds" + versionStr;
	ms_HlslVersion[ShaderStage::Geometry] = "gs" + versionStr;
	ms_HlslVersion[ShaderStage::Compute]  = "cs" + versionStr;
}
GraphicsShader::GraphicsShader(int maxMacroSize) : Shader(maxMacroSize)
{

}
void GraphicsShader::AddMacro(EShaderStage stage, const string& define, const string& value)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	if (stage == ShaderStage::Compute)
	{
		DX12_LOG_WARNING("ComputeStage can't use in GraphicShader .. so ignore");
		return;
	}
	//
	if (m_MacroMemOffset >= m_MaxMacroMemSize)
	{
		DX12_LOG_WARNING("Macro ( define : %s , value : %s ) Max Size over so.. ignore",
			define.c_str(), value.c_str());
	}

	uint32_t offset = m_MacroMemOffset;
	m_MacroStrMems[offset]     = move(define);
	m_MacroStrMems[offset + 1] = move(value);


	m_ShaderCodeDatas[stage].Macro.push_back({
		m_MacroStrMems[offset].c_str(), 
		m_MacroStrMems[offset + 1].c_str() });

	m_MacroMemOffset += 2;
	//
}
void GraphicsShader::AddMacros(const ShaderStages& stages, const string& define, const string& value)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	uint32_t len = (uint32_t)stages.size();
	for (uint32_t i = 0; i < len; ++i)
	{
		AddMacro(stages[i], define, value);
	}
}
void GraphicsShader::AddShaderPath(EShaderStage stage, const string& path)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	if (stage == ShaderStage::Compute)
	{
		DX12_LOG_WARNING("ComputeStage can't use in GraphicShader .. so ignore");
		return;
	}
	m_ShaderCodeDatas[stage].Path = move(path);
	m_ShaderCodeDatas[stage].IsNonePath = false;
}
void GraphicsShader::AddShaderPaths(const ShaderStages& stages, const string& path)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	for (uint32_t i = 0; i < (uint32_t)stages.size(); ++i)
	{
		AddShaderPath(stages[i], path);
	}
}

void GraphicsShader::AddShaderCode(EShaderStage stage, const ShaderCharCode& code)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	m_ShaderCodeDatas[stage].IsNonePath = true;
	m_ShaderCodeDatas[stage].CharCode   = move(code);
}
void GraphicsShader::AddShaderCodes(const ShaderStages& stages, const ShaderCharCodes& codes)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	for (uint32_t i = 0; i < (uint32_t)stages.size(); ++i)
	{
		AddShaderCode(stages[i], codes[i]);
	}
}
const void* GraphicsShader::GetShaderCode(EShaderStage stage) const
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return nullptr;
	}
	if (stage == ShaderStage::Compute)
	{
		DX12_LOG_WARNING("ComputeStage can't use in GraphicShader .. so ignore");
		return nullptr;
	}

	return m_ShaderCodeDatas.at(stage).BtCode->GetBufferPointer();
}
uint32_t    GraphicsShader::GetShaderCodeSize(EShaderStage stage) const
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return 0;
	}
	if (stage == ShaderStage::Compute)
	{
		DX12_LOG_WARNING("ComputeStage can't use in GraphicShader .. so ignore");
		return 0;
	}
	return (uint32_t)m_ShaderCodeDatas.at(stage).BtCode->GetBufferSize();
}
const string& GraphicsShader::GetErrorCode(EShaderStage stage) const
{
	if (stage == ShaderStage::Compute)
	{
		DX12_LOG_WARNING("ComputeStage can't use in GraphicShader .. so ignore");
	}
	return m_ShaderCodeDatas.at(stage).Error;
}
bool GraphicsShader::Compile()
{
	bool result = true;
	// 각 단계별 셰이더 데이터 루프
	for (auto& iter : m_ShaderCodeDatas)
	{
		EShaderStage shaderStage = iter.first;
		ShaderData   shaderData  = iter.second;
		uint32_t     macroHashCode = D3D_Macro_Hash(shaderData.Macro, shaderStage, " ");
		shaderData.Macro.push_back({ nullptr,nullptr });
		bool is_sucess_find = true;
		// 패쓰가 저장되어있지 않다면..
		if (!shaderData.IsNonePath)
		{
			string path = shaderData.Path;
			lock_guard<mutex> lock(ms_GlobalShadersMutex);
			// 셰이더 경로 해쉬 탐색
			if (ms_GlobalShaders.find(path) != ms_GlobalShaders.end())
			{
				// 매크로 해쉬 탐색
				if (ms_GlobalShaders[path].find(macroHashCode) != ms_GlobalShaders[path].end())
				{
					shaderData.BtCode = ms_GlobalShaders[path][macroHashCode];
				}
				else
				{
					is_sucess_find = false;
				}
			}
			// 셰이더 경로 가 탐색되지 않았다면
			else
			{
				is_sucess_find = false;
			}
			if (!is_sucess_find)
			{
				bool is_compile_success;
				// 컴파일 
				
				shaderData.BtCode = DxUtil::CompileShader(
					Util::AnsiToWString(path),
					shaderData.Macro.data(),
					ms_EntryPoints[shaderStage],
					ms_HlslVersion[shaderStage],
					&is_compile_success,
					&shaderData.Error);

				// 실패시 로그 출력
				if (!is_compile_success) {
					DX12_LOG_ERROR("%s 's failed compile", path.c_str());
					DX12_LOG_ERROR("%s : %s ", path.c_str(), shaderData.Error.c_str());
					result = false;
				}
				else
				{
					ms_GlobalShaders[path][macroHashCode] = shaderData.BtCode;
				}
		
			}
		}
		else
		{
			std::string charCode = shaderData.CharCode;
			bool is_sucess_find = true;
			lock_guard<mutex> lock(ms_GlobalShadersMutex);
			if (ms_GlobalShaders.find(charCode) != ms_GlobalShaders.end())
			{
				if (ms_GlobalShaders[charCode].find(macroHashCode) != ms_GlobalShaders[charCode].end())
				{
					shaderData.BtCode = ms_GlobalShaders[charCode][macroHashCode];
				}
				else
				{
					is_sucess_find = false;
				}
			}
			else
			{
				is_sucess_find = false;
			}


			if (!is_sucess_find)
			{
				ComPtr<ID3DBlob> error;
				HRESULT hr = D3DCompile(
					charCode.c_str(),
					charCode.length(),
					nullptr,
					shaderData.Macro.data(),
					nullptr,
					ms_EntryPoints[shaderStage].c_str(),
					ms_HlslVersion[shaderStage].c_str(),
					0, 0, shaderData.BtCode.GetAddressOf(), error.GetAddressOf());
				if (error)
				{
					shaderData.Error = string((char*)error->GetBufferPointer());
				}
				if (FAILED(hr))
				{
					DX12_LOG_ERROR("%s 's failed compile", shaderData.CharCode.c_str());
					DX12_LOG_ERROR("%s : %s ", shaderData.CharCode.c_str(), shaderData.Error.c_str());
					result = false;
				}
				else
				{
					ms_GlobalShaders[charCode][macroHashCode] = shaderData.BtCode;
				}
			}

		}

		iter.second = shaderData;
	}
	return result;
}

ComputeShader::ComputeShader(int maxMacroSize) : Shader(maxMacroSize)
{

}

void ComputeShader::AddMacro(
	const string& funcName,
	const string& define,
	const string value)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}


	if (m_MacroMemOffset >= m_MaxMacroMemSize)
	{
		DX12_LOG_WARNING("Macro ( define : %s , value : %s ) Max Size over so.. ignore",
			define.c_str(), value.c_str());
	}

	uint32_t offset = m_MacroMemOffset;
	m_MacroStrMems[offset] = move(define);
	m_MacroStrMems[offset + 1] = move(value);


	m_ShaderCodeDatas[funcName].Macro.push_back({
		m_MacroStrMems[offset].c_str(),
		m_MacroStrMems[offset + 1].c_str() });

	m_MacroMemOffset += 2;
}

void ComputeShader::AddMacros(
	const vector<string>& funcName,
	const string& define,
	const string value)
{
	for (auto& func : funcName)
	{
		AddMacro(func, define, value);
	}
}
void ComputeShader::SetShaderPathAndFunction(
	const string& path,
	const string& funcName)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	if (m_ShaderCodeDatas.find(funcName) != m_ShaderCodeDatas.end())
	{
		DX12_LOG_WARNING("This Compute Shader is already exsit so.. discover new data");
	}
	m_ShaderCodeDatas[funcName].Path = path;
	m_ShaderCodeDatas[funcName].IsNonePath = false;
}
void ComputeShader::SetShaderPathAndFunctions(
	const string& path,
	const vector<string>& funcNames)
{
	for (auto& func : funcNames)
	{
		SetShaderPathAndFunction(path, func);
	}

}

void ComputeShader::SetShaderCode(const string& funcName, const ShaderCharCode& code)
{
	if (m_IsCompile)
	{
		DX12_LOG_WARNING("Current Shader is already compile .. so ignore");
		return;
	}
	if (m_ShaderCodeDatas.find(funcName) != m_ShaderCodeDatas.end())
	{
		DX12_LOG_WARNING("This Compute Shader is already exsit so.. discover new data");
	}

	m_ShaderCodeDatas[funcName].CharCode = code;
	m_ShaderCodeDatas[funcName].IsNonePath = true;
}
void ComputeShader::SetShaderCode(const vector<string>& funcName, const ShaderCharCodes& code)
{
	for (uint32_t i = 0; i < code.size(); ++i)
	{
		SetShaderCode(funcName[i], code[i]);
	}
}

const void* ComputeShader::GetShaderCode(const string& funcName) const
{
	if (m_ShaderCodeDatas.find(funcName) == m_ShaderCodeDatas.end())
	{
		return nullptr;
	}
	return m_ShaderCodeDatas.at(funcName).BtCode->GetBufferPointer();
}
uint32_t ComputeShader::GetShaderCodeSize(const string& funcName) const
{
	return (uint32_t)m_ShaderCodeDatas.at(funcName).BtCode->GetBufferSize();
}

const string& ComputeShader::GetErrorCode(const string& funcName) const
{
	return m_ShaderCodeDatas.at(funcName).Error;
}

bool ComputeShader::Compile()
{
	bool result = true;


	for (auto& iter : m_ShaderCodeDatas)
	{
		string funcName = iter.first;
		ShaderData data = iter.second;
		uint32_t hashCode = D3D_Macro_Hash(iter.second.Macro, ShaderStage::Compute, funcName);
		data.Macro.push_back({ nullptr, nullptr });
		if (!data.IsNonePath)
		{
			string path = iter.second.Path;
			bool is_success_find = true;

			lock_guard<mutex> lock(ms_GlobalShadersMutex);
			if (ms_GlobalShaders.find(path) != ms_GlobalShaders.end())
			{

				if (ms_GlobalShaders[path].find(hashCode) != ms_GlobalShaders[path].end())
				{
					data.BtCode = ms_GlobalShaders[path][hashCode];
				}
				else
				{
					is_success_find = false;
				}
			}
			else
			{
				is_success_find = false;
			}

			if (!is_success_find)
			{
				bool is_compile_success = false;
				string errorStr;
				data.BtCode = DxUtil::CompileShader(
					Util::AnsiToWString(path),
					data.Macro.data(),
					funcName,
					ms_HlslVersion[ShaderStage::Compute],
					&is_compile_success,
					&errorStr);


				if (!is_compile_success)
				{
					DX12_LOG_ERROR("%s 's failed compile", path.c_str());
					DX12_LOG_ERROR("%s : %s ", path.c_str(), data.Error.c_str());
					result = false;
				}
				else
				{

					ms_GlobalShaders[path][hashCode] = data.BtCode;
				}
			}
		}
		else
		{
			string CharCode = data.CharCode;
			bool is_success_find = true;
			lock_guard<mutex> lock(ms_GlobalShadersMutex);
			if (ms_GlobalShaders.find(CharCode) != ms_GlobalShaders.end())
			{
				if (ms_GlobalShaders[CharCode].find(hashCode) != ms_GlobalShaders[CharCode].end())
				{
					data.BtCode = ms_GlobalShaders[CharCode][hashCode];
				}
				else
				{
					is_success_find = false;
				}
			}
			else
			{
				is_success_find = false;
			}

			if (!is_success_find)
			{
				ComPtr<ID3DBlob> error;
				HRESULT hr = D3DCompile(
					CharCode.c_str(),
					CharCode.length(),
					nullptr,
					data.Macro.data(),
					nullptr,
					funcName.c_str(),
					ms_HlslVersion[ShaderStage::Compute].c_str(),
					0, 0,
					data.BtCode.GetAddressOf(),
					error.GetAddressOf());

				if (error)
				{
					data.Error = string((char*)error->GetBufferPointer());

				}
				if (FAILED(hr))
				{
					DX12_LOG_ERROR("%s 's failed compile", data.CharCode.c_str());
					DX12_LOG_ERROR("%s : %s ", data.CharCode.c_str(), data.Error.c_str());
					result = false;
				}
				else
				{
					ms_GlobalShaders[CharCode][hashCode] = data.BtCode;
				}
			}

		

		}

		iter.second = data;

	}

	return result;
}