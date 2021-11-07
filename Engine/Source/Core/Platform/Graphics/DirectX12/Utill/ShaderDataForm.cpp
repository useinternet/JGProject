#include "pch.h"
#include "ShaderDataForm.h"
#include "CommandList.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/DirectX12Shader.h"
#include "Platform/Graphics/DirectX12/DirectX12Resource.h"

namespace JG
{
	bool ShaderDataForm::Set(String& code)
	{
		bool result = true;
		Reset();


		u64 pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisStruct(code, pos, &result);
		}

		// Upload Data 추출
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisCBuffer(code, pos, &result);
		}
		// StructuredBuffer
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisStructuredBuffer(code, pos, &result);
		}
		// RWStructuredBuffer
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisRWStructuredBuffer(code, pos, &result);
		}

		// Texture
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisTexture2D(code, pos, &result);
		}
		// RWTexture
		pos = 0;
		{
			pos = AnalysisRWTexture2D(code, pos, &result);
		}

		// SamplerState
		pos = 0;
		while (pos != String::npos)
		{
			pos = AnalysisSamplerState(code, pos, &result);
		}


		// Sampler는 이미 저

		return result;
	}
	void ShaderDataForm::Reset()
	{
		RootParamOffset = 0;
		SpaceOffset = 0;
		TextureRegisterNumberOffset = 0;
		TextureCubeRegisterNumberOffset = 0;
		SamplerStateRegisterNumberOffset = 0;
		RootParamMap.clear();
		CBufferVarMap.clear();
		CBufferDataMap.clear();
		TextureDataMap.clear();
		SamplerStateDataMap.clear();
	}

	u64 ShaderDataForm::AnalysisStruct(const String& code, u64 startPos, bool* result)
	{
		u64 dataTokenStartPos = code.find(HLSL::Token::Struct, startPos);
		StructData* structData = nullptr;
		if (dataTokenStartPos != String::npos)
		{
			String structName;
			ExtractStructName(code, dataTokenStartPos, &structName);
			if (RegisterStruct(structName) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}
			structData = StructDataMap[structName].get();
			structData->Name = structName;
		}
		else
		{
			return String::npos;
		}


		dataTokenStartPos = code.find_first_of("{", dataTokenStartPos) + 1;
		if (dataTokenStartPos != String::npos)
		{
			u64 dataTokenEndPos = code.find_first_of("}", dataTokenStartPos);
			String dataCode = code.substr(dataTokenStartPos, dataTokenEndPos - dataTokenStartPos);

			u64 pos = 0;
			while (pos != String::npos)
			{
				String varCode;
				pos = ExtractVarCode(dataCode, pos, &varCode);
				if (pos == String::npos)
					break;
				if (RegisterStructVar(structData, varCode) == false)
				{
					if (result != nullptr)
					{
						*result = false;
					}
					return String::npos;
				}
			}

			startPos = dataTokenEndPos + 1;
		}
		else
		{
			return String::npos;
		}


		return startPos;
	}

	u64 ShaderDataForm::AnalysisCBuffer(String& code, u64 startPos, bool* result)
	{
		CBufferData* cBuffer = nullptr;
		u64 uploadDataSize = 0;
		u64 dataTokenStartPos = code.find(HLSL::Token::CBuffer, startPos);
		u64 endPos = String::npos;
		if (dataTokenStartPos != String::npos)
		{
			endPos = code.find("\n", dataTokenStartPos);
			if (endPos == String::npos) {
				endPos = code.find("{", dataTokenStartPos);
			}
			
			String cbName;
			ExtractCBufferName(code, dataTokenStartPos, &cbName);
			if (RegisterCBuffer(cbName) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}
			cBuffer = CBufferDataMap[cbName].get();
			cBuffer->Name = cbName;
		}
		else
		{
			return String::npos;
		}

		// Var 처리
		dataTokenStartPos = code.find_first_of("{", dataTokenStartPos) + 1;
		if (dataTokenStartPos != String::npos)
		{
			u64 dataTokenEndPos = code.find_first_of("}", dataTokenStartPos);
			String dataCode = code.substr(dataTokenStartPos, dataTokenEndPos - dataTokenStartPos);

			u64 pos = 0;
			while (pos != String::npos)
			{
				String varCode;
				pos = ExtractVarCode(dataCode, pos, &varCode);
				if (pos == String::npos)
					break;
				if (RegisterCBufferVar(cBuffer, varCode, uploadDataSize) == false)
				{
					if (result != nullptr)
					{
						*result = false;
					}
					return String::npos;
				}
			}

			startPos = dataTokenEndPos + 1;
		}
		else
		{
			return String::npos;
		}

		cBuffer->DataSize = uploadDataSize;
		cBuffer->ElementType = HLSL::EHLSLElement::CBuffer;
		cBuffer->RootParm = RootParamOffset++;
		cBuffer->RegisterNum = (u32)CBufferDataMap.size() - 1;

		code.insert(endPos, " : register(b" + std::to_string(CBufferRegisterNumberOffset++) + ")");

		RootParamMap[cBuffer->RootParm] = cBuffer;
		return startPos;
	}
	u64 ShaderDataForm::AnalysisStructuredBuffer(String& code, u64 startPos, bool* result)
	{
		u64 dataTokenStartPos = code.find(HLSL::Token::StructuredBuffer, startPos);
		if (dataTokenStartPos != String::npos && dataTokenStartPos >= 2)
		{
			String token = code.substr(dataTokenStartPos - 2, strlen(HLSL::Token::RWStructuredBuffer));
			if (token == HLSL::Token::RWStructuredBuffer)
			{
				return code.find(";", dataTokenStartPos) + 1;
			}
		}

		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(";", dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			// Type
			u64 dataTypeStartPos = dataCode.find("<") + 1;
			u64 dataTypeEndPos = dataCode.find(">");



			String typeCode = dataCode.substr(dataTypeStartPos, dataTypeEndPos - dataTypeStartPos);
			typeCode = ReplaceAll(typeCode, " ", "");

		
			String typeName;
			u64 typeSize = 0;
			if (FindTypeInfo(typeCode, &typeName, &typeSize) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}





			String nameCode = dataCode.substr(dataTypeEndPos + 1, dataCode.length() - dataTypeEndPos - 1);
			nameCode = ReplaceAll(nameCode, " ", "");

			if (RegisterStructuredBuffer(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto structuredBufferData = StructuredBufferDataMap[nameCode].get();
			structuredBufferData->Type = typeName;
			structuredBufferData->Name = nameCode;
			structuredBufferData->RootParm = RootParamOffset++;
			structuredBufferData->RegisterNum = 0;
			structuredBufferData->RegisterSpace = (u32)(ShaderDataForm::StructuredBufferStartSpace + SpaceOffset++);
			structuredBufferData->ElementType = HLSL::EHLSLElement::StructuredBuffer;
			structuredBufferData->ElementDataSize = typeSize;
			RootParamMap[structuredBufferData->RootParm] = structuredBufferData;



			code.insert(endPos, " : register(t0, space" + std::to_string(structuredBufferData->RegisterSpace) + ")");
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 ShaderDataForm::AnalysisTexture2D(String& code, u64 startPos, bool* result)
	{

		u64 dataTokenStartPos = code.find(HLSL::Token::Texture2D, startPos);
		if (dataTokenStartPos != String::npos && dataTokenStartPos >= 2)
		{
			String token = code.substr(dataTokenStartPos - 2, strlen(HLSL::Token::RWTexture2D));
			if (token == HLSL::Token::RWTexture2D)
			{
				return code.find(";", dataTokenStartPos) + 1;
			}
		}
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(";", dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			String nameCode = ReplaceAll(dataCode, HLSL::Token::Texture2D, "");
			nameCode = ReplaceAll(nameCode, " ", "");

			u64 arraySize = 1;

			u64 arrayStartPos = dataCode.find("[");
			if (arrayStartPos != String::npos)
			{

				nameCode = nameCode.substr(0, nameCode.find("["));



				arrayStartPos += 1;
				u64 arrayEndPos = dataCode.find("]", arrayStartPos);


				String arraySizeCode = dataCode.substr(arrayStartPos, arrayEndPos - arrayStartPos);
				arraySizeCode = ReplaceAll(arraySizeCode, " ", "");

				arraySize = atol(arraySizeCode.c_str());
			}

			if (RegisterTextureData(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto textureData = TextureDataMap[nameCode].get();
			textureData->Name = nameCode;
			textureData->RootParm = RootParamOffset++;
			textureData->RegisterNum = (u32)TextureRegisterNumberOffset; TextureRegisterNumberOffset += arraySize;
			textureData->RegisterSpace = ShaderDataForm::Texture2DStartSpace;
			textureData->ElementType = HLSL::EHLSLElement::Texture;
			textureData->Type = HLSL::EHLSLTextureType::_2D;
			textureData->TextureCount = arraySize;
			RootParamMap[textureData->RootParm] = textureData;


			code.insert(endPos,
				" : register(t" + std::to_string(TextureDataMap[nameCode]->RegisterNum) +
				", space" + std::to_string(TextureDataMap[nameCode]->RegisterSpace) + ")");
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 ShaderDataForm::AnalysisSamplerState(String& code, u64 startPos, bool* result)
	{
		// TODO SamplerState 분석

		u64 dataTokenStartPos = code.find(HLSL::Token::SamplerState, startPos);

		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(";", dataTokenStartPos);
			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);
			u64 samplerDataStartPos = dataCode.find("{");

			String nameCode = dataCode.substr(0, samplerDataStartPos);
			nameCode = ReplaceAll(nameCode, HLSL::Token::SamplerState, "");
			nameCode = ReplaceAll(nameCode, " ", "");
			nameCode = ReplaceAll(nameCode, "\n", "");
			nameCode = ReplaceAll(nameCode, "\t", "");

			if (samplerDataStartPos != String::npos)
			{
				samplerDataStartPos += 1;
				u64 samplerDataEndPos = dataCode.find("}", samplerDataStartPos);

				String samplerDataCode = dataCode.substr(samplerDataStartPos, samplerDataEndPos - samplerDataStartPos) + ",";

				if (samplerDataCode.find_last_of(",") != samplerDataCode.length() - 1)
				{
					samplerDataCode += ",";
				}
				Dictionary<String, String> SamplerDataMap;

				u64 pos = 0;
				while (pos != String::npos)
				{
					String key; String value;
					pos = ExtractSamplerStateValue(samplerDataCode, pos, &key, &value);
					if (pos != String::npos)
					{
						SamplerDataMap[key] = value;
					}
				}

				if (RegisterSamplerStateData(nameCode) == false)
				{
					if (result != nullptr)
					{
						*result = false;
					}
					return String::npos;
				}

				auto samplerStateData = SamplerStateDataMap[nameCode].get();
				samplerStateData->Desc = CreateSamplerStateDesc(SamplerDataMap);


				samplerStateData->Name = nameCode;
				samplerStateData->RegisterNum = (u32)SamplerStateRegisterNumberOffset++;
				samplerStateData->Desc.ShaderRegister = samplerStateData->RegisterNum;
				samplerStateData->RegisterSpace = 0;
				samplerStateData->ElementType = HLSL::EHLSLElement::SamplerState;

				u64 replaceStartPos = code.find("{", dataTokenStartPos);
				u64 replaceEndPos = code.find(";", replaceStartPos) + 1;
				code = ReplaceAll(code, code.substr(replaceStartPos, replaceEndPos - replaceStartPos), "");


				u64 insertStartPos = code.find(nameCode, dataTokenStartPos) + nameCode.length();
				String additionalCode = " : register(s" + std::to_string(samplerStateData->RegisterNum) +
					", space" + std::to_string(samplerStateData->RegisterSpace) + ");";

				code.insert(insertStartPos, additionalCode);
				endPos = insertStartPos + additionalCode.length();
			}
			else
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 ShaderDataForm::AnalysisRWStructuredBuffer(String& code, u64 startPos, bool* result)
	{
		u64 dataTokenStartPos = code.find(HLSL::Token::RWStructuredBuffer, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(";", dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			// Type
			u64 dataTypeStartPos = dataCode.find("<") + 1;
			u64 dataTypeEndPos = dataCode.find(">");



			String typeCode = dataCode.substr(dataTypeStartPos, dataTypeEndPos - dataTypeStartPos);
			typeCode = ReplaceAll(typeCode, " ", "");

			String typeName;
			u64 typeSize = 0;
			if (FindTypeInfo(typeCode, &typeName, &typeSize) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			String nameCode = dataCode.substr(dataTypeEndPos + 1, dataCode.length() - dataTypeEndPos - 1);
			nameCode = ReplaceAll(nameCode, " ", "");

			if (RegisterRWStructuredBuffer(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto structuredBufferData = RWStructuredBufferDataMap[nameCode].get();
			structuredBufferData->Type = typeName;
			structuredBufferData->Name = nameCode;
			structuredBufferData->RootParm = RootParamOffset++;
			structuredBufferData->RegisterNum = 0;
			structuredBufferData->RegisterSpace = (u32)(ShaderDataForm::StructuredBufferStartSpace + SpaceOffset++);
			structuredBufferData->ElementType = HLSL::EHLSLElement::RWStructuredBuffer;
			structuredBufferData->ElementDataSize = typeSize;
			RootParamMap[structuredBufferData->RootParm] = structuredBufferData;



			code.insert(endPos, " : register(u0, space" + std::to_string(structuredBufferData->RegisterSpace) + ")");
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 ShaderDataForm::AnalysisRWTexture2D(String& code, u64 startPos, bool* result)
	{
		u64 dataTokenStartPos = code.find(HLSL::Token::RWTexture2D, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(";", dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			String nameCode = ReplaceAll(dataCode, HLSL::Token::RWTexture2D, "");
			nameCode = ReplaceAll(nameCode, " ", "");

			u64 arraySize = 1;

			u64 arrayStartPos = dataCode.find("[");
			if (arrayStartPos != String::npos)
			{

				nameCode = nameCode.substr(0, nameCode.find("["));



				arrayStartPos += 1;
				u64 arrayEndPos = dataCode.find("]", arrayStartPos);


				String arraySizeCode = dataCode.substr(arrayStartPos, arrayEndPos - arrayStartPos);
				arraySizeCode = ReplaceAll(arraySizeCode, " ", "");

				arraySize = atol(arraySizeCode.c_str());
			}

			if (RegisterRWTextureData(nameCode) == false)
			{
				if (result != nullptr)
				{
					*result = false;
				}
				return String::npos;
			}

			auto textureData = RWTextureDataMap[nameCode].get();
			textureData->Name = nameCode;
			textureData->RootParm = RootParamOffset++;
			textureData->RegisterNum = (u32)TextureRegisterNumberOffset; TextureRegisterNumberOffset += arraySize;
			textureData->RegisterSpace = ShaderDataForm::Texture2DStartSpace;
			textureData->ElementType = HLSL::EHLSLElement::RWTexture;
			textureData->Type = HLSL::EHLSLTextureType::_2D;
			textureData->TextureCount = arraySize;
			RootParamMap[textureData->RootParm] = textureData;


			code.insert(endPos,
				" : register(u" + std::to_string(RWTextureDataMap[nameCode]->RegisterNum) +
				", space" + std::to_string(RWTextureDataMap[nameCode]->RegisterSpace) + ")");
			startPos = endPos + 1;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	bool ShaderDataForm::FindTypeInfo(const String& typeCode, String* out_type, u64* out_typeSize)
	{
		auto type = StringToShaderDataType(typeCode);
		if (type == EShaderDataType::unknown)
		{
			if (StructDataMap.find(typeCode) != StructDataMap.end())
			{
				if (out_type)
				{
					*out_type = typeCode;
				}
				if (out_typeSize)
				{
					*out_typeSize = StructDataMap[typeCode]->DataSize;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (out_type)
			{
				*out_type = typeCode;
			}
			if (out_typeSize)
			{
				*out_typeSize = GetShaderDataTypeSize(type);
			}
		}

		return true;
	}
	void ShaderDataForm::ExtractStructName(const String& code, u64 pos, String* out_value)
	{
		u64 startPos = pos + strlen(HLSL::Token::Struct);
		u64 endPos = code.find_first_of("{", startPos);
		String structName = code.substr(startPos, endPos - startPos);
		structName = ReplaceAll(structName, "\n", "");
		structName = ReplaceAll(structName, "\t", "");
		structName = ReplaceAll(structName, " ", "");

		if (out_value != nullptr)
		{
			*out_value = structName;
		}
	}
	void ShaderDataForm::ExtractCBufferName(const String& code, u64 pos, String* out_value)
	{
		u64 startPos = pos + strlen(HLSL::Token::CBuffer);
		u64 endPos = code.find_first_of("{", startPos);
		String cbName = code.substr(startPos, endPos - startPos);
		cbName = ReplaceAll(cbName, "\n", "");
		cbName = ReplaceAll(cbName, "\t", "");
		cbName = ReplaceAll(cbName, " ", "");

		if (out_value != nullptr)
		{
			*out_value = cbName;
		}

	}
	u64 ShaderDataForm::ExtractVarCode(const String& code, u64 pos, String* out_value)
	{
		u64 startPos = (pos == 0) ? 0 : code.find_first_of("\n", pos) + 1;
		if (startPos != String::npos)
		{
			u64 endPos = code.find_first_of(";", startPos);
			if (endPos != String::npos)
			{
				String varCode = code.substr(startPos, endPos - startPos + 1);
				varCode = ReplaceAll(varCode, "\n", "");
				varCode = ReplaceAll(varCode, "\t", "");

				if (out_value)
				{
					*out_value = varCode;
				}
				else
				{
					return String::npos;
				}
				pos = endPos + 1;
				return pos;
			}
		}
		return String::npos;
	}
	u64 ShaderDataForm::ExtractSamplerStateValue(const String& samplerStateDataCode, u64 startPos, String* out_key, String* out_value)
	{
		auto endPos = samplerStateDataCode.find(",", startPos);
		if (endPos == String::npos)
		{
			return String::npos;
		}

		u64 result = endPos + 1;

		String dataCode = samplerStateDataCode.substr(startPos, endPos - startPos);
		dataCode = ReplaceAll(dataCode, "\t", "");
		dataCode = ReplaceAll(dataCode, "\n", "");


		startPos = 0;
		u64 midPos = dataCode.find("=");
		endPos = dataCode.length();

		if (out_key != nullptr)
		{
			*out_key = dataCode.substr(startPos, midPos - startPos);
			*out_key = ReplaceAll(*out_key, " ", "");
		}

		if (out_value != nullptr)
		{
			*out_value = dataCode.substr(midPos + 1, endPos - midPos - 1);
			*out_value = ReplaceAll(*out_value, " ", "");
		}


		return result;
	}
	D3D12_STATIC_SAMPLER_DESC ShaderDataForm::CreateSamplerStateDesc(const Dictionary<String, String>& samplerDataMap)
	{
		D3D12_STATIC_SAMPLER_DESC desc = {};

		//
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Template) != samplerDataMap.end())
		{
			CreateSamplerStateByTemplate(StringToSamplerStateTemplate(samplerDataMap.at(HLSL::Token::SamplerStateElement::Template)), &desc);
		}
		else
		{
			desc = CD3DX12_STATIC_SAMPLER_DESC(0);
		}

		String Min = HLSL::Token::SamplerSatateFilter::Point;
		String Mag = HLSL::Token::SamplerSatateFilter::Point;
		String Mip = HLSL::Token::SamplerSatateFilter::Point;

		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Min) != samplerDataMap.end())
		{
			Min = samplerDataMap.at(HLSL::Token::SamplerStateElement::Min);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Mag) != samplerDataMap.end())
		{
			Mag = samplerDataMap.at(HLSL::Token::SamplerStateElement::Mag);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::Mip) != samplerDataMap.end())
		{
			Mip = samplerDataMap.at(HLSL::Token::SamplerStateElement::Mip);
		}
		desc.Filter = GetSamplerStateFilter(Min, Mag, Mip);


		String AddressU = HLSL::Token::SamplerSatateAddressMode::Wrap;
		String AddressV = HLSL::Token::SamplerSatateAddressMode::Wrap;
		String AddressW = HLSL::Token::SamplerSatateAddressMode::Wrap;
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::AddressU) != samplerDataMap.end())
		{
			AddressU = samplerDataMap.at(HLSL::Token::SamplerStateElement::AddressU);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::AddressV) != samplerDataMap.end())
		{
			AddressV = samplerDataMap.at(HLSL::Token::SamplerStateElement::AddressV);
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::AddressW) != samplerDataMap.end())
		{
			AddressW = samplerDataMap.at(HLSL::Token::SamplerStateElement::AddressW);
		}
		desc.AddressU = GetTextureAddressMode(AddressU);
		desc.AddressV = GetTextureAddressMode(AddressV);
		desc.AddressW = GetTextureAddressMode(AddressW);


		String ComparisonFunc = HLSL::Token::SamplerStateComparisonFunc::LessEqual;
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::ComparisonFunc) != samplerDataMap.end())
		{
			ComparisonFunc = samplerDataMap.at(HLSL::Token::SamplerStateElement::ComparisonFunc);
		}
		desc.ComparisonFunc = GetComparisonFunc(ComparisonFunc);


		String BorderColor = HLSL::Token::SamplerStateBorderColor::OpaqueBlack;
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::BorderColor) != samplerDataMap.end())
		{
			BorderColor = samplerDataMap.at(HLSL::Token::SamplerStateElement::BorderColor);
		}
		desc.BorderColor = GetBorderColor(BorderColor);


		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MinLOD) != samplerDataMap.end())
		{
			desc.MinLOD = (f32)atof(samplerDataMap.at(HLSL::Token::SamplerStateElement::MinLOD).c_str());
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MaxLOD) != samplerDataMap.end())
		{
			desc.MaxLOD = (f32)atof(samplerDataMap.at(HLSL::Token::SamplerStateElement::MaxLOD).c_str());
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MaxAnisotropy) != samplerDataMap.end())
		{
			desc.MaxAnisotropy = (f32)atoi(samplerDataMap.at(HLSL::Token::SamplerStateElement::MaxAnisotropy).c_str());
		}
		if (samplerDataMap.find(HLSL::Token::SamplerStateElement::MipLODBias) != samplerDataMap.end())
		{
			desc.MipLODBias = (f32)atof(samplerDataMap.at(HLSL::Token::SamplerStateElement::MipLODBias).c_str());
		}
		return desc;
	}

	void ShaderDataForm::CreateSamplerStateByTemplate(ESamplerStateTemplate _template, D3D12_STATIC_SAMPLER_DESC* out_desc)
	{
		if (out_desc == nullptr)
		{
			return;
		}
		if (_template == ESamplerStateTemplate::Unknown)
		{
			JG_CORE_WARN("Unknown Sampler Template");
		}
		switch (_template)
		{
		case ESamplerStateTemplate::Point_Wrap:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
			break;
		case ESamplerStateTemplate::Point_Clamp:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			break;
		case ESamplerStateTemplate::Point_Border:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER);
			break;
		case ESamplerStateTemplate::Point_Mirror:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
			break;
		case ESamplerStateTemplate::Point_MirrorOnce:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_POINT,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
			break;
		case ESamplerStateTemplate::Linear_Wrap:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
			break;
		case ESamplerStateTemplate::Linear_Clamp:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			break;
		case ESamplerStateTemplate::Linear_Border:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER);
			break;
		case ESamplerStateTemplate::Linear_Mirror:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
			break;
		case ESamplerStateTemplate::Linear_MirrorOnce:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_MIN_MAG_MIP_LINEAR,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
			break;
		case ESamplerStateTemplate::Anisotropic_Wrap:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
			break;
		case ESamplerStateTemplate::Anisotropic_Clamp:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
			break;
		case ESamplerStateTemplate::Anisotropic_Border:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER);
			break;
		case ESamplerStateTemplate::Anisotropic_Mirror:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
			break;
		case ESamplerStateTemplate::Anisotropic_MirrorOnce:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0,
				D3D12_FILTER_ANISOTROPIC,
				D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE, D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
			break;
		default:
			*out_desc = CD3DX12_STATIC_SAMPLER_DESC(0);
			break;
		}
	}
	D3D12_FILTER ShaderDataForm::GetSamplerStateFilter(const String& Min, const String& Mag, const String& Mip)
	{
		enum
		{
			Point = 0,
			Linear = 1,
			Anisotropic = 2
		};
		i32 min = Point; i32 mag = Point; i32 mip = Point;


		if (Min == HLSL::Token::SamplerSatateFilter::Point) min = Point;
		else if (Min == HLSL::Token::SamplerSatateFilter::Linear) min = Linear;
		else if (Min == HLSL::Token::SamplerSatateFilter::Anisotropic) min = Anisotropic;

		if (Mag == HLSL::Token::SamplerSatateFilter::Point) mag = Point;
		else if (Mag == HLSL::Token::SamplerSatateFilter::Linear) mag = Linear;
		else if (Mag == HLSL::Token::SamplerSatateFilter::Anisotropic) mag = Anisotropic;

		if (Mip == HLSL::Token::SamplerSatateFilter::Point) mip = Point;
		else if (Mip == HLSL::Token::SamplerSatateFilter::Linear) mip = Linear;
		else if (Mip == HLSL::Token::SamplerSatateFilter::Anisotropic) mip = Anisotropic;





		if (min == Point && mag == Point && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
		else if (min == Point && mag == Point && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		else if (min == Point && mag == Linear && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		else if (min == Linear && mag == Point && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		else if (min == Point && mag == Linear && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		else if (min == Linear && mag == Point && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		else if (min == Linear && mag == Linear && mip == Point) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		else if (min == Linear && mag == Linear && mip == Linear) return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		else if (min == Anisotropic || mag == Anisotropic || mip == Anisotropic) return D3D12_FILTER::D3D12_FILTER_ANISOTROPIC;
		else return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
	D3D12_TEXTURE_ADDRESS_MODE ShaderDataForm::GetTextureAddressMode(const String& addressMode)
	{
		if (addressMode == HLSL::Token::SamplerSatateAddressMode::Wrap) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::Mirror) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::Clamp) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::Border) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		else if (addressMode == HLSL::Token::SamplerSatateAddressMode::MirrorOnce) return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		else return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
	D3D12_COMPARISON_FUNC ShaderDataForm::GetComparisonFunc(const String& comparisonFunc)
	{
		if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Never) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Less) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Equal) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::LessEqual) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Greater) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::NotEqual) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::GreaterEqual) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		else if (comparisonFunc == HLSL::Token::SamplerStateComparisonFunc::Always) return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
		else return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
	}
	D3D12_STATIC_BORDER_COLOR ShaderDataForm::GetBorderColor(const String& borderColor)
	{
		// BorderColor = TransparentBlack, OpaqueBlack, OpaqueWhite
		if (borderColor == HLSL::Token::SamplerStateBorderColor::TransparentBlack) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		else if (borderColor == HLSL::Token::SamplerStateBorderColor::OpaqueBlack) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		else if (borderColor == HLSL::Token::SamplerStateBorderColor::OpaqueWhite) return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		else return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	}

	bool ShaderDataForm::RegisterStruct(const String& name)
	{
		if (StructDataMap.find(name) != StructDataMap.end())
		{
			JG_CORE_ERROR("{0} Struct Already Exists.", name);
			return false;
		}
		StructDataMap[name] = CreateUniquePtr<StructData>();
		return true;
	}

	bool ShaderDataForm::RegisterStructVar(StructData* structData, const String& varCode)
	{
		u64 varStartPos = varCode.find_first_not_of(" ", varCode.find_first_not_of("\t"));
		u64 varMidPos = varCode.find(" ", varStartPos);
		u64 varEndPos = varCode.find(";", varMidPos) - 1;




		String typeCode = varCode.substr(varStartPos, varMidPos - varStartPos);
		typeCode = ReplaceAll(typeCode, " ", "");
		String nameCode = varCode.substr(varMidPos + 1, varEndPos - varMidPos);
		nameCode = ReplaceAll(nameCode, " ", "");
		nameCode = ReplaceAll(nameCode, ";", "");

		u64 varSize = 0;
		if (FindTypeInfo(typeCode, nullptr, &varSize) == false)
		{
			return false;
		}

		structData->DataNameList.push_back(nameCode);
		structData->DataTypeList.push_back(typeCode);
		structData->DataSize += varSize;
		return true;
	}

	bool ShaderDataForm::RegisterStructuredBuffer(const String& name)
	{

		if (StructuredBufferDataMap.find(name) != StructuredBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} StructuredBuffer Already Exists.", name);
			return false;
		}
		StructuredBufferDataMap[name] = CreateUniquePtr<StructuredBufferData>();

		return true;
	}
	bool ShaderDataForm::RegisterRWStructuredBuffer(const String& name)
	{
		if (RWStructuredBufferDataMap.find(name) != RWStructuredBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} RWStructuredBuffer Already Exists.", name);
			return false;
		}
		RWStructuredBufferDataMap[name] = CreateUniquePtr<StructuredBufferData>();

		return true;
	}
	bool ShaderDataForm::RegisterCBuffer(const String& name)
	{

		if (CBufferDataMap.find(name) != CBufferDataMap.end())
		{
			JG_CORE_ERROR("{0} CBuffer Already Exists.", name);
			return false;
		}

		CBufferDataMap[name] = CreateUniquePtr<CBufferData>();
		return true;
	}

	bool ShaderDataForm::RegisterTextureData(const String& name)
	{
		if (TextureDataMap.find(name) != TextureDataMap.end())
		{
			JG_CORE_ERROR("{0} TextureData Already Exists.", name);
			return false;
		}

		TextureDataMap[name] = CreateUniquePtr<TextureData>();

		return true;
	}

	bool ShaderDataForm::RegisterRWTextureData(const String& name)
	{
		if (RWTextureDataMap.find(name) != RWTextureDataMap.end())
		{
			JG_CORE_ERROR("{0} RWTextureData Already Exists.", name);
			return false;
		}

		RWTextureDataMap[name] = CreateUniquePtr<TextureData>();

		return true;
	}

	bool ShaderDataForm::RegisterCBufferVar(CBufferData* cBuffer, const String& varCode, u64& uploadDataSize)
	{
		u64 varStartPos = varCode.find_first_not_of(" ", varCode.find_first_not_of("\t"));
		u64 varMidPos = varCode.find(" ", varStartPos);
		u64 varEndPos = varCode.find(";", varMidPos) - 1;




		String typeCode = varCode.substr(varStartPos, varMidPos - varStartPos);
		typeCode = ReplaceAll(typeCode, " ", "");
		String nameCode = varCode.substr(varMidPos + 1, varEndPos - varMidPos);
		nameCode = ReplaceAll(nameCode, " ", "");
		nameCode = ReplaceAll(nameCode, ";", "");





		cBuffer->DataMap[nameCode] = CreateUniquePtr<Data>();
		Data& uploadData = *(cBuffer->DataMap[nameCode]);
		uploadData.Type = StringToShaderDataType(typeCode);
		uploadData.DataSize = GetShaderDataTypeSize(uploadData.Type);
		uploadData.DataPos = uploadDataSize;
		uploadData.Owner = cBuffer;
		uploadDataSize += uploadData.DataSize;


		if (CBufferVarMap.find(nameCode) != CBufferVarMap.end())
		{
			JG_CORE_ERROR("{0} CBuffer Var Already Exists.", nameCode);
			return false;
		}
		CBufferVarMap[nameCode] = cBuffer->DataMap[nameCode].get();
		return true;
	}

	bool ShaderDataForm::RegisterSamplerStateData(const String& name)
	{
		if (SamplerStateDataMap.find(name) != SamplerStateDataMap.end())
		{
			JG_CORE_ERROR("{0} SamplerStateData Already Exists.", name);
			return false;
		}

		SamplerStateDataMap[name] = CreateUniquePtr<SamplerStateData>();

		return true;
	}



	ShaderData::ShaderData(SharedPtr<IShader> shader)
	{

		mOwnerShader = shader;
		mUploadAllocator = CreateUniquePtr<UploadAllocator>();
		auto dx12Shader = static_cast<DirectX12Shader*>(mOwnerShader.get());
		if (dx12Shader != nullptr)
		{
			auto shaderDataForm = dx12Shader->GetShaderDataForm();


			for (auto& _pair : shaderDataForm->CBufferDataMap)
			{
				auto alloc = mUploadAllocator->Allocate(_pair.second->DataSize, 256);
				mReadDatas[_pair.first] = alloc;
			}

			for (auto& _pair : shaderDataForm->StructuredBufferDataMap)
			{
				auto alloc = mUploadAllocator->Allocate(_pair.second->ElementDataSize * MaxElementCount, _pair.second->ElementDataSize);
				mReadDatas[_pair.first] = alloc;
			}
			for (auto& _pair : shaderDataForm->RWStructuredBufferDataMap)
			{
				auto alloc = mUploadAllocator->Allocate(_pair.second->ElementDataSize * MaxElementCount, _pair.second->ElementDataSize);
				mReadWriteDatas[_pair.first] = alloc;
			}

			for (auto& _pair : shaderDataForm->TextureDataMap)
			{
				mTextureDatas[_pair.first].resize(_pair.second->TextureCount, nullptr);
			}
			for (auto& _pair : shaderDataForm->RWTextureDataMap)
			{
				mRWTextureDatas[_pair.first].resize(_pair.second->TextureCount, nullptr);
			}

		}
	}
	bool ShaderData::Bind(u64 commandID)
	{
		std::lock_guard<std::shared_mutex> lock(mMutex);
		auto dx12Shader = static_cast<DirectX12Shader*>(mOwnerShader.get());

		if (dx12Shader != nullptr)
		{
			if (dx12Shader->Bind(commandID) == false)
			{
				JG_CORE_ERROR("Failed Bind {0} Shader", dx12Shader->GetName());
				return false;
			}
	
	
			auto shaderDataForm = dx12Shader->GetShaderDataForm();

			if (dx12Shader->GetFlags() & EShaderFlags::Allow_ComputeShader)
			{
				auto commandList = DirectX12API::GetComputeCommandList(commandID);
				for (auto& _pair : shaderDataForm->CBufferDataMap)
				{
					auto cBufferName = _pair.first;
					auto cBufferData = _pair.second.get();
					commandList->BindConstantBuffer(cBufferData->RootParm, mReadDatas[cBufferName]);
				}
				// structuredBuffer
				for (auto& _pair : shaderDataForm->StructuredBufferDataMap)
				{
					auto structuredBufferName = _pair.first;
					auto structuredBufferData = _pair.second.get();
					commandList->BindStructuredBuffer(structuredBufferData->RootParm, mReadDatas[structuredBufferName]);
				}

				// RWStructuredBuffer
				for (auto& _pair : shaderDataForm->RWStructuredBufferDataMap)
				{
					auto structuredBufferName = _pair.first;
					auto structuredBufferData = _pair.second.get();
					commandList->BindStructuredBuffer(structuredBufferData->RootParm, mReadWriteDatas[structuredBufferName]);
				}
				for (auto& _pair : shaderDataForm->TextureDataMap)
				{
					auto  textureData = _pair.second.get();
					auto& textureList = mTextureDatas[_pair.first];
					u64 textureCount = textureList.size();
					List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;


					for (u64 i = 0; i < textureCount; ++i)
					{
						if (textureList[i] != nullptr && textureList[i]->IsValid())
						{
							handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetSRV());
						}
					}


					if (handles.empty() == false)
					{
						commandList->BindTextures((u32)textureData->RootParm, handles);
					}

				}

				// RWTexture
				for (auto& _pair : shaderDataForm->RWTextureDataMap)
				{
					auto textureData = _pair.second.get();
					auto& textureList = mTextureDatas[_pair.first];
					u64 textureCount = textureList.size();
					List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
					for (u64 i = 0; i < textureCount; ++i)
					{
						if (textureList[i] != nullptr && textureList[i]->IsValid())
						{
							handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetUAV());
						}
					}

					if (handles.empty() == false)
					{
						commandList->BindTextures((u32)textureData->RootParm, handles);
					}
				}
			}
			else
			{
				auto commandList = DirectX12API::GetGraphicsCommandList(commandID);
				for (auto& _pair : shaderDataForm->CBufferDataMap)
				{
					auto cBufferName = _pair.first;
					auto cBufferData = _pair.second.get();
					commandList->BindConstantBuffer(cBufferData->RootParm, mReadDatas[cBufferName]);
				}
				// structuredBuffer
				for (auto& _pair : shaderDataForm->StructuredBufferDataMap)
				{
					auto structuredBufferName = _pair.first;
					auto structuredBufferData = _pair.second.get();
					commandList->BindStructuredBuffer(structuredBufferData->RootParm, mReadDatas[structuredBufferName]);
				}

				// RWStructuredBuffer
				for (auto& _pair : shaderDataForm->RWStructuredBufferDataMap)
				{
					auto structuredBufferName = _pair.first;
					auto structuredBufferData = _pair.second.get();
					commandList->BindStructuredBuffer(structuredBufferData->RootParm, mReadWriteDatas[structuredBufferName]);
				}
				for (auto& _pair : shaderDataForm->TextureDataMap)
				{
					auto  textureData = _pair.second.get();
					auto& textureList = mTextureDatas[_pair.first];
					u64 textureCount = textureList.size();
					List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;


					for (u64 i = 0; i < textureCount; ++i)
					{
						if (textureList[i] != nullptr && textureList[i]->IsValid())
						{
							handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetSRV());
						}
					}
					if (handles.empty() == false)
					{
						commandList->BindTextures((u32)textureData->RootParm, handles);
					}
				}

				// RWTexture
				for (auto& _pair : shaderDataForm->RWTextureDataMap)
				{
					auto textureData = _pair.second.get();
					auto& textureList = mTextureDatas[_pair.first];
					u64 textureCount = textureList.size();
					List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
					for (u64 i = 0; i < textureCount; ++i)
					{
						if (textureList[i] != nullptr && textureList[i]->IsValid())
						{
							handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetUAV());
						}
					}

					if (handles.empty() == false)
					{
						commandList->BindTextures((u32)textureData->RootParm, handles);
					}
				}
			}
		}
		else
		{
			return false;
		}
		return true;
	}
	void ShaderData::Reset()
	{
		mUploadAllocator->Reset();
		mReadDatas.clear();
		mReadWriteDatas.clear();
		mTextureDatas.clear();
		mRWTextureDatas.clear();
		mOwnerShader.reset();
	}
	bool ShaderData::SetFloat(const String& name, float value)
	{
		return SetData<float, EShaderDataType::_float>(name, &value);
	}
	bool ShaderData::SetFloat2(const String& name, const JVector2& value)
	{
		return SetData<JVector2, EShaderDataType::_float2>(name, &value);
	}
	bool ShaderData::SetFloat3(const String& name, const JVector3& value)
	{
		return SetData<JVector3, EShaderDataType::_float3>(name, &value);
	}
	bool ShaderData::SetFloat4(const String& name, const JVector4& value)
	{
		return SetData<JVector4, EShaderDataType::_float4>(name, &value);
	}
	bool ShaderData::SetInt(const String& name, i32 value)
	{
		return SetData<i32, EShaderDataType::_int>(name, &value);
	}
	bool ShaderData::SetInt2(const String& name, const JVector2Int& value)
	{
		return SetData<JVector2Int, EShaderDataType::_int2>(name, &value);
	}
	bool ShaderData::SetInt3(const String& name, const JVector3Int& value)
	{
		return SetData<JVector3Int, EShaderDataType::_int3>(name, &value);
	}
	bool ShaderData::SetInt4(const String& name, const JVector4Int& value)
	{
		return SetData<JVector4Int, EShaderDataType::_int4>(name, &value);
	}
	bool ShaderData::SetUint(const String& name, u32 value)
	{
		return SetData<u32, EShaderDataType::_uint>(name, &value);
	}
	bool ShaderData::SetUint2(const String& name, const JVector2Uint& value)
	{
		return SetData<JVector2Uint, EShaderDataType::_uint2>(name, &value);
	}
	bool ShaderData::SetUint3(const String& name, const JVector3Uint& value)
	{
		return SetData<JVector3Uint, EShaderDataType::_uint3>(name, &value);
	}
	bool ShaderData::SetUint4(const String& name, const JVector4Uint& value)
	{
		return SetData<JVector4Uint, EShaderDataType::_uint4>(name, &value);
	}
	bool ShaderData::SetFloat4x4(const String& name, const JMatrix& value)
	{
		return SetData<JMatrix, EShaderDataType::_float4x4>(name, &value);
	}

	bool ShaderData::SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture)
	{
		if (mTextureDatas.find(name) == mTextureDatas.end())
		{
			return false;
		}

		auto& textureList = mTextureDatas[name];
		u64 textureCount = textureList.size();

		if (textureCount <= textureSlot)
		{
			return false;
		}

		textureList[textureSlot] = texture;
		return true;
	}



	bool ShaderData::SetFloatArray(const String& name, const List<float>& value)
	{
		return SetDataArray<float, EShaderDataType::_float>(name, value);
	}

	bool ShaderData::SetFloat2Array(const String& name, const List<JVector2>& value)
	{
		return SetDataArray<JVector2, EShaderDataType::_float2>(name, value);
	}

	bool ShaderData::SetFloat3Array(const String& name, const List<JVector3>& value)
	{
		return SetDataArray<JVector3, EShaderDataType::_float3>(name, value);
	}

	bool ShaderData::SetFloat4Array(const String& name, const List<JVector4>& value)
	{
		return SetDataArray<JVector4, EShaderDataType::_float4>(name, value);
	}

	bool ShaderData::SetIntArray(const String& name, const List<i32>& value)
	{
		return SetDataArray<i32, EShaderDataType::_int>(name, value);
	}

	bool ShaderData::SetInt2Array(const String& name, const List<JVector2Int>& value)
	{
		return SetDataArray<JVector2Int, EShaderDataType::_int2>(name, value);
	}

	bool ShaderData::SetInt3Array(const String& name, const List<JVector3Int>& value)
	{
		return SetDataArray<JVector3Int, EShaderDataType::_int3>(name, value);
	}

	bool ShaderData::SetInt4Array(const String& name, const List<JVector4Int>& value)
	{
		return SetDataArray<JVector4Int, EShaderDataType::_int4>(name, value);
	}

	bool ShaderData::SetUintArray(const String& name, const List<u32>& value)
	{
		return SetDataArray<u32, EShaderDataType::_uint>(name, value);
	}

	bool ShaderData::SetUint2Array(const String& name, const List<JVector2Uint>& value)
	{
		return SetDataArray<JVector2Uint, EShaderDataType::_uint2>(name, value);
	}

	bool ShaderData::SetUint3Array(const String& name, const List<JVector3Uint>& value)
	{
		return SetDataArray<JVector3Uint, EShaderDataType::_uint3>(name, value);
	}

	bool ShaderData::SetUint4Array(const String& name, const List<JVector4Uint>& value)
	{
		return SetDataArray<JVector4Uint, EShaderDataType::_uint4>(name, value);
	}

	bool ShaderData::SetFloat4x4Array(const String& name, const List<JMatrix>& value)
	{
		return SetDataArray<JMatrix, EShaderDataType::_float4x4>(name, value);
	}

	bool ShaderData::SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize)
	{
		return SetDataArray(name, datas, elementCount, elementSize);
	}



	bool ShaderData::GetFloat(const String& name, float* out_value)
	{
		return GetData<float, EShaderDataType::_float>(name, out_value);
	}

	bool ShaderData::GetFloat2(const String& name, JVector2* out_value)
	{
		return GetData<JVector2, EShaderDataType::_float2>(name, out_value);
	}

	bool ShaderData::GetFloat3(const String& name, JVector3* out_value)
	{
		return GetData<JVector3, EShaderDataType::_float3>(name, out_value);
	}

	bool ShaderData::GetFloat4(const String& name, JVector4* out_value)
	{
		return GetData<JVector4, EShaderDataType::_float4>(name, out_value);
	}

	bool ShaderData::GetInt(const String& name, i32* out_value)
	{
		return GetData<i32, EShaderDataType::_int>(name, out_value);
	}

	bool ShaderData::GetInt2(const String& name, JVector2Int* out_value)
	{
		return GetData<JVector2Int, EShaderDataType::_int2>(name, out_value);
	}

	bool ShaderData::GetInt3(const String& name, JVector3Int* out_value)
	{
		return GetData<JVector3Int, EShaderDataType::_int3>(name, out_value);
	}

	bool ShaderData::GetInt4(const String& name, JVector4Int* out_value)
	{
		return GetData<JVector4Int, EShaderDataType::_int4>(name, out_value);
	}

	bool ShaderData::GetUint(const String& name, u32* out_value)
	{
		return GetData<u32, EShaderDataType::_uint>(name, out_value);
	}

	bool ShaderData::GetUint2(const String& name, JVector2Uint* out_value)
	{
		return GetData<JVector2Uint, EShaderDataType::_uint2>(name, out_value);
	}

	bool ShaderData::GetUint3(const String& name, JVector3Uint* out_value)
	{
		return GetData<JVector3Uint, EShaderDataType::_uint3>(name, out_value);
	}

	bool ShaderData::GetUint4(const String& name, JVector4Uint* out_value)
	{
		return GetData<JVector4Uint, EShaderDataType::_uint4>(name, out_value);
	}

	bool ShaderData::GetFloat4x4(const String& name, JMatrix* outValue)
	{
		return GetData<JMatrix, EShaderDataType::_float4x4>(name, outValue);
	}

	bool ShaderData::GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value)
	{
		if (mTextureDatas.find(name) == mTextureDatas.end())
		{
			return false;
		}

		auto& textureList = mTextureDatas[name];
		u64 textureCount = textureList.size();
		if (textureCount <= textureSlot)
		{
			return false;
		}
		*out_value = textureList[textureSlot];
		return true;
	}
	UploadAllocator::Allocation ShaderData::GetRWData(const String& name)
	{
		if (mReadWriteDatas.find(name) != mReadWriteDatas.end())
		{
			return mReadWriteDatas[name];
		}
		return UploadAllocator::Allocation();
	}
	DirectX12Shader* ShaderData::GetOwnerShader() const
	{
		if (mOwnerShader == nullptr)
		{
			return nullptr;
		}
		auto dx12Shader = static_cast<DirectX12Shader*>(mOwnerShader.get());
		return dx12Shader;
	}
	ShaderDataForm::Data* ShaderData::GetAndCheckData(const String& name, EShaderDataType checkType)
	{
		auto dx12Shader = static_cast<DirectX12Shader*>(mOwnerShader.get());
		if (dx12Shader != nullptr)
		{
			auto shaderDataForm = dx12Shader->GetShaderDataForm();
			auto& CBufferVarMap = shaderDataForm->CBufferVarMap;

			auto iter = CBufferVarMap.find(name);
			if (iter == CBufferVarMap.end())
			{
				return nullptr;
			}
			auto data = iter->second;
			if (data->Type != checkType)
			{
				return nullptr;
			}
			return data;
		}
		else
		{
			return nullptr;
		}
	}
	bool ShaderData::CheckDataArray(const String& name, EShaderDataType checkType)
	{
		auto dx12Shader = static_cast<DirectX12Shader*>(mOwnerShader.get());
		if (dx12Shader != nullptr)
		{
			auto shaderDataForm = dx12Shader->GetShaderDataForm();
			auto iter = shaderDataForm->StructuredBufferDataMap.find(name);
			if (iter == shaderDataForm->StructuredBufferDataMap.end())
			{
				return false;
			}

			auto data = iter->second.get();
			if (data->Type != ShaderDataTypeToString(checkType))
			{
				return false;
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	bool ShaderData::CheckDataArray(const String& name, u64 elementSize)
	{
		auto dx12Shader = static_cast<DirectX12Shader*>(mOwnerShader.get());
		if (dx12Shader != nullptr)
		{
			auto shaderDataForm = dx12Shader->GetShaderDataForm();
			auto iter = shaderDataForm->StructuredBufferDataMap.find(name);
			if (iter == shaderDataForm->StructuredBufferDataMap.end())
			{
				return false;
			}
			u64 typeSize = 0;
			auto data = iter->second.get();
			if (shaderDataForm->FindTypeInfo(data->Type, nullptr, &typeSize) == false)
			{
				return false;
			}
			if (elementSize != typeSize)
			{
				return false;
			}

			return true;
		}
		else
		{
			return false;
		}


		return false;
	}
}