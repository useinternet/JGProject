#pragma once
#include "JGCore.h"


namespace JG
{
	class ITexture;
	class IStructuredBuffer;
	class IComputeShader;
	class IByteAddressBuffer;
	class IComputer
	{
	public:
		virtual ~IComputer() = default;
	public:
		virtual bool SetFloat(const String& name, float value) = 0;
		virtual bool SetFloat2(const String& name, const JVector2& value) = 0;
		virtual bool SetFloat3(const String& name, const JVector3& value) = 0;
		virtual bool SetFloat4(const String& name, const JVector4& value) = 0;
		virtual bool SetInt(const String& name, i32 value) = 0;
		virtual bool SetInt2(const String& name, const JVector2Int& value) = 0;
		virtual bool SetInt3(const String& name, const JVector3Int& value) = 0;
		virtual bool SetInt4(const String& name, const JVector4Int& value) = 0;
		virtual bool SetUint(const String& name, u32 value) = 0;
		virtual bool SetUint2(const String& name, const JVector2Uint& value) = 0;
		virtual bool SetUint3(const String& name, const JVector3Uint& value) = 0;
		virtual bool SetUint4(const String& name, const JVector4Uint& value) = 0;
		virtual bool SetFloat4x4(const String& name, const JMatrix& value) = 0;
		virtual bool SetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture> texture) = 0;
		virtual bool SetByteAddressBuffer(const String& name, SharedPtr<IByteAddressBuffer> bab) = 0;
		virtual bool SetStructuredBuffer(const String& name, SharedPtr<IStructuredBuffer> sb) = 0;
		virtual bool GetFloat(const String& name, float* out_value) = 0;
		virtual bool GetFloat2(const String& name, JVector2* out_value) = 0;
		virtual bool GetFloat3(const String& name, JVector3* out_value) = 0;
		virtual bool GetFloat4(const String& name, JVector4* out_value) = 0;
		virtual bool GetInt(const String& name, i32* out_value) = 0;
		virtual bool GetInt2(const String& name, JVector2Int* value) = 0;
		virtual bool GetInt3(const String& name, JVector3Int* value) = 0;
		virtual bool GetInt4(const String& name, JVector4Int* value) = 0;
		virtual bool GetUint(const String& name, u32* value) = 0;
		virtual bool GetUint2(const String& name, JVector2Uint* value) = 0;
		virtual bool GetUint3(const String& name, JVector3Uint* value) = 0;
		virtual bool GetUint4(const String& name, JVector4Uint* value) = 0;
		virtual bool GetFloat4x4(const String& name, JMatrix* out_value) = 0;
		virtual bool GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value) = 0;

		virtual SharedPtr<IByteAddressBuffer> GetByteAddressBuffer(const String& name) = 0;
		virtual SharedPtr<IStructuredBuffer> GetStructuredBuffer(const String& name) = 0;
	public:
		virtual const String& GetName() const = 0;
		virtual void SetName(const String& name) = 0;
		virtual bool Dispatch(u64 commandID, u32 groupX, u32 groupY, u32 groupZ, bool asComputeCommand = true) = 0;
	public:
		static SharedPtr<IComputer> Create(const String& name, SharedPtr<IComputeShader> shader);
	};
}