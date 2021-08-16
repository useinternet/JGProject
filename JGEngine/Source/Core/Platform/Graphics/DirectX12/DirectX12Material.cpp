#include "pch.h"
#include "DirectX12Material.h"
#include "DirectX12API.h"
#include "Utill/CommandList.h"
#include "Utill/PipelineState.h"
#include "Utill/ShaderDataForm.h"
namespace JG
{
	bool DirectX12Material::SetFloat(const String& name, float value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetFloat(name, value);
	}
	bool DirectX12Material::SetFloat2(const String& name, const JVector2& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return  mShaderData->SetFloat2(name, value);
	}
	bool DirectX12Material::SetFloat3(const String& name, const JVector3& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return  mShaderData->SetFloat3(name, value);
	}
	bool DirectX12Material::SetFloat4(const String& name, const JVector4& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return  mShaderData->SetFloat4(name, value);
	}
	bool DirectX12Material::SetInt(const String& name, i32 value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetInt(name, value);
	}
	bool DirectX12Material::SetInt2(const String& name, const JVector2Int& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetInt2(name, value);
	}
	bool DirectX12Material::SetInt3(const String& name, const JVector3Int& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetInt3(name, value);
	}
	bool DirectX12Material::SetInt4(const String& name, const JVector4Int& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetInt4(name, value);
	}
	bool DirectX12Material::SetUint(const String& name, u32 value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUint(name, value);
	}
	bool DirectX12Material::SetUint2(const String& name, const JVector2Uint& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUint2(name, value);
	}
	bool DirectX12Material::SetUint3(const String& name, const JVector3Uint& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUint3(name, value);
	}
	bool DirectX12Material::SetUint4(const String& name, const JVector4Uint& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUint4(name, value);
	}
	bool DirectX12Material::SetFloat4x4(const String& name, const JMatrix& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetFloat4x4(name, value);
	}

	bool DirectX12Material::SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetTexture(name, textureSlot, texture);
	}

	bool DirectX12Material::SetFloatArray(const String& name, const List<float>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetFloatArray(name, value);
	}

	bool DirectX12Material::SetFloat2Array(const String& name, const List<JVector2>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetFloat2Array(name, value);
	}

	bool DirectX12Material::SetFloat3Array(const String& name, const List<JVector3>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetFloat3Array(name, value);
	}

	bool DirectX12Material::SetFloat4Array(const String& name, const List<JVector4>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetFloat4Array(name, value);
	}

	bool DirectX12Material::SetIntArray(const String& name, const List<i32>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetIntArray(name, value);
	}

	bool DirectX12Material::SetInt2Array(const String& name, const List<JVector2Int>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetInt2Array(name, value);
	}

	bool DirectX12Material::SetInt3Array(const String& name, const List<JVector3Int>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetInt3Array(name, value);
	}

	bool DirectX12Material::SetInt4Array(const String& name, const List<JVector4Int>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetInt4Array(name, value);
	}

	bool DirectX12Material::SetUintArray(const String& name, const List<u32>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUintArray(name, value);
	}

	bool DirectX12Material::SetUint2Array(const String& name, const List<JVector2Uint>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUint2Array(name, value);
	}

	bool DirectX12Material::SetUint3Array(const String& name, const List<JVector3Uint>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUint3Array(name, value);
	}

	bool DirectX12Material::SetUint4Array(const String& name, const List<JVector4Uint>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetUint4Array(name, value);
	}

	bool DirectX12Material::SetFloat4x4Array(const String& name, const List<JMatrix>& value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetFloat4x4Array(name, value);
	}

	bool DirectX12Material::SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->SetStructDataArray(name, datas, elementCount, elementSize);
	}



	bool DirectX12Material::GetFloat(const String& name, float* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetFloat(name, out_value);
	}

	bool DirectX12Material::GetFloat2(const String& name, JVector2* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetFloat2(name, out_value);
	}

	bool DirectX12Material::GetFloat3(const String& name, JVector3* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetFloat3(name, out_value);
	}

	bool DirectX12Material::GetFloat4(const String& name, JVector4* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetFloat4(name, out_value);
	}

	bool DirectX12Material::GetInt(const String& name, i32* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetInt(name, out_value);
	}

	bool DirectX12Material::GetInt2(const String& name, JVector2Int* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetInt2(name, out_value);
	}

	bool DirectX12Material::GetInt3(const String& name, JVector3Int* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetInt3(name, out_value);
	}

	bool DirectX12Material::GetInt4(const String& name, JVector4Int* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetInt4(name, out_value);
	}

	bool DirectX12Material::GetUint(const String& name, u32* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetUint(name, out_value);
	}

	bool DirectX12Material::GetUint2(const String& name, JVector2Uint* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetUint2(name, out_value);
	}

	bool DirectX12Material::GetUint3(const String& name, JVector3Uint* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetUint3(name, out_value);
	}

	bool DirectX12Material::GetUint4(const String& name, JVector4Uint* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetUint4(name, out_value);
	}

	bool DirectX12Material::GetFloat4x4(const String& name, JMatrix* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetFloat4x4(name, out_value);
	}

	bool DirectX12Material::GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value)
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		return mShaderData->GetTexture(name, textureSlot, out_value);
	}

	void DirectX12Material::SetDepthStencilState(EDepthStencilStateTemplate _template)
	{
		DirectX12API::GetDepthStencilDesc(_template, &mDepthStencilDesc);
	}

	void DirectX12Material::SetBlendState(u32 slot, EBlendStateTemplate _template)
	{
		if (slot >= MAX_RENDERTARGET)
		{
			return;
		}
		DirectX12API::GetBlendDesc(_template, &mBlendDesc.RenderTarget[slot]);
	}

	void DirectX12Material::SetRasterizerState(ERasterizerStateTemplate _template)
	{
		DirectX12API::GetRasterizerDesc(_template, &mRasterzerDesc);
	}


	void DirectX12Material::SetName(const String& name)
	{
		mName = name;
	}

	const String& DirectX12Material::GetName() const
	{
		return mName;
	}

	void DirectX12Material::SetShader(SharedPtr<IShader> shader)
	{
		Init(shader);
	}

	List<std::pair<EShaderDataType, String>> DirectX12Material::GetPropertyList() const
	{
		if (mShaderData != nullptr)
		{
			auto shader = mShaderData->GetOwnerShader();
			if (shader != nullptr)
			{
				return shader->GetPropertyList();
			}

		}

		return List<std::pair<EShaderDataType, String>>();
	}

	bool DirectX12Material::Bind()
	{
		if (mShaderData == nullptr)
		{
			return false;
		}
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->SetDepthStencilState(mDepthStencilDesc);
		pso->SetBlendState(mBlendDesc);
		pso->SetRasterizerState(mRasterzerDesc);
		return mShaderData->Bind(GetCommandID());
	}

	void DirectX12Material::Init(SharedPtr<IShader> shader)
	{
		mShaderData = CreateUniquePtr<ShaderData>(shader);
		SetDepthStencilState(EDepthStencilStateTemplate::Default);
		mBlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		SetBlendState(0, EBlendStateTemplate::Default);
		SetRasterizerState(ERasterizerStateTemplate::Default);
	}
}