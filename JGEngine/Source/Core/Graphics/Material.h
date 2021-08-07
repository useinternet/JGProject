#pragma once
#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"

namespace JG
{
	class IShader;
	class ITexture;
	class IShaderScript;
	class IMaterial : public GraphicsCommandable
	{
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



		virtual bool SetFloatArray(const String& name, const List<float>& value) = 0;
		virtual bool SetFloat2Array(const String& name, const List<JVector2>& value) = 0;
		virtual bool SetFloat3Array(const String& name, const List<JVector3>& value) = 0;
		virtual bool SetFloat4Array(const String& name, const List<JVector4>& value) = 0;
		virtual bool SetIntArray(const String& name, const List<i32>& value) = 0;
		virtual bool SetInt2Array(const String& name, const List<JVector2Int>& value) = 0;
		virtual bool SetInt3Array(const String& name, const List<JVector3Int>& value) = 0;
		virtual bool SetInt4Array(const String& name, const List<JVector4Int>& value) = 0;
		virtual bool SetUintArray(const String& name, const List<u32>& value) = 0;
		virtual bool SetUint2Array(const String& name, const List<JVector2Uint>& value) = 0;
		virtual bool SetUint3Array(const String& name, const List<JVector3Uint>& value) = 0;
		virtual bool SetUint4Array(const String& name, const List<JVector4Uint>& value) = 0;
		virtual bool SetFloat4x4Array(const String& name, const List<JMatrix>& value) = 0;
		virtual bool SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize) = 0;





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

		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) = 0;
		virtual void SetBlendState(u32 slot, EBlendStateTemplate _template) = 0;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) = 0;


		virtual void SetName(const String& name) = 0;
		virtual const String& GetName() const = 0;

		virtual List<std::pair<EShaderDataType, String>> GetPropertyList() const = 0;
	protected:
		virtual void Init(SharedPtr<IShader> shader) = 0;
	public:
		virtual bool Bind() = 0;
	public:
		virtual ~IMaterial() = default;
	public:
		static SharedPtr<IMaterial> Create(const String& name, SharedPtr<IShader> shader);

	};
}