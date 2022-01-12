#pragma once
#include "JGCore.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/ShaderDataForm.h"
#include "Graphics/Computer.h"

namespace JG
{
	class ComputeCommandList;
	class DirectX12Computer : public IComputer
	{
	private:
		String         mName;
		SharedPtr<IComputeShader> mOwnerShader;
		UniquePtr<ShaderData>     mShaderData;
	public:
		virtual ~DirectX12Computer() = default;
	public:
		virtual bool SetFloat(const String& name, float value) override;
		virtual bool SetFloat2(const String& name, const JVector2& value) override;
		virtual bool SetFloat3(const String& name, const JVector3& value) override;
		virtual bool SetFloat4(const String& name, const JVector4& value) override;
		virtual bool SetInt(const String& name, i32 value) override;
		virtual bool SetInt2(const String& name, const JVector2Int& value) override;
		virtual bool SetInt3(const String& name, const JVector3Int& value) override;
		virtual bool SetInt4(const String& name, const JVector4Int& value) override;
		virtual bool SetUint(const String& name, u32 value) override;
		virtual bool SetUint2(const String& name, const JVector2Uint& value) override;
		virtual bool SetUint3(const String& name, const JVector3Uint& value) override;
		virtual bool SetUint4(const String& name, const JVector4Uint& value) override;
		virtual bool SetFloat4x4(const String& name, const JMatrix& value) override;
		virtual bool SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture) override;
		virtual bool SetByteAddressBuffer(const String& name, SharedPtr<IByteAddressBuffer> bab) override;
		virtual bool SetStructuredBuffer(const String& name, SharedPtr<IStructuredBuffer> sb) override;

		virtual bool GetFloat(const String& name, float* out_value) override;
		virtual bool GetFloat2(const String& name, JVector2* out_value) override;
		virtual bool GetFloat3(const String& name, JVector3* out_value) override;
		virtual bool GetFloat4(const String& name, JVector4* out_value) override;
		virtual bool GetInt(const String& name, i32* out_value) override;
		virtual bool GetInt2(const String& name, JVector2Int* value) override;
		virtual bool GetInt3(const String& name, JVector3Int* value) override;
		virtual bool GetInt4(const String& name, JVector4Int* value) override;
		virtual bool GetUint(const String& name, u32* value) override;
		virtual bool GetUint2(const String& name, JVector2Uint* value) override;
		virtual bool GetUint3(const String& name, JVector3Uint* value) override;
		virtual bool GetUint4(const String& name, JVector4Uint* value) override;
		virtual bool GetFloat4x4(const String& name, JMatrix* out_value) override;
		virtual bool GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value) override;

		virtual SharedPtr<IByteAddressBuffer> GetByteAddressBuffer(const String& name) override;
		virtual SharedPtr<IStructuredBuffer> GetStructuredBuffer(const String& name) override;
	public:
		void SetComputeShader(SharedPtr<IComputeShader> shader);
		virtual const String& GetName() const override;
		virtual void  SetName(const String& name) override;
		virtual bool Dispatch(u64 commandID, u32 groupX, u32 groupY, u32 groupZ, bool asComputeCommand) override;
	private:
		bool DispatchInternal(u64 commandID, ComputeCommandList* commandList, u32 groupX, u32 groupY, u32 groupZ);
	};
}