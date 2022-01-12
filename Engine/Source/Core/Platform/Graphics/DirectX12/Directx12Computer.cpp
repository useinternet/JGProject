#include "pch.h"
#include "Directx12Computer.h"
#include "DirectX12Shader.h"
#include "DirectX12API.h"
#include "Utill/ResourceStateTracker.h"
#include "Utill/CommandQueue.h"
#include "Utill/DescriptorAllocator.h"
#include "Utill/CommandList.h"
#include "Utill/PipelineState.h"

namespace JG
{
	bool DirectX12Computer::SetFloat(const String& name, float value)
	{
		return mShaderData->SetFloat(name, value);
	}
	bool DirectX12Computer::SetFloat2(const String& name, const JVector2& value)
	{
		return  mShaderData->SetFloat2(name, value);
	}
	bool DirectX12Computer::SetFloat3(const String& name, const JVector3& value)
	{
		return  mShaderData->SetFloat3(name, value);
	}
	bool DirectX12Computer::SetFloat4(const String& name, const JVector4& value)
	{
		return  mShaderData->SetFloat4(name, value);
	}
	bool DirectX12Computer::SetInt(const String& name, i32 value)
	{
		return mShaderData->SetInt(name, value);
	}
	bool DirectX12Computer::SetInt2(const String& name, const JVector2Int& value)
	{
		return mShaderData->SetInt2(name, value);
	}
	bool DirectX12Computer::SetInt3(const String& name, const JVector3Int& value)
	{
		return mShaderData->SetInt3(name, value);
	}
	bool DirectX12Computer::SetInt4(const String& name, const JVector4Int& value)
	{
		return mShaderData->SetInt4(name, value);
	}
	bool DirectX12Computer::SetUint(const String& name, u32 value)
	{
		return mShaderData->SetUint(name, value);
	}
	bool DirectX12Computer::SetUint2(const String& name, const JVector2Uint& value)
	{
		return mShaderData->SetUint2(name, value);
	}
	bool DirectX12Computer::SetUint3(const String& name, const JVector3Uint& value)
	{
		return mShaderData->SetUint3(name, value);
	}
	bool DirectX12Computer::SetUint4(const String& name, const JVector4Uint& value)
	{
		return mShaderData->SetUint4(name, value);
	}
	bool DirectX12Computer::SetFloat4x4(const String& name, const JMatrix& value)
	{
		return mShaderData->SetFloat4x4(name, value);
	}

	bool DirectX12Computer::SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture)
	{
		return mShaderData->SetTexture(name, textureSlot, texture);
	}

	bool DirectX12Computer::SetByteAddressBuffer(const String& name, SharedPtr<IByteAddressBuffer> bab)
	{
		return mShaderData->SetByteAddressBuffer(name, bab);
	}

	bool DirectX12Computer::SetStructuredBuffer(const String& name, SharedPtr<IStructuredBuffer> sb)
	{
		return mShaderData->SetStructuredBuffer(name, sb);
	}
	bool DirectX12Computer::GetFloat(const String& name, float* out_value)
	{
		return mShaderData->GetFloat(name, out_value);
	}

	bool DirectX12Computer::GetFloat2(const String& name, JVector2* out_value)
	{
		return mShaderData->GetFloat2(name, out_value);
	}

	bool DirectX12Computer::GetFloat3(const String& name, JVector3* out_value)
	{
		return mShaderData->GetFloat3(name, out_value);
	}

	bool DirectX12Computer::GetFloat4(const String& name, JVector4* out_value)
	{
		return mShaderData->GetFloat4(name, out_value);
	}

	bool DirectX12Computer::GetInt(const String& name, i32* out_value)
	{
		return mShaderData->GetInt(name, out_value);
	}

	bool DirectX12Computer::GetInt2(const String& name, JVector2Int* out_value)
	{
		return mShaderData->GetInt2(name, out_value);
	}

	bool DirectX12Computer::GetInt3(const String& name, JVector3Int* out_value)
	{
		return mShaderData->GetInt3(name, out_value);
	}

	bool DirectX12Computer::GetInt4(const String& name, JVector4Int* out_value)
	{
		return mShaderData->GetInt4(name, out_value);
	}

	bool DirectX12Computer::GetUint(const String& name, u32* out_value)
	{
		return mShaderData->GetUint(name, out_value);
	}

	bool DirectX12Computer::GetUint2(const String& name, JVector2Uint* out_value)
	{
		return mShaderData->GetUint2(name, out_value);
	}

	bool DirectX12Computer::GetUint3(const String& name, JVector3Uint* out_value)
	{
		return mShaderData->GetUint3(name, out_value);
	}

	bool DirectX12Computer::GetUint4(const String& name, JVector4Uint* out_value)
	{
		return mShaderData->GetUint4(name, out_value);
	}

	bool DirectX12Computer::GetFloat4x4(const String& name, JMatrix* out_value)
	{
		return mShaderData->GetFloat4x4(name, out_value);
	}

	bool DirectX12Computer::GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value)
	{
		return mShaderData->GetTexture(name, textureSlot, out_value);
	}

	SharedPtr<IByteAddressBuffer> DirectX12Computer::GetByteAddressBuffer(const String& name)
	{
		return mShaderData->GetByteAddressBuffer(name);
	}

	SharedPtr<IStructuredBuffer> DirectX12Computer::GetStructuredBuffer(const String& name)
	{
		return mShaderData->GetStructuredBuffer(name);
	}

	void DirectX12Computer::SetComputeShader(SharedPtr<IComputeShader> shader)
	{
		if (shader == nullptr || shader->IsSuccessed() == false)
		{
			return;
		}

		mOwnerShader = shader;
		auto dx12Shader = static_cast<DirectX12ComputeShader*>(shader.get());
		mShaderData = CreateUniquePtr<ShaderData>(dx12Shader->GetShaderDataForm());
	}
	const String& DirectX12Computer::GetName() const
	{
		return mName;
	}
	void DirectX12Computer::SetName(const String& name)
	{
		mName = name;
	}

	bool DirectX12Computer::Dispatch(u32 groupX, u32 groupY, u32 groupZ , bool asComputeCommand)
	{
		if (mOwnerShader == nullptr || mOwnerShader->IsSuccessed() == false)
		{
			return false;
		}
		if (asComputeCommand)
		{
			ComputeCommandList* commandList = DirectX12API::GetComputeCommandList();
			DispatchInternal(commandList, groupX, groupY, groupZ);
		}
		else
		{
			auto graphicsCmdList = DirectX12API::GetGraphicsCommandList();
			graphicsCmdList->AsCompute([&](SharedPtr<ComputeCommandList> commandList)
			{
				DispatchInternal(commandList.get(), groupX, groupY, groupZ);
			});
		}

		return true;
	}

	bool  DirectX12Computer::DispatchInternal(ComputeCommandList* commandList, u32 groupX, u32 groupY, u32 groupZ)
	{
		auto RootSig = DirectX12API::GetComputeRootSignature();
		commandList->BindRootSignature(RootSig);


		// Data Bind
		mShaderData->ForEach_CB([&](const ShaderDataForm::CBufferData* data, const List<jbyte>& btData)
		{
			commandList->BindConstantBuffer(data->RootParm, btData.data(), btData.size());
		});
		mShaderData->ForEach_SB([&](const ShaderDataForm::StructuredBufferData* data, SharedPtr<IStructuredBuffer> sb)
		{
			if (sb == nullptr)
			{
				return;
			}
			auto dx12SBuffer = static_cast<DirectX12StructuredBuffer*>(sb.get());
			commandList->BindStructuredBuffer(data->RootParm, dx12SBuffer->GetBufferID(), dx12SBuffer->Get());
		});
		mShaderData->ForEach_RWSB([&](const ShaderDataForm::StructuredBufferData* data, SharedPtr<IStructuredBuffer> rwsb)
		{
			if (rwsb == nullptr)
			{
				return;
			}
			auto dx12SBuffer = static_cast<DirectX12StructuredBuffer*>(rwsb.get());
			commandList->BindStructuredBuffer(data->RootParm, dx12SBuffer->GetBufferID(), dx12SBuffer->Get());
		});



		D3D12_CPU_DESCRIPTOR_HANDLE nullTextureHandle = { ITexture::NullTexture()->GetTextureID() };

		List<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
		mShaderData->ForEach_Tex([&](const ShaderDataForm::TextureData* data, const List<SharedPtr<ITexture>>& textureList)
		{
			u64 textureCount = textureList.size();
			for (u64 i = 0; i < textureCount; ++i)
			{
				if (textureList[i] != nullptr && textureList[i]->IsValid())
				{
					handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetSRV());
				}
				else
				{
					handles.push_back(nullTextureHandle);
				}
			}
		});
		if (handles.empty() == false)
		{
			commandList->BindTextures((u32)ShaderDefine::EComputeRootParam::TEXTURE2D, handles);
		}
		handles.clear();



		mShaderData->ForEach_RWTex([&](const ShaderDataForm::TextureData* data, const List<SharedPtr<ITexture>>& textureList)
		{
			u64 textureCount = textureList.size();
			for (u64 i = 0; i < textureCount; ++i)
			{
				if (textureList[i] != nullptr && textureList[i]->IsValid())
				{
					handles.push_back(static_cast<DirectX12Texture*>(textureList[i].get())->GetUAV());
				}
			}
		});

		if (handles.empty() == false)
		{
			commandList->BindTextures((u32)ShaderDefine::EComputeRootParam::RWTEXTURE2D, handles);
		}
		handles.clear();


		mShaderData->ForEach_BAB([&](const ShaderDataForm::ByteAddressData* data, SharedPtr<IByteAddressBuffer> byteAddrBuffer)
		{
			if (byteAddrBuffer != nullptr && byteAddrBuffer->IsValid())
			{
				handles.push_back(static_cast<DirectX12ByteAddressBuffer*>(byteAddrBuffer.get())->GetSRV());
			}
		});
		if (handles.empty() == false)
		{
			commandList->BindTextures((u32)ShaderDefine::EComputeRootParam::BYTEADDRESSBUFFER, handles);
		}
		handles.clear();


		mShaderData->ForEach_RWBAB([&](const ShaderDataForm::ByteAddressData* data, SharedPtr<IByteAddressBuffer> byteAddrBuffer)
		{
			if (byteAddrBuffer != nullptr && byteAddrBuffer->IsValid())
			{
				handles.push_back(static_cast<DirectX12ByteAddressBuffer*>(byteAddrBuffer.get())->GetUAV());
			}
		});
		if (handles.empty() == false)
		{
			commandList->BindTextures((u32)ShaderDefine::EComputeRootParam::RWBYTEADDRESSBUFFER, handles);
		}
		handles.clear();


		auto PSO = DirectX12API::GetComputePipelineState();
		PSO->BindRootSignature(*RootSig);
		PSO->BindShader(*(static_cast<DirectX12ComputeShader*>(mOwnerShader.get())));
		if (PSO->Finalize() == false)
		{
			return false;
		}
		commandList->BindPipelineState(PSO);
		commandList->Dispatch(groupX, groupY, groupZ);
		return true;
	}
}