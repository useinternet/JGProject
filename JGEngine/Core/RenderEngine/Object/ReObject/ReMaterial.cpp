#include "pch.h"
#include "ReMaterial.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/DxObject/PipelineState.h"
#include "Object/DxObject/TextureManager.h"
#include "Object/Shader/ShaderModuleManager.h"
#include "Object/DxObject/Shader.h"
#include "Object/Shader/ShaderData.h"
#include "RenderItem.h"
using namespace std;

namespace RE
{
	ReMaterial::ReMaterial(const std::string& name, ERenderItemUsage usage) :
		ReObject(name), m_Usage(usage)
	{ 
		m_CompiledPixelShader = make_shared<PixelShader>();
		m_PSO = make_shared<GraphicsPipelineState>();
		m_MaterialCB = make_shared<SBDConstantBuffer>(FixedGShaderModule::MatCBName());
	}
	void ReMaterial::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		m_PSO->SetBlendState(desc);
	}
	void ReMaterial::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		m_PSO->SetDepthStencilState(desc);
	}
	void ReMaterial::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		m_PSO->SetRasterizerState(desc);
	}
	bool ReMaterial::AddTexture(const std::string& name, const std::string& asset_texture_path)
	{
		if (m_TextureIndexMap.find(name) != m_TextureIndexMap.end())
			return false;

		Texture t;
		t = TextureManager::GetTexture(asset_texture_path);
		if (!t.IsVaild())
			return false;


		uint32_t index = (uint32_t)m_BindedTextures.size();
		m_BindedTextures.push_back(t);
		m_TextureIndexMap[name] = index;


		//
		for (auto& controller_pair : m_MatControllerPool)
		{
			controller_pair.second->m_BindedTextures.push_back(t);
		}


		return true;
	}
	bool ReMaterial::AddTexture(const std::string& name, const Texture& t)
	{
		if (m_TextureIndexMap.find(name) != m_TextureIndexMap.end())
			return false;

		uint32_t index = (uint32_t)m_BindedTextures.size();
		m_BindedTextures.push_back(t);
		m_TextureIndexMap[name] = index;

		for (auto& controller_pair : m_MatControllerPool)
		{
			controller_pair.second->m_BindedTextures.push_back(t);
		}


		return true;
	}
	bool ReMaterial::SetTexture(uint32_t index, const std::string& asset_texture_path)
	{
		auto t = TextureManager::GetTexture(asset_texture_path);
		if (!t.IsVaild())
			return false;
		if (index >= m_BindedTextures.size())
			return false;
		m_BindedTextures[index] = t;
		return true;
	}
	bool ReMaterial::SetTexture(const std::string& name, const std::string& asset_texture_path)
	{
		uint32_t index = GetBindedTextureIndex(name);
		if (index == -1)
			return false;
		return SetTexture(index, asset_texture_path);
	}

	bool ReMaterial::SetTexture(uint32_t index, const Texture& t)
	{
		if (!t.IsVaild())
			return false;
		if (index >= m_BindedTextures.size())
			return false;
		m_BindedTextures[index] = t;

		return true;
	}
	bool ReMaterial::SetTexture(const std::string& name, const Texture& t)
	{
		uint32_t index = GetBindedTextureIndex(name);
		if (index == -1)
			return false;
		return SetTexture(index, t);
	}
	bool ReMaterial::EraseTexture(const std::string& name)
	{
		uint32_t index = GetBindedTextureIndex(name);
		if (index == -1)
			return false;

		return EraseTexture(index);
	}
	bool ReMaterial::EraseTexture(uint32_t index)
	{
		if (m_BindedTextures.size() <= index)
			return false;
		std::string erase_keyname;
		for (auto& index_pair : m_TextureIndexMap)
		{
			if (index_pair.second > index)
				index_pair.second--;
			else if (index_pair.second == index)
				erase_keyname = index_pair.first;
		}
		m_TextureIndexMap.erase(erase_keyname);

		auto iter = m_BindedTextures.begin() + index;
		m_BindedTextures.erase(iter);

		for (auto& controller_pair : m_MatControllerPool)
		{
			auto& t_arr = controller_pair.second->m_BindedTextures;
			auto t_iter = t_arr.begin() + index;
			t_arr.erase(t_iter);
		}

		return true;
	}
	bool ReMaterial::TextureReName(const std::string& origin_name, const std::string& new_name)
	{
		if (m_TextureIndexMap.find(origin_name) == m_TextureIndexMap.end())
			return false;

		uint32_t index = m_TextureIndexMap[origin_name];
		m_TextureIndexMap.erase(origin_name);
		m_TextureIndexMap[new_name] = index;


		if (m_PixelCode.length() != 0)
		{
			string name = origin_name;
			string replace_name = new_name;
			size_t pos = 0;
			while ((pos = m_PixelCode.find(name, pos)) != std::string::npos)
			{
				char next_c = m_PixelCode[pos + name.length()];
				if (next_c == '.' || next_c == ' ')
				{
					m_PixelCode.replace(pos, name.length(), replace_name);
				}
				pos += name.length();
			}
		}

		


		return true;
	}
	void ReMaterial::SetTextureCount(uint32_t count)
	{
		m_BindedTextures.resize(count);

		for (auto& matcontroller_pair : m_MatControllerPool)
		{
			matcontroller_pair.second->m_BindedTextures.resize(count);
		}
	}
	uint32_t ReMaterial::GetTextureCount() const
	{
		return (uint32_t)m_BindedTextures.size();
	}
	void ReMaterial::SetCode(const std::string& code)
	{
		m_PixelCode = code;
	}
	bool ReMaterial::Compile()
	{
		auto shader_module = GetShaderModuleManager()->FindGraphicsModule(
			RenderItem::GetModuleNameByUsage(m_Usage));

		if (shader_module == nullptr)
		{
			RE_LOG_ERROR("Failed Compile Material ({0}) : Can't Find ShaderModule", GetName());
			return false;
		}
		std::string code = m_PixelCode;

		for (auto& index_pair : m_TextureIndexMap)
		{
			std::string name = index_pair.first;
			uint32_t    index = index_pair.second;
			std::string replace_name = FixedGShaderModule::MatTextureArrayName() + "[" + to_string(index) + "]";
	
			size_t pos = 0;
			while ((pos = code.find(name, pos)) != std::string::npos)
			{
				char next_c = code[pos + name.length()];
				if (next_c == '.' || next_c == ' ')
				{
					code.replace(pos, name.length(), replace_name);
				}
				pos += name.length();
			}

		}

		auto matCB = shader_module->FindConstantBuffer(FixedGShaderModule::MatCBName());
		if (matCB)
		{
			*matCB = *m_MaterialCB;
		}
		if (!shader_module->CustomCompile(m_CompiledPixelShader, code))
		{
			RE_LOG_ERROR("Failed Compile Material ({0}) : Compile Shader", GetName());
		}
		shader_module->MakePSO(m_PSO.get());
		m_PSO->BindShader(*m_CompiledPixelShader);
		if (!m_PSO->Finalize())
		{
			RE_LOG_ERROR("Failed Compile Material ({0}) : Failed Create PipelineState", GetName());
		}

		return true;
	}
	uint32_t       ReMaterial::GetBindedTextureIndex(const std::string& name)
	{
		if (m_TextureIndexMap.find(name) == m_TextureIndexMap.end())
		{
			return -1;
		}
		return m_TextureIndexMap[name];
	}
	uint32_t ReMaterial::GetBindedTextureCount() const
	{
		return (uint32_t)m_BindedTextures.size();
	}
	const std::vector<Texture>& ReMaterial::GetBindedTextures() const
	{
		return m_BindedTextures;
	}

	void ReMaterial::EraseValueInMaterialCB(const std::string& name)
	{
		m_MaterialCB->Remove(name);
		for (auto& controller_pair : m_MatControllerPool)
		{
			controller_pair.second->m_MaterialCB->Remove(name);
		}
	}
	ReMaterialController* ReMaterial::GetController() 
	{
		auto m = make_shared<ReMaterialController>(this);
		m->SetName(GetName() + "_MaterialController" + to_string(m_MatControllerCount++));
		m_MatControllerPool[m.get()] = m;
		return m.get();
	}
	bool  ReMaterial::DeleteController(ReMaterialController* matController)
	{
		if (m_MatControllerPool.find(matController) == m_MatControllerPool.end())
			return false;

		m_MatControllerPool.erase(matController);
		return true;
	}
	template<typename STType, typename InitType, JGShader::EShaderData enum_type>
	void AddInMaterialCBTemplate(
		std::unordered_map<ReMaterialController*, std::shared_ptr<ReMaterialController>>& matControllerPool,
		std::shared_ptr<SBDConstantBuffer>& matCB, 
		const std::string& name, 
		const InitType& init_value)
	{
		auto data = (STType*)matCB->Add(enum_type, name);
		data->Set(init_value);

		for (auto& controller_pair : matControllerPool)
		{
			data = (STType*)controller_pair.second->GetSBDMatCB()->Add(enum_type, name);
			data->Set(init_value);
		}
	}

	void ReMaterial::AddIntInMaterialCB(const std::string& name, int init_value)
	{
		AddInMaterialCBTemplate<STInt, int, JGShader::_int>(
			m_MatControllerPool,m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddInt2InMaterialCB(const std::string& name, const JVector2Int& init_value)
	{
		AddInMaterialCBTemplate<STInt2, JVector2Int, JGShader::_int2>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddInt3InMaterialCB(const std::string& name, const JVector3Int& init_value)
	{
		AddInMaterialCBTemplate<STInt3, JVector3Int, JGShader::_int3>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddInt4InMaterialCB(const std::string& name, const JVector4Int& init_value)
	{
		AddInMaterialCBTemplate<STInt4, JVector4Int, JGShader::_int4>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddUintInMaterialCB(const std::string& name, uint32_t init_value)
	{
		AddInMaterialCBTemplate<STUint, uint32_t, JGShader::_uint>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddUint2InMaterialCB(const std::string& name, const JVector2Uint& init_value)
	{
		AddInMaterialCBTemplate<STUint2, JVector2Uint, JGShader::_uint2>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddUint3InMaterialCB(const std::string& name, const JVector3Uint& init_value)
	{
		AddInMaterialCBTemplate<STUint3, JVector3Uint, JGShader::_uint3>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddUint4InMaterialCB(const std::string& name, const JVector4Uint& init_value)
	{
		AddInMaterialCBTemplate<STUint4, JVector4Uint, JGShader::_uint4>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddFloatInMaterialCB(const std::string& name, float init_value)
	{
		AddInMaterialCBTemplate<STFloat, float, JGShader::_float>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddFloat2InMaterialCB(const std::string& name, const JVector2& init_value)
	{
		AddInMaterialCBTemplate<STFloat2, JVector2, JGShader::_float2>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddFloat3InMaterialCB(const std::string& name, const JVector3& init_value)
	{
		AddInMaterialCBTemplate<STFloat3, JVector3, JGShader::_float3>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddFloat4InMaterialCB(const std::string& name, const JVector4& init_value)
	{
		AddInMaterialCBTemplate<STFloat4, JVector4, JGShader::_float4>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	void ReMaterial::AddMatrixInMaterialCB(const std::string& name, const JMatrix& init_value)
	{
		AddInMaterialCBTemplate<STMatrix, JMatrix, JGShader::_matrix4x4>(
			m_MatControllerPool, m_MaterialCB, name, init_value);
	}

	ReMaterialController::ReMaterialController(ReMaterial* owner) :
		m_MatOwner(owner)
	{
		m_MaterialCB = make_shared<SBDConstantBuffer>(FixedGShaderModule::MatCBName());
		*m_MaterialCB = *(owner->GetMaterialCB());
		m_BindedTextures = owner->GetBindedTextures();
	}
	std::vector<byte> ReMaterialController::GetCBData() const
	{
		return move(m_MaterialCB->GetData());
	}
	void     ReMaterialController::SetTexture(const std::string& name, const std::string& asset_texture_path)
	{
		uint32_t index = m_MatOwner->GetBindedTextureIndex(name);
		if (index == -1)
			return;
		auto t = TextureManager::GetTexture(asset_texture_path);
		m_BindedTextures[index] = t;
		//if (t.IsVaild())
		//{
		//	
		//}
	}
	void     ReMaterialController::SetTexture(const std::string& name, const Texture& t)
	{
		uint32_t index = m_MatOwner->GetBindedTextureIndex(name);
		if (index == -1)
			return;
		if (t.IsVaild())
		{
			m_BindedTextures[index] = t;
		}
	}
	uint32_t ReMaterialController::GetTextureCount() const
	{
		return (uint32_t)m_BindedTextures.size();
	}

	const std::vector<Texture>& ReMaterialController::GetTextureArray() const
	{
		return m_BindedTextures;
	}

	template<typename STType, typename ValueType, JGShader::EShaderData compare_type> 
	bool SetCBValue(SBDConstantBuffer* buffer, const std::string& name,const ValueType& value)
	{
		auto data = buffer->Get(name);
		if (data == nullptr)
			return false;
		if (data->GetType() != compare_type)
			return false;

		((STType*)data)->Set(value);
		return true;
	}

	template<typename STType, typename ValueType, JGShader::EShaderData compare_type>
	bool GetCBValue(SBDConstantBuffer* buffer, const std::string& name, ValueType& value)
	{
		auto data = buffer->Get(name);
		if (data == nullptr)
			return false;

		if (data->GetType() != compare_type)
			return false;

		value = ((STType*)data)->Get();
		return true;
	}






	bool ReMaterialController::SetValueAsInt(const std::string& name, int value)
	{
		
		return SetCBValue<STInt, int, JGShader::_int>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsInt2(const std::string& name, const JVector2Int& value)
	{
		return SetCBValue<STInt2, JVector2Int, JGShader::_int2>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsInt3(const std::string& name, const JVector3Int& value)
	{
		return SetCBValue<STInt3, JVector3Int, JGShader::_int3>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsInt4(const std::string& name, const JVector4Int& value)
	{
		return SetCBValue<STInt4, JVector4Int, JGShader::_int4>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsUInt(const std::string& name, uint32_t value)
	{
		return SetCBValue<STUint, uint32_t, JGShader::_uint>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsUInt2(const std::string& name, const JVector2Uint& value)
	{
		return SetCBValue<STUint2, JVector2Uint, JGShader::_uint2>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsUInt3(const std::string& name, const JVector3Uint& value)
	{
		return SetCBValue<STUint3, JVector3Uint, JGShader::_uint3>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsUInt4(const std::string& name, const JVector4Uint& value)
	{
		return SetCBValue<STUint4, JVector4Uint, JGShader::_uint4>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsFloat(const std::string& name, float value)
	{
		return SetCBValue<STFloat, float, JGShader::_uint4>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsFloat2(const std::string& name, const JVector2& value)
	{
		return SetCBValue<STFloat2, JVector2, JGShader::_float2>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsFloat3(const std::string& name, const JVector3& value)
	{
		return SetCBValue<STFloat3, JVector3, JGShader::_float3>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsFloat4(const std::string& name, const JVector4& value)
	{
		return SetCBValue<STFloat4, JVector4, JGShader::_float4>(
			m_MaterialCB.get(), name, value);
	}
	bool ReMaterialController::SetValueAsMatrix(const std::string& name, const JMatrix& value)
	{
		return SetCBValue<STMatrix, JMatrix, JGShader::_matrix4x4>(
			m_MaterialCB.get(), name, value);
	}

	bool ReMaterialController::GetValueAsInt(const std::string& name, int& out)
	{
		return GetCBValue<STInt, int, JGShader::_int>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsInt2(const std::string& name, JVector2Int& out)
	{
		return GetCBValue<STInt2, JVector2Int, JGShader::_int2>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsInt3(const std::string& name, JVector3Int& out)
	{
		return GetCBValue<STInt3, JVector3Int, JGShader::_int3>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsInt4(const std::string& name, JVector4Int& out)
	{
		return GetCBValue<STInt4, JVector4Int, JGShader::_int4>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsUInt(const std::string& name, uint32_t& out)
	{
		return GetCBValue<STUint, uint32_t, JGShader::_uint>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsUInt2(const std::string& name, JVector2Uint& out)
	{
		return GetCBValue<STUint2, JVector2Uint , JGShader::_uint2>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsUInt3(const std::string& name, JVector3Uint& out)
	{
		return GetCBValue<STUint3, JVector3Uint, JGShader::_uint3>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsUInt4(const std::string& name, JVector4Uint& out)
	{
		return GetCBValue<STUint4, JVector4Uint, JGShader::_uint4>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsFloat(const std::string& name, float& out)
	{
		return GetCBValue<STFloat, float, JGShader::_float>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsFloat2(const std::string& name, JVector2& out)
	{
		return GetCBValue<STFloat2, JVector2, JGShader::_float2>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsFloat3(const std::string& name, JVector3& out)
	{
		return GetCBValue<STFloat3, JVector3, JGShader::_float3>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsFloat4(const std::string& name, JVector4& out)
	{
		return GetCBValue<STFloat4, JVector4, JGShader::_float4>(
			m_MaterialCB.get(), name, out);
	}
	bool ReMaterialController::GetValueAsMatrix(const std::string& name, JMatrix& out)
	{
		return GetCBValue<STMatrix, JMatrix, JGShader::_matrix4x4>(
			m_MaterialCB.get(), name, out);
	}



	ReMaterialManager* g_MaterialManager = nullptr;
	ReMaterialManager::ReMaterialManager(const std::string& name) : ReObject(name)
	{
		g_MaterialManager = this;
	}
	bool ReMaterialManager::RegisterMaterial(const std::string& name, const ReMaterial& mat)
	{
		if (g_MaterialManager->m_MaterialPool.find(name) != g_MaterialManager->m_MaterialPool.end())
			return false;

		g_MaterialManager->m_MaterialPool[name] = make_shared<ReMaterial>(mat);
		g_MaterialManager->m_MaterialPool[name]->SetName(name);
		return true;
	}
	ReMaterial* ReMaterialManager::GetMaterial(const std::string& name)
	{
		if (g_MaterialManager->m_MaterialPool.find(name) == g_MaterialManager->m_MaterialPool.end())
			return false;
		return g_MaterialManager->m_MaterialPool[name].get();
	}
	ReMaterialController* ReMaterialManager::GetMaterialController(const std::string& mat_name)
	{
		if (g_MaterialManager->m_MaterialPool.find(mat_name) == g_MaterialManager->m_MaterialPool.end())
			return nullptr;

		auto p = g_MaterialManager->m_MaterialPool[mat_name].get();
		return p->GetController();
	}
	bool ReMaterialManager::DeleteMaterialController(ReMaterialController* mat_controller)
	{
		auto owner = mat_controller->GetMatOwner();
		if (GetMaterial(owner->GetName()) == nullptr)
		{
			return false;
		}
		return owner->DeleteController(mat_controller);
	}
}



