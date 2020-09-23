#include "pch.h"
#include "DX12_Material.h"
#include "DX12_Texture.h"
#include "../GraphicsEngine.h"
#include "../GraphicsIF.h"
using namespace std;


namespace DX12
{
	bool DX12_MaterialInstance::SetVarAsFloat(const std::string& name, float f)
	{
		return SetVarAs<float, _float_>(name, f);
	}
	bool DX12_MaterialInstance::SetVarAsFloat2(const std::string& name, const JVector2& v)
	{
		return SetVarAs<JVector2, _float2_>(name, v);
	}
	bool DX12_MaterialInstance::SetVarAsFloat3(const std::string& name, const JVector3& v)
	{
		return SetVarAs<JVector3, _float3_>(name, v);
	}
	bool DX12_MaterialInstance::SetVarAsFloat4(const std::string& name, const JVector4& v)
	{
		return SetVarAs<JVector4, _float4_>(name, v);
	}
	bool DX12_MaterialInstance::SetVarAsInt(const std::string& name, int i)
	{
		return SetVarAs<int, _int_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsInt2(const std::string& name, const JVector2Int& i)
	{
		return SetVarAs<JVector2Int, _int2_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsInt3(const std::string& name, const JVector3Int& i)
	{
		return SetVarAs<JVector3Int, _int3_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsInt4(const std::string& name, const JVector4Int& i)
	{
		return SetVarAs<JVector4Int, _int4_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsUint(const std::string& name, uint32_t i)
	{
		return SetVarAs<uint32_t, _uint_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsUint2(const std::string& name, const JVector2Uint& i)
	{
		return SetVarAs<JVector2Uint, _uint2_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsUint3(const std::string& name, const JVector3Uint& i)
	{
		return SetVarAs<JVector3Uint, _uint3_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsUint4(const std::string& name, const JVector4Uint& i)
	{
		return SetVarAs<JVector4Uint, _uint4_>(name, i);
	}
	bool DX12_MaterialInstance::SetVarAsMatrix4x4(const std::string& name, const JMatrix& m)
	{
		return SetVarAs<JMatrix, _float4x4_>(name, m);
	}

	bool DX12_MaterialInstance::BindTexture(const std::string& name, GE::Texture* t)
	{
		auto iter = std::find_if(m_BindedTextures.begin(), m_BindedTextures.end(), [&](const pair<string, GE::Texture*>& _pair)
		{
			return name == _pair.first;
		});
		if (iter == m_BindedTextures.end()) return false;

		iter->second = t;

		return true;
	}

	bool DX12_MaterialInstance::GetVarAsFloat(const std::string& name, float& dataRef) const
	{
		return GetVarAs<float, _float_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsFloat2(const std::string& name, JVector2& dataRef) const
	{
		return GetVarAs<JVector2, _float2_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsFloat3(const std::string& name, JVector3& dataRef) const
	{
		return GetVarAs<JVector3, _float3_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsFloat4(const std::string& name, JVector4& dataRef) const
	{
		return GetVarAs<JVector4, _float4_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsInt(const std::string& name, int& dataRef) const
	{

		return GetVarAs<int, _int_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsInt2(const std::string& name, JVector2Int& dataRef) const
	{
		return GetVarAs<JVector2Int, _int2_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsInt3(const std::string& name, JVector3Int& dataRef) const
	{
		return GetVarAs<JVector3Int, _int3_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsInt4(const std::string& name, JVector4Int& dataRef) const
	{
		return GetVarAs<JVector4Int, _int4_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsUint(const std::string& name, uint32_t& dataRef) const
	{
		return GetVarAs<uint32_t, _uint_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsUint2(const std::string& name, JVector2Uint& dataRef) const
	{
		return GetVarAs<JVector2Uint, _uint2_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsUint3(const std::string& name, JVector3Uint& dataRef) const
	{
		return GetVarAs<JVector3Uint, _uint3_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsUint4(const std::string& name, JVector4Uint& dataRef) const
	{
		return GetVarAs<JVector4Uint, _uint4_>(name, &dataRef);
	}
	bool DX12_MaterialInstance::GetVarAsMatrix4x4(const std::string& name, JMatrix& dataRef) const
	{
		return GetVarAs<JMatrix, _float4x4_>(name, &dataRef);
	}

	void DX12_MaterialInstance::GetTextureData(void* dataPtr) const
	{
		vector<Texture>& tArray = (*(vector<Texture>*)dataPtr);
		size_t len = m_BindedTextures.size();
		tArray.resize(m_BindedTextures.size());

		for (size_t i = 0; i < len; ++i)
		{
			if (m_BindedTextures[i].second)
			{ 
				tArray[i] = *(Texture*)m_BindedTextures[i].second->GetUserTextureData();
			}
			else
			{
				if (GraphicsIF::GetInValidTexture().IsValid())
				{
					tArray[i] = (*(Texture*)GraphicsIF::GetInValidTexture()->GetUserTextureData());
				}
				else
				{
					Texture t;
					t.CreateTexture(TT("InvalidTexture"), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1);
					tArray[i] = move(t);
				}
			}
		}
	}
	const std::vector<std::pair<std::string, GE::Texture*>>& DX12_MaterialInstance::GetBindedTextures() const
	{
		return m_BindedTextures;
	}


	void DX12_Material::SetMaterialProperty(const GE::MaterialProperty& ppt)
	{
		SendDirty();
		m_Property = ppt;
	}

	const GE::MaterialProperty& DX12_Material::GetMaterialProperty() const
	{
		return m_Property;
	}

	void DX12_Material::SetCode(const std::string& code)
	{
		SendDirty();
		m_ShaderCode = code;
	}

	bool DX12_Material::DefineVarAsFloat(const std::string& name, float f)
	{
		return DefineVarAs<float, _float_>(name, f);
	}

	bool DX12_Material::DefineVarAsFloat2(const std::string& name, const JVector2& v)
	{
		return DefineVarAs<JVector2, _float2_>(name, v);
	}

	bool DX12_Material::DefineVarAsFloat3(const std::string& name, const JVector3& v)
	{
		return DefineVarAs<JVector3, _float3_>(name, v);
	}

	bool DX12_Material::DefineVarAsFloat4(const std::string& name, const JVector4& v)
	{
		return DefineVarAs<JVector4, _float4_>(name, v);
	}

	bool DX12_Material::DefineVarAsInt(const std::string& name, int i)
	{
		return DefineVarAs<int, _int_>(name, i);
	}

	bool DX12_Material::DefineVarAsInt2(const std::string& name, const JVector2Int& i)
	{
		return DefineVarAs<JVector2Int, _int2_>(name, i);
	}
	bool DX12_Material::DefineVarAsInt3(const std::string& name, const JVector3Int& i)
	{
		return DefineVarAs<JVector3Int, _int3_>(name, i);
	}
	bool DX12_Material::DefineVarAsInt4(const std::string& name, const JVector4Int& i)
	{
		return DefineVarAs<JVector4Int, _int4_>(name, i);
	}
	bool DX12_Material::DefineVarAsUint(const std::string& name, uint32_t i)
	{
		return DefineVarAs<uint32_t, _uint_>(name, i);

	}
	bool DX12_Material::DefineVarAsUint2(const std::string& name, const JVector2Uint& i)
	{
		return DefineVarAs<JVector2Uint, _uint2_>(name, i);
	}
	bool DX12_Material::DefineVarAsUint3(const std::string& name, const JVector3Uint& i)
	{
		return DefineVarAs<JVector3Uint, _uint3_>(name, i);
	}
	bool DX12_Material::DefineVarAsUint4(const std::string& name, const JVector4Uint& i)
	{
		return DefineVarAs<JVector4Uint, _uint4_>(name, i);
	}
	bool DX12_Material::DefineVarAsMatrix4x4(const std::string& name, const JMatrix& m)
	{
		return DefineVarAs<JMatrix, _float4x4_>(name, m);
	}
	void DX12_Material::UnDefineVar(const std::string& name)
	{
		SendDirty();
		auto iter = std::find_if(m_DataBlocks.begin(), m_DataBlocks.end(), [&](const DX12_MaterialInstance::DataBlock& block) -> bool
		{
			return block.name == name;
		});

		if (iter != m_DataBlocks.end()) {

			auto& block = *iter;

			auto start_iter = m_BtData.begin() + block.pos;
			auto end_iter = m_BtData.end() + block.pos + block.size;

			m_BtData.erase(start_iter, end_iter);
			m_DataBlocks.erase(iter);

			replaceAll(m_CBCode, ToString(block.type) + " " + name + SHADER_NAME_SUFFIX_MATERIAL_DATA + "; \n", "");
			for (auto& ins : m_MaterialInstances)
			{
				DX12_MaterialInstance* ptr = (DX12_MaterialInstance*)ins.get();
				

				auto ins_start_iter = ptr->m_BtData.begin() + block.pos;
				auto ins_end_iter = ptr->m_BtData.end() + block.pos + block.size;

				ptr->m_BtData.erase(ins_start_iter, ins_end_iter);
				ptr->m_DataBlocks = m_DataBlocks;
			}
		}
	}

	bool DX12_Material::DefineTexture(const std::string& name, GE::Texture* init_t)
	{
		SendDirty();
		auto iter = std::find_if(m_BindedTextures.begin(), m_BindedTextures.end(), [&](const pair<string, GE::Texture*>& _pair)
		{
			return name == _pair.first;
		});

		if (iter != m_BindedTextures.end()) return false;


		m_BindedTextures.push_back(make_pair(name, init_t));

		for (auto& ins : m_MaterialInstances)
		{
			DX12_MaterialInstance* ptr = (DX12_MaterialInstance*)ins.get();
			ptr->m_BindedTextures.push_back(make_pair(name, init_t));
		}
		return true;
	}

	void DX12_Material::UnDefineTexture(const std::string& name)
	{
		SendDirty();
		auto iter = std::find_if(m_BindedTextures.begin(), m_BindedTextures.end(), [&](const pair<string, GE::Texture*>& _pair)
		{
			return name == _pair.first;
		});

		if (iter == m_BindedTextures.end()) return;

		m_BindedTextures.erase(iter);

		size_t index = std::distance(m_BindedTextures.begin(), iter);

		for (auto& ins : m_MaterialInstances)
		{
			DX12_MaterialInstance* ptr = (DX12_MaterialInstance*)ins.get();
			auto remove_iter = ptr->m_BindedTextures.begin() + index;
			ptr->m_BindedTextures.erase(remove_iter);
		}
		

	}

	const std::vector<std::pair<std::string, GE::Texture*>>& DX12_Material::GetBindedTextures() const
	{
		return m_BindedTextures;
	}

	GE::MaterialInstance* DX12_Material::CreateMaterialInstance()
	{
		auto matInstance = make_unique<DX12_MaterialInstance>();
		



		matInstance->m_BtData.resize(m_BtData.size());
		memcpy(&matInstance->m_BtData[0], m_BtData.data(), m_BtData.size());
		matInstance->m_DataBlocks = m_DataBlocks;
		matInstance->m_BindedTextures = m_BindedTextures;
		matInstance->m_OwnerMaterial = this;
		GE::MaterialInstance* result = matInstance.get();
		m_MaterialInstances.push_back(move(matInstance));

		return result;
	}

	void DX12_Material::DeleteMaterialInstance(GE::MaterialInstance* instance)
	{
		auto iter = std::find_if(m_MaterialInstances.begin(), m_MaterialInstances.end(), [&](const std::unique_ptr<GE::MaterialInstance>& ins) -> bool
		{
			return ins.get() == instance;
		});
		if (iter != m_MaterialInstances.end())
		{
			m_MaterialInstances.erase(iter);
		}
	}

	uint32_t DX12_Material::GetMaterialInstanceCount() const
	{
		return (uint32_t)m_MaterialInstances.size();
	}

	bool DX12_Material::IsDirty() const
	{
		return m_IsDirty;
	}
	void DX12_Material::ClearDirty()
	{
		m_IsDirty = false;
	}
	void DX12_Material::SendDirty()
	{
		m_IsDirty = true;
	}


	const std::string& DX12_Material::GetCode() const
	{
		return m_ShaderCode;
	}

	const std::string& DX12_Material::GetMaterialCBCode() const
	{
		return m_CBCode;
	}
	std::vector<std::string> DX12_Material::GetMaterialDataNameList() const
	{
		size_t listSize = m_DataBlocks.size();
		std::vector<std::string> list(listSize);
		for (size_t i = 0; i < listSize; ++i)
		{
			list[i] = m_DataBlocks[i].name;
		}
		return move(list);
	}
	std::vector<std::string> DX12_Material::GetMaterialTextureNameList() const
	{
		size_t listSize = m_BindedTextures.size();
		std::vector<std::string> list(listSize);
		for (size_t i = 0; i < listSize; ++i)
		{
			list[i] = m_BindedTextures[i].first;
		}


		return move(list);
	}
}