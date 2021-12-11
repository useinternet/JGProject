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
		SharedPtr<IGraphicsShader> mGraphicsShader;
		D3D12_BLEND_DESC           mBlendDesc;
		D3D12_DEPTH_STENCIL_DESC   mDepthStencilDesc;
		D3D12_RASTERIZER_DESC      mRasterzerDesc;


		Dictionary<String, List<jbyte>>		    mCBDatas;
		Dictionary<String, SharedPtr<ITexture>> mTextureDic;
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
		virtual void  SetShader(SharedPtr<IGraphicsShader> shader) override;

		virtual List<std::pair<EShaderDataType, String>> GetPropertyList() const override;
	public:
		virtual bool Bind(u64 commandID) override;
		void Init(SharedPtr<IGraphicsShader> shader);
	private:
		template<class T>
		bool SetData(const String& name, const T& value)
		{
			// 적기

			return true;
		}
		template<class T>
		bool GetData(const String& name, T* out_value)
		{
			// 적기

			return true;
		}


	public:
		virtual ~DirectX12Material() = default;
	};

}