#include "pch.h"
#include "DirectX12RayTracingShaderResourceTable.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/DirectX12Resource.h"
#include "Platform/Graphics/DirectX12/Utill/CommandList.h"
#ifndef ROUND_UP
#define ROUND_UP(v, powerOf2Alignment)                                         \
  (((v) + (powerOf2Alignment)-1) & ~((powerOf2Alignment)-1))
#endif
namespace JG
{
	void DirectX12RayTracingShaderResourceTable::AddRayGeneration(const String& entryPoint)
	{
		mRayGenLocalRootArgInfos.push_back(LocalRootArgumentInfo(entryPoint));
	}
	void DirectX12RayTracingShaderResourceTable::AddMiss(const String& entryPoint)
	{
		mMissLocalRootArgInfos.push_back(LocalRootArgumentInfo(entryPoint));
	}
	void DirectX12RayTracingShaderResourceTable::AddHitGroupAndBindLocalRootArgument(const String& entryPoint, const LocalRootArgument& localRootArg)
	{
		mHitLocalRootArgInfos.push_back(LocalRootArgumentInfo(entryPoint, localRootArg));
	}
	void DirectX12RayTracingShaderResourceTable::Reset()
	{
		mRayGenLocalRootArgInfos.clear();
		mMissLocalRootArgInfos.clear();
		mHitLocalRootArgInfos.clear();

		mRayGenSectionSize = 0;
		mRayEntrySize = 0;
		mMissSectionSize = 0;
		mMissEntrySize = 0;
		mHitSectionSize = 0;
		mHitEntrySize = 0;
	}

	u64 DirectX12RayTracingShaderResourceTable::GetRayGenStartAddr() const
	{
		return mRayGenSRT->GetGPUVirtualAddress();
	}
	u64 DirectX12RayTracingShaderResourceTable::GetRayGenSectionSize() const
	{
		return mRayGenSectionSize;
	}
	u64 DirectX12RayTracingShaderResourceTable::GetMissStartAddr() const
	{
		return mMissSRT->GetGPUVirtualAddress();
	}
	u64 DirectX12RayTracingShaderResourceTable::GetMissSectionSize() const
	{
		return mMissSectionSize;
	}
	u64 DirectX12RayTracingShaderResourceTable::GetMissEntrySize() const
	{
		return mMissEntrySize;
	}
	u64 DirectX12RayTracingShaderResourceTable::GetHitGroupStartAddr() const
	{
		return mHitSRT->GetGPUVirtualAddress();
	}
	u64 DirectX12RayTracingShaderResourceTable::GetHitGroupSectionSize() const
	{
		return mHitSectionSize;
	}
	u64 DirectX12RayTracingShaderResourceTable::GetHitGroupEntrySize() const
	{
		return mHitEntrySize;
	}
	void DirectX12RayTracingShaderResourceTable::Generate(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties)
	{
		UpdateRayGenSRT(commandList, pipelineProperties);
		UpdateMissSRT(commandList, pipelineProperties);
		UpdateHitSRT(commandList, pipelineProperties);
	}

	u64 DirectX12RayTracingShaderResourceTable::GetEntrySize(const List<LocalRootArgumentInfo>& argInfos)
	{
		u64 maxArgSize = 0;
		for (const auto& info : argInfos)
		{
			maxArgSize = Math::Max(maxArgSize, info.Argument.GetArgumentSize());
		}
		u32 entrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + static_cast<u32>(maxArgSize);
		entrySize = ROUND_UP(entrySize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

		return entrySize;
	}

	void DirectX12RayTracingShaderResourceTable::UpdateRayGenSRT(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties)
	{
		// Create SRT Resource
		mRayEntrySize = GetEntrySize(mRayGenLocalRootArgInfos);


		mRayGenSectionSize = mRayEntrySize * (mRayGenLocalRootArgInfos.size());
		if (mRayGenSRT == nullptr || mRayGenSectionSize >= mRayGenSRTSize)
		{
			mRayGenSRTSize = mRayGenSectionSize;
			if (mRayGenSRT != nullptr)
			{
				mRayGenSRT->Unmap(0, nullptr);
				DirectX12API::DestroyCommittedResource(mRayGenSRT);
				mRayGenSRTSize *= 2;
			}
			
			mRayGenSRT = DirectX12API::CreateCommittedResource(
				"RayGenSRT_Buffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mRayGenSRTSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr);

			mRayGenSRT->Map(0, nullptr, &mRayGenSRTPointer);
		}
		if (mRayGenSRTPointer == nullptr)
		{

			return;
		}

		u8* pData = (u8*)mRayGenSRTPointer;
		for (LocalRootArgumentInfo& info : mRayGenLocalRootArgInfos)
		{
			const std::wstring& entryPoint = GetEntryPoint_w(info.EntryPoint);

			void* id = pipelineProperties->GetShaderIdentifier(entryPoint.c_str());
			if (id == nullptr)
			{
				std::wstring errMsg(std::wstring(L"Unknown shader identifier used in the SBT: ") +
					entryPoint);
				throw std::logic_error(std::string(errMsg.begin(), errMsg.end()));
			}
			memcpy(pData, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
			pData += mRayEntrySize;
		}
	}

	void DirectX12RayTracingShaderResourceTable::UpdateMissSRT(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties)
	{
		// Create SRT Resource
		mMissEntrySize = GetEntrySize(mMissLocalRootArgInfos);
		mMissSectionSize = mMissEntrySize * (mMissLocalRootArgInfos.size());
		if (mMissSRT == nullptr || mMissSectionSize >= mMissSRTSize)
		{
			mMissSRTSize = mMissSectionSize;
			if (mMissSRT != nullptr)
			{
				mMissSRT->Unmap(0, nullptr);
				DirectX12API::DestroyCommittedResource(mMissSRT);
				mMissSRTSize *= 2;
			}

			mMissSRT = DirectX12API::CreateCommittedResource(
				"MissSRT_Buffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mMissSRTSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr);

			mMissSRT->Map(0, nullptr, &mMissSRTPointer);
		}
		if (mMissSRTPointer == nullptr)
		{

			return;
		}

		u8* pData = (u8*)mMissSRTPointer;
		for (LocalRootArgumentInfo& info : mMissLocalRootArgInfos)
		{
			const std::wstring& entryPoint = GetEntryPoint_w(info.EntryPoint);

			void* id = pipelineProperties->GetShaderIdentifier(entryPoint.c_str());
			if (id == nullptr)
			{
				std::wstring errMsg(std::wstring(L"Unknown shader identifier used in the SBT: ") +
					entryPoint);
				throw std::logic_error(std::string(errMsg.begin(), errMsg.end()));
			}
			memcpy(pData, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
			pData += mMissEntrySize;
		}
	}

	void DirectX12RayTracingShaderResourceTable::UpdateHitSRT(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties)
	{
		// Create SRT Resource
		mHitEntrySize	= GetEntrySize(mHitLocalRootArgInfos);
		mHitSectionSize = mHitEntrySize * mHitLocalRootArgInfos.size();


		if (mHitSRT == nullptr || mHitSectionSize >= mHitSRTSize)
		{
			if (mHitSRT != nullptr)
			{
				mHitSRT->Unmap(0, nullptr);
				DirectX12API::DestroyCommittedResource(mHitSRT);
				mHitSRTSize *= 2;
			}

			mHitSRT = DirectX12API::CreateCommittedResource(
				"HitSRT_Buffer",
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mHitSRTSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr);

			mHitSRT->Map(0, nullptr, &mHitSRTPointer);
		}
		if (mHitSRTPointer == nullptr)
		{

			return;
		}

		u8* pData = (u8*)mHitSRTPointer;

		for (const LocalRootArgumentInfo& info : mHitLocalRootArgInfos)
		{
			const std::wstring& entryPoint = GetEntryPoint_w(info.EntryPoint);
			void* id = pipelineProperties->GetShaderIdentifier(entryPoint.c_str());
			if (id == nullptr)
			{
				std::wstring errMsg(std::wstring(L"Unknown shader identifier used in the SBT: ") +
					entryPoint);
				throw std::logic_error(std::string(errMsg.begin(), errMsg.end()));
			}
			u64 offset = 0;
			memcpy(pData, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
			offset += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;



			// Constant
			memcpy(pData + offset, info.Argument.GetConstant().data(), info.Argument.GetConstant().size());
			offset += info.Argument.GetConstant().size();


			// SRV, VertexBuffer, IndexBuffer
			List<D3D12_GPU_DESCRIPTOR_HANDLE> gpuHandles;
			D3D12_GPU_DESCRIPTOR_HANDLE nullHandle = commandList->UploadDirect({ ITexture::NullTexture()->GetTextureID() });

			if (info.Argument.GetVertexBuffer() != nullptr)
			{
				DirectX12VertexBuffer* dx12VBuffer = static_cast<DirectX12VertexBuffer*>(info.Argument.GetVertexBuffer().get());
				D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = commandList->UploadDirect(dx12VBuffer->GetSRV());
				gpuHandles.push_back(gpuHandle);
			}
			// Prev VertexBuffer
			if (info.Argument.GetVertexBuffer() != nullptr)
			{
				DirectX12VertexBuffer* dx12VBuffer = static_cast<DirectX12VertexBuffer*>(info.Argument.GetVertexBuffer().get());
				D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = commandList->UploadDirect(dx12VBuffer->GetSRV());
				gpuHandles.push_back(gpuHandle);
			}
			if (info.Argument.GetIndexBuffer() != nullptr)
			{
				DirectX12IndexBuffer* dx12IBuffer = static_cast<DirectX12IndexBuffer*>(info.Argument.GetIndexBuffer().get());
				D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = commandList->UploadDirect(dx12IBuffer->GetSRV());
				gpuHandles.push_back(gpuHandle);
			}


			for (SharedPtr<ITexture> t : info.Argument.GetTextures())
			{
				if (t == nullptr || t->IsValid() == false)
				{
					gpuHandles.push_back(nullHandle);
				}
				else
				{
					D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = commandList->UploadDirect({ t->GetTextureID() });
					gpuHandles.push_back(gpuHandle);
				}
			}
			memcpy(pData + offset, gpuHandles.data(), sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * gpuHandles.size());
			pData += mHitEntrySize;
		}
	}

	const std::wstring& DirectX12RayTracingShaderResourceTable::GetEntryPoint_w(const String& entrypoint)
	{
		if (mEntryPointNameDic.find(entrypoint) == mEntryPointNameDic.end())
		{
			mEntryPointNameDic[entrypoint] = StringHelper::s2ws(entrypoint);
		}
		return mEntryPointNameDic[entrypoint];
	}


}