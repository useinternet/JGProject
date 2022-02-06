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
		return SetData(name, value);
	}
	bool DirectX12Material::SetFloat2(const String& name, const JVector2& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetFloat3(const String& name, const JVector3& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetFloat4(const String& name, const JVector4& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetInt(const String& name, i32 value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetInt2(const String& name, const JVector2Int& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetInt3(const String& name, const JVector3Int& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetInt4(const String& name, const JVector4Int& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetUint(const String& name, u32 value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetUint2(const String& name, const JVector2Uint& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetUint3(const String& name, const JVector3Uint& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetUint4(const String& name, const JVector4Uint& value)
	{
		return SetData(name, value);
	}
	bool DirectX12Material::SetFloat4x4(const String& name, const JMatrix& value)
	{
		return SetData(name, value);
	}

	bool DirectX12Material::SetTexture(const String& name, SharedPtr<ITexture> texture)
	{
		if (mTextures.find(name) == mTextures.end())
		{
			return false;
		}

		mTextures[name] = texture;
		return true;
	}
	//bool DirectX12Material::SetTextureCube(const String& name, SharedPtr<ITexture> texture)
	//{
	//	if (mTextureCubes.find(name) == mTextureCubes.end())
	//	{
	//		return false;
	//	}

	//	mTextureCubes[name] = texture;
	//	return true;
	//}
	bool DirectX12Material::GetFloat(const String& name, float* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetFloat2(const String& name, JVector2* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetFloat3(const String& name, JVector3* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetFloat4(const String& name, JVector4* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetInt(const String& name, i32* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetInt2(const String& name, JVector2Int* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetInt3(const String& name, JVector3Int* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetInt4(const String& name, JVector4Int* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetUint(const String& name, u32* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetUint2(const String& name, JVector2Uint* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetUint3(const String& name, JVector3Uint* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetUint4(const String& name, JVector4Uint* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetFloat4x4(const String& name, JMatrix* out_value)
	{
		return GetData(name, out_value);
	}

	bool DirectX12Material::GetTexture(const String& name, SharedPtr<ITexture>* out_value)
	{
		if (mTextures.find(name) == mTextures.end())
		{
			return false;
		}
		if (out_value != nullptr)
		{
			*out_value = mTextures[name];
		}

		return true;
	}

	//bool DirectX12Material::GetTextureCube(const String& name, SharedPtr<ITexture>* out_value)
	//{
	//	return false;
	//}

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

	SharedPtr<IGraphicsShader> DirectX12Material::GetShader() const
	{
		return mGraphicsShader;
	}


	void DirectX12Material::SetShader(SharedPtr<IGraphicsShader> shader)
	{
		Init(shader);
	}

	const List<std::pair<EShaderDataType, String>>& DirectX12Material::GetPropertyList() const
	{
		if (mGraphicsShader == nullptr)
		{
			static List<std::pair<EShaderDataType, String>> temp;
			return temp;
		}
		return mGraphicsShader->GetPropertyList();
	}

	SharedPtr<IShaderScript> DirectX12Material::GetScript() const
	{
		if (mGraphicsShader == nullptr)
		{
			return nullptr;
		}
		return mGraphicsShader->GetScriptList()[0];
	}

	bool DirectX12Material::IsValid() const
	{
		return mGraphicsShader != nullptr && mGraphicsShader->IsSuccessed();
	}

	List<SharedPtr<ITexture>> DirectX12Material::GetTextureList() const
	{
		if (mGraphicsShader == nullptr || mGraphicsShader->IsSuccessed() == false)
		{
			return List<SharedPtr<ITexture>>();
		}
		List<SharedPtr<ITexture>> texList;
		DirectX12GraphicsShader*  pDX12Shader = static_cast<DirectX12GraphicsShader*>(mGraphicsShader.get());
		pDX12Shader->ForEach_TextureSlot([&](const String& name)
		{
			if (mTextures.find(name) == mTextures.end())
			{
				texList.push_back(nullptr);
			}
			else
			{
				texList.push_back(mTextures.at(name));
			}


		});
		return texList;
	}

	//List<SharedPtr<ITexture>> DirectX12Material::GetCubeTextureList() const
	//{
	//	if (mGraphicsShader == nullptr || mGraphicsShader->IsSuccessed() == false)
	//	{
	//		return List<SharedPtr<ITexture>>();;
	//	}
	//	List<SharedPtr<ITexture>> texList;
	//	DirectX12GraphicsShader* pDX12Shader = static_cast<DirectX12GraphicsShader*>(mGraphicsShader.get());
	//	pDX12Shader->ForEach_TextureCubeSlot([&](const String& name)
	//	{
	//		if (mTextureCubes.find(name) == mTextureCubes.end())
	//		{
	//			texList.push_back(nullptr);
	//		}
	//		else
	//		{
	//			texList.push_back(mTextureCubes.at(name));
	//		}
	//	});
	//	return texList;
	//}

	const List<jbyte>& DirectX12Material::GetMaterialPropertyByteData()
	{
		{
			std::lock_guard<std::mutex> lock(mMutex);
			memcpy(mUploadBtData.data(), mBtData.data(), mBtData.size());
		}
		return mUploadBtData;
	}

	bool DirectX12Material::Bind()
	{
		if (mGraphicsShader == nullptr || mGraphicsShader->IsSuccessed() == false)
		{
			return false;
		}
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->SetDepthStencilState(mDepthStencilDesc);
		pso->SetBlendState(mBlendDesc);
		pso->SetRasterizerState(mRasterzerDesc);
		return true;
	}

	void DirectX12Material::Init(SharedPtr<IGraphicsShader> shader)
	{
		SetDepthStencilState(EDepthStencilStateTemplate::Default);
		mBlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		SetBlendState(0, EBlendStateTemplate::Default);
		SetRasterizerState(ERasterizerStateTemplate::Default);




		mGraphicsShader = shader;



		auto propertyList = mGraphicsShader->GetPropertyList();

		u64 btPos = 0;
		for (auto& _pair : propertyList)
		{
			auto type = _pair.first;
			auto name = _pair.second;
			u64  size = 0;
			switch (type)
			{
			case EShaderDataType::textureCube:
			case EShaderDataType::texture2D:
				mTextures[name] = nullptr;
				break;
			default:
				size = GetShaderDataTypeSize(type);
				mCBDatas[name] = CBPair(btPos, size);
				btPos += size;
				break;
			}
		}
		mBtData.resize(btPos, 0);
		mUploadBtData.resize(btPos, 0);
	}
}