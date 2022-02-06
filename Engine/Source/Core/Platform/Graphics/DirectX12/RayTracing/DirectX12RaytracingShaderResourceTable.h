#pragma once
#include "Graphics/RayTracing/RayTracingShaderResourceTable.h"
#include "Platform/Graphics/DirectX12/Utill/DirectX12Helper.h"
#include "Platform/Graphics/DirectX12/Utill/RayTracingHelper.h"


namespace JG
{
	class DirectX12RayTracingShaderResourceTable : public IRayTracingShaderResourceTable
	{
		struct LocalRootArgumentInfo
		{
			String EntryPoint;
			LocalRootArgument Argument;

			LocalRootArgumentInfo(const String& entryPoint, const LocalRootArgument& arg = LocalRootArgument()) :
				EntryPoint(entryPoint), Argument(arg) {}
		};
		struct InputData
		{
			const void* DataPointer = nullptr;
			const u64 DataSize = 0;
		};
	private:
		List<LocalRootArgumentInfo> mRayGenLocalRootArgInfos;
		List<LocalRootArgumentInfo> mMissLocalRootArgInfos;
		List<LocalRootArgumentInfo> mHitLocalRootArgInfos;

		ComPtr<ID3D12Resource> mRayGenSRT;
		ComPtr<ID3D12Resource> mMissSRT;
		ComPtr<ID3D12Resource> mHitSRT;

		void* mRayGenSRTPointer = nullptr;
		void* mMissSRTPointer = nullptr;
		void* mHitSRTPointer = nullptr;


		u64 mRayGenSRTSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		u64 mRayGenSectionSize = 0;
		u64 mRayEntrySize = 0;


		u64 mMissSRTSize   = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		u64 mMissSectionSize   = 0;
		u64 mMissEntrySize     = 0;
		u64 mHitSRTSize	   = _2MB;
		u64 mHitSectionSize    = 0;
		u64 mHitEntrySize      = 0;

		Dictionary<String, std::wstring> mEntryPointNameDic;
	public:
		virtual ~DirectX12RayTracingShaderResourceTable() = default;
	public:
		virtual void AddRayGeneration(const String& entryPoint) override;
		virtual void AddMiss(const String& entryPoint) override;
		virtual void AddHitGroupAndBindLocalRootArgument(const String& entryPoint, const LocalRootArgument& localRootArg) override;
		virtual void Reset() override;

		u64 GetRayGenStartAddr() const;
		u64 GetRayGenSectionSize() const;
		u64 GetMissStartAddr() const;
		u64 GetMissSectionSize() const;
		u64 GetMissEntrySize() const;
		u64 GetHitGroupStartAddr() const;
		u64 GetHitGroupSectionSize() const;
		u64 GetHitGroupEntrySize() const;

		ID3D12Resource* GetRayGenSRT() const {
			return mRayGenSRT.Get();
		}
		ID3D12Resource* GetMissSRT() const {
			return mMissSRT.Get();
		}
		ID3D12Resource* GetHitSRT() const {
			return mHitSRT.Get();
		}
		void Generate(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties);
	private:
		u64 GetEntrySize(const List<LocalRootArgumentInfo>& argInfos);
		void UpdateRayGenSRT(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties);
		void UpdateMissSRT(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties);
		void UpdateHitSRT(CommandList* commandList, ID3D12StateObjectProperties* pipelineProperties);

		const std::wstring& GetEntryPoint_w(const String& entrypoint);
	};



}