#include "pch.h"
#include "ShaderDataForm.h"
#include "CommandList.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/DirectX12Shader.h"
#include "Platform/Graphics/DirectX12/DirectX12Resource.h"
#include "Platform/Graphics/DirectX12/Utill/RootSignature.h"

namespace JG
{
	bool ShaderDataForm::Set(String& code)
	{
		// Shader 코드 생성 및 정보 수집
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
		while (pos != String::npos)
		{
			pos = AnalysisRWTexture2D(code, pos, &result);
		}
		return result;
	}
	void ShaderDataForm::Reset()
	{
		CBOffset = 0;
		SBOffset = 0;
		RWSBOffset = 0;
		TexRegisterNumOffset = 0;
		RWTexRegisterNumOffset = 0;
		CBufferVarMap.clear();
		CBufferDataMap.clear();
		StructuredBufferDataMap.clear();
		RWStructuredBufferDataMap.clear();

		TextureDataMap.clear();
		SortedTextureDataMap.clear();
		RWTextureDataMap.clear();
		SortedRWTextureDataMap.clear();
		StructDataMap.clear();
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
		if (MAX_CB_COUNT <= CBOffset)
		{
			if (result)
			{
				*result = false;
			}
			return String::npos;
		}


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

		cBuffer->DataSize    = uploadDataSize;
		cBuffer->ElementType = HLSL::EHLSLElement::CBuffer;
		cBuffer->RootParm    = (u64)ShaderDefine::EComputeRootParam::CB_0 + CBOffset;
		cBuffer->RegisterNum = CBOffset;

		code.insert(endPos, " : register(b" + std::to_string(cBuffer->RegisterNum) + ")");
		CBOffset += 1;
		return startPos;
	}
	u64 ShaderDataForm::AnalysisStructuredBuffer(String& code, u64 startPos, bool* result)
	{
		if (MAX_SB_COUNT <= SBOffset)
		{
			if (result)
			{
				*result = false;
			}
			return String::npos;
		}

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
			typeCode = StringExtend::ReplaceAll(typeCode, " ", "");

		
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
			nameCode = StringExtend::ReplaceAll(nameCode, " ", "");

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
			structuredBufferData->RootParm    = (u64)ShaderDefine::EComputeRootParam::STRUCTUREDBUFFER_0 + SBOffset;
			structuredBufferData->RegisterNum = 0;
			structuredBufferData->RegisterSpace = HLSL::RegisterSpace::StructuredBufferRegisterSpace + SBOffset;
			structuredBufferData->ElementType = HLSL::EHLSLElement::StructuredBuffer;
			structuredBufferData->ElementDataSize = typeSize;
	

			code.insert(endPos, " : register(t0, space" + std::to_string(structuredBufferData->RegisterSpace) + ")");
			startPos = endPos + 1;

			SBOffset += 1;
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

			String nameCode = StringExtend::ReplaceAll(dataCode, HLSL::Token::Texture2D, "");
			u64 labPos = nameCode.find("<"); u64 rabPos = nameCode.find(">");
			if (labPos != String::npos && rabPos != String::npos)
			{
				String dataType = nameCode.substr(labPos, rabPos - labPos + 1);
				nameCode = StringExtend::ReplaceAll(nameCode, dataType, "");
			}

			nameCode = StringExtend::ReplaceAll(nameCode, " ", "");
			





			u64 arraySize = 1;

			u64 arrayStartPos = dataCode.find("[");
			if (arrayStartPos != String::npos)
			{

				nameCode = nameCode.substr(0, nameCode.find("["));



				arrayStartPos += 1;
				u64 arrayEndPos = dataCode.find("]", arrayStartPos);


				String arraySizeCode = dataCode.substr(arrayStartPos, arrayEndPos - arrayStartPos);
				arraySizeCode = StringExtend::ReplaceAll(arraySizeCode, " ", "");

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
			textureData->RootParm = (u64)ShaderDefine::EComputeRootParam::TEXTURE2D;
			textureData->RegisterNum   = TexRegisterNumOffset;
			textureData->RegisterSpace = HLSL::RegisterSpace::Texture2DRegisterSpace;
			textureData->ElementType   = HLSL::EHLSLElement::Texture;
			textureData->Type	       = HLSL::EHLSLTextureType::_2D;
			textureData->TextureCount  = arraySize;
			code.insert(endPos,
				" : register(t" + std::to_string(TextureDataMap[nameCode]->RegisterNum) +
				", space" + std::to_string(TextureDataMap[nameCode]->RegisterSpace) + ")");
			startPos = endPos + 1;

			SortedTextureDataMap[textureData->RegisterNum] = textureData;
			TexRegisterNumOffset += arraySize;
		}
		else
		{
			return String::npos;
		}
		return startPos;
	}
	u64 ShaderDataForm::AnalysisRWStructuredBuffer(String& code, u64 startPos, bool* result)
	{
		if (MAS_RWSB_COUNT <= RWSBOffset)
		{
			if (result)
			{
				*result = false;
			}
			return String::npos;
		}


		u64 dataTokenStartPos = code.find(HLSL::Token::RWStructuredBuffer, startPos);
		if (dataTokenStartPos != String::npos)
		{
			u64 endPos = code.find(";", dataTokenStartPos);

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);


			// Type
			u64 dataTypeStartPos = dataCode.find("<") + 1;
			u64 dataTypeEndPos = dataCode.find(">");



			String typeCode = dataCode.substr(dataTypeStartPos, dataTypeEndPos - dataTypeStartPos);
			typeCode = StringExtend::ReplaceAll(typeCode, " ", "");

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
			nameCode = StringExtend::ReplaceAll(nameCode, " ", "");

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
			structuredBufferData->RootParm = (u64)ShaderDefine::EComputeRootParam::RWSTRUCTUREDBUFFER_0 + RWSBOffset;
			structuredBufferData->RegisterNum = 0;
			structuredBufferData->RegisterSpace = HLSL::RegisterSpace::RWStructuredBufferRegisterSpace + RWSBOffset;
			structuredBufferData->ElementType   = HLSL::EHLSLElement::RWStructuredBuffer;
			structuredBufferData->ElementDataSize = typeSize;

			code.insert(endPos, " : register(u0, space" + std::to_string(structuredBufferData->RegisterSpace) + ")");
			startPos = endPos + 1;

			RWSBOffset += 1;
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

			dataTokenStartPos = code.find(">", dataTokenStartPos) + 1;

			String dataCode = code.substr(dataTokenStartPos, endPos - dataTokenStartPos);
			String nameCode = StringExtend::ReplaceAll(dataCode, " ", "");

			u64 arraySize = 1;

			u64 arrayStartPos = dataCode.find("[");
			if (arrayStartPos != String::npos)
			{

				nameCode = nameCode.substr(0, nameCode.find("["));



				arrayStartPos += 1;
				u64 arrayEndPos = dataCode.find("]", arrayStartPos);


				String arraySizeCode = dataCode.substr(arrayStartPos, arrayEndPos - arrayStartPos);
				arraySizeCode = StringExtend::ReplaceAll(arraySizeCode, " ", "");

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
			textureData->RootParm      = (u64)ShaderDefine::EComputeRootParam::RWTEXTURE2D;
			textureData->RegisterNum   = RWTexRegisterNumOffset;
			textureData->RegisterSpace = HLSL::RegisterSpace::RWTexture2DRegisterSpace;
			textureData->ElementType   = HLSL::EHLSLElement::RWTexture;
			textureData->Type = HLSL::EHLSLTextureType::_2D;
			textureData->TextureCount = arraySize;

			code.insert(endPos,
				" : register(u" + std::to_string(RWTextureDataMap[nameCode]->RegisterNum) +
				", space" + std::to_string(RWTextureDataMap[nameCode]->RegisterSpace) + ")");
			startPos = endPos + 1;

			SortedRWTextureDataMap[textureData->RegisterNum] = textureData;
			RWTexRegisterNumOffset += arraySize;
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
		structName = StringExtend::ReplaceAll(structName, "\n", "");
		structName = StringExtend::ReplaceAll(structName, "\t", "");
		structName = StringExtend::ReplaceAll(structName, " ", "");

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
		cbName = StringExtend::ReplaceAll(cbName, "\n", "");
		cbName = StringExtend::ReplaceAll(cbName, "\t", "");
		cbName = StringExtend::ReplaceAll(cbName, " ", "");

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
				varCode = StringExtend::ReplaceAll(varCode, "\n", "");
				varCode = StringExtend::ReplaceAll(varCode, "\t", "");

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
		dataCode = StringExtend::ReplaceAll(dataCode, "\t", "");
		dataCode = StringExtend::ReplaceAll(dataCode, "\n", "");


		startPos = 0;
		u64 midPos = dataCode.find("=");
		endPos = dataCode.length();

		if (out_key != nullptr)
		{
			*out_key = dataCode.substr(startPos, midPos - startPos);
			*out_key = StringExtend::ReplaceAll(*out_key, " ", "");
		}

		if (out_value != nullptr)
		{
			*out_value = dataCode.substr(midPos + 1, endPos - midPos - 1);
			*out_value = StringExtend::ReplaceAll(*out_value, " ", "");
		}


		return result;
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
		typeCode = StringExtend::ReplaceAll(typeCode, " ", "");
		String nameCode = varCode.substr(varMidPos + 1, varEndPos - varMidPos);
		nameCode = StringExtend::ReplaceAll(nameCode, " ", "");
		nameCode = StringExtend::ReplaceAll(nameCode, ";", "");

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
		typeCode = StringExtend::ReplaceAll(typeCode, " ", "");
		String nameCode = varCode.substr(varMidPos + 1, varEndPos - varMidPos);
		nameCode = StringExtend::ReplaceAll(nameCode, " ", "");
		nameCode = StringExtend::ReplaceAll(nameCode, ";", "");





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
	ShaderData::ShaderData(SharedPtr<ShaderDataForm> shaderDataForm)
	{
		mShaderDataForm   = shaderDataForm;
		mUploadAllocator  = CreateUniquePtr<UploadAllocator>();
		for (auto& _pair : shaderDataForm->CBufferDataMap)
		{
			mReadDatas[_pair.first].resize(_pair.second->DataSize);
		}

		for (auto& _pair : shaderDataForm->StructuredBufferDataMap)
		{
			mReadDatas[_pair.first].resize(_pair.second->ElementDataSize);
		}
		for (auto& _pair : shaderDataForm->RWStructuredBufferDataMap)
		{
			u64 alignment = _pair.second->ElementDataSize;
			u64 alignDataSize = (MaxDataSize + (alignment - 1)) & ~(alignment - 1);
			mReadWriteDatas[_pair.first] = IReadWriteBuffer::Create(_pair.first, alignDataSize);
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
	void ShaderData::ForEach_CB(const std::function<void(const ShaderDataForm::CBufferData*, const List<jbyte>&)>& action)
	{
		if (action == nullptr) return;
		for (auto& _pair : mShaderDataForm->CBufferDataMap)
		{
			auto cBufferName = _pair.first;
			auto cBufferData = _pair.second.get();
			action(cBufferData, mReadDatas[cBufferName]);
		}
	}
	void ShaderData::ForEach_SB(const std::function<void(const ShaderDataForm::StructuredBufferData*, const List<jbyte>&)>& action)
	{
		if (action == nullptr) return;
		for (auto& _pair : mShaderDataForm->StructuredBufferDataMap)
		{
			auto structuredBufferName = _pair.first;
			auto structuredBufferData = _pair.second.get();
			action(structuredBufferData, mReadDatas[structuredBufferName]);
		}
	}
	void ShaderData::ForEach_RWSB(const std::function<void(const ShaderDataForm::StructuredBufferData*, SharedPtr<IReadWriteBuffer>)>& action)
	{
		if (action == nullptr) return;
		for (auto& _pair : mShaderDataForm->RWStructuredBufferDataMap)
		{
			auto structuredBufferName = _pair.first;
			auto structuredBufferData = _pair.second.get();
			action(structuredBufferData, mReadWriteDatas[structuredBufferName]);
		}
	}
	void ShaderData::ForEach_Tex(const std::function<void(const ShaderDataForm::TextureData*, const List<SharedPtr<ITexture>>&)>& action)
	{
		if (action == nullptr) return;


		Dictionary<u64, std::pair<String, ShaderDataForm::TextureData*>> dic;

		for (auto& _pair : mShaderDataForm->SortedTextureDataMap)
		{
			auto  textureData = _pair.second;
			action(textureData, mTextureDatas[textureData->Name]);
		}
	}
	void ShaderData::ForEach_RWTex(const std::function<void(const ShaderDataForm::TextureData*, const List<SharedPtr<ITexture>>&)>& action)
	{
		if (action == nullptr) return;
		for (auto& _pair : mShaderDataForm->SortedRWTextureDataMap)
		{
			auto  textureData = _pair.second;
			action(textureData, mRWTextureDatas[textureData->Name]);
		}
	}
	void ShaderData::Reset()
	{
		mUploadAllocator->Reset();
		mReadDatas.clear();
		mReadWriteDatas.clear();
		mTextureDatas.clear();
		mRWTextureDatas.clear();
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
		if (mTextureDatas.find(name) != mTextureDatas.end())
		{
			auto& textureList = mTextureDatas[name];
			u64 textureCount = textureList.size();

			if (textureCount <= textureSlot)
			{
				return false;
			}

			textureList[textureSlot] = texture;
			return true;
		}
		if (mRWTextureDatas.find(name) != mRWTextureDatas.end())
		{

			auto& textureList = mRWTextureDatas[name];
			u64 textureCount = textureList.size();

			if (textureCount <= textureSlot)
			{
				return false;
			}

			textureList[textureSlot] = texture;
			return true;
		}

		return false;
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

	bool ShaderData::SetStructDataArray(const String& name, const void* datas, u64 elementCount, u64 elementSize)
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
	SharedPtr<IReadWriteBuffer> ShaderData::GetRWData(const String& name)
	{
		if (mReadWriteDatas.find(name) != mReadWriteDatas.end())
		{
			return mReadWriteDatas[name];
		}
		return nullptr;
	}
	ShaderDataForm::Data* ShaderData::GetAndCheckData(const String& name, EShaderDataType checkType)
	{
		auto& CBufferVarMap = mShaderDataForm->CBufferVarMap;

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
	bool ShaderData::CheckDataArray(const String& name, EShaderDataType checkType)
	{
		auto iter = mShaderDataForm->StructuredBufferDataMap.find(name);
		if (iter == mShaderDataForm->StructuredBufferDataMap.end())
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
	bool ShaderData::CheckDataArray(const String& name, u64 elementSize)
	{
		auto iter = mShaderDataForm->StructuredBufferDataMap.find(name);
		if (iter == mShaderDataForm->StructuredBufferDataMap.end())
		{
			return false;
		}
		u64 typeSize = 0;
		auto data = iter->second.get();
		if (mShaderDataForm->FindTypeInfo(data->Type, nullptr, &typeSize) == false)
		{
			return false;
		}
		if (elementSize != typeSize)
		{
			return false;
		}

		return true;
	}
}