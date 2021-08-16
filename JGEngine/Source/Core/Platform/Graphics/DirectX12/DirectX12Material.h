#pragma once
#include "Graphics/Material.h"
#include "DirectX12Shader.h"


namespace JG
{
	class ShaderData;
	class DirectX12Material : public IMaterial
	{
	private:
		String mName;
		UniquePtr<ShaderData>    mShaderData;
		D3D12_BLEND_DESC         mBlendDesc;
		D3D12_DEPTH_STENCIL_DESC mDepthStencilDesc;
		D3D12_RASTERIZER_DESC    mRasterzerDesc;
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

		virtual bool SetFloatArray(const String& name, const List<float>& value) override;
		virtual bool SetFloat2Array(const String& name, const List<JVector2>& value) override;
		virtual bool SetFloat3Array(const String& name, const List<JVector3>& value) override;
		virtual bool SetFloat4Array(const String& name, const List<JVector4>& value) override;
		virtual bool SetIntArray(const String& name, const List<i32>& value) override;
		virtual bool SetInt2Array(const String& name, const List<JVector2Int>& value) override;
		virtual bool SetInt3Array(const String& name, const List<JVector3Int>& value) override;
		virtual bool SetInt4Array(const String& name, const List<JVector4Int>& value) override;
		virtual bool SetUintArray(const String& name, const List<u32>& value) override;
		virtual bool SetUint2Array(const String& name, const List<JVector2Uint>& value) override;
		virtual bool SetUint3Array(const String& name, const List<JVector3Uint>& value) override;
		virtual bool SetUint4Array(const String& name, const List<JVector4Uint>& value) override;
		virtual bool SetFloat4x4Array(const String& name, const List<JMatrix>& value) override;
		virtual bool SetStructDataArray(const String& name, void* datas, u64 elementCount, u64 elementSize) override;


		virtual bool GetFloat(const String& name, float* out_value) override;
		virtual bool GetFloat2(const String& name, JVector2* out_value) override;
		virtual bool GetFloat3(const String& name, JVector3* out_value) override;
		virtual bool GetFloat4(const String& name, JVector4* out_value) override;
		virtual bool GetInt(const String& name, i32* out_value) override;
		virtual bool GetInt2(const String& name, JVector2Int* out_value) override;
		virtual bool GetInt3(const String& name, JVector3Int* out_value) override;
		virtual bool GetInt4(const String& name, JVector4Int* out_value) override;
		virtual bool GetUint(const String& name, u32* out_value) override;
		virtual bool GetUint2(const String& name, JVector2Uint* out_value) override;
		virtual bool GetUint3(const String& name, JVector3Uint* out_value) override;
		virtual bool GetUint4(const String& name, JVector4Uint* out_value) override;
		virtual bool GetFloat4x4(const String& name, JMatrix* outValue) override;
		virtual bool GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value) override;


		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) override;
		virtual void SetBlendState(u32 slot, EBlendStateTemplate _template) override;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) override;

		virtual void  SetName(const String& name) override;
		virtual const String& GetName() const override;
		virtual void  SetShader(SharedPtr<IShader> shader) override;

		virtual List<std::pair<EShaderDataType, String>> GetPropertyList() const override;
	public:
		virtual bool Bind() override;
		virtual void Init(SharedPtr<IShader> shader) override;
	public:
		virtual ~DirectX12Material() = default;
	};

}