#pragma once
#include "Graphics/Material.h"
#include "DirectX12Shader.h"


namespace JG
{
	class ShaderData;
	class DirectX12Material : public IMaterial
	{
		using CBPair = std::pair<u64, u64>;
	private:
		String mName;
		SharedPtr<IGraphicsShader> mGraphicsShader;
		

		D3D12_BLEND_DESC           mBlendDesc;
		D3D12_DEPTH_STENCIL_DESC   mDepthStencilDesc;
		D3D12_RASTERIZER_DESC      mRasterzerDesc;

		List<jbyte> mBtData;
		List<jbyte> mUploadBtData;
		std::mutex  mMutex;


		Dictionary<String, CBPair> mCBDatas;
		Dictionary<String, SharedPtr<ITexture>> mTextures;
		//Dictionary<String, SharedPtr<ITexture>> mTextureCubes;
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
		virtual bool SetTexture(const String& name, SharedPtr<ITexture> texture) override;

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
		virtual bool GetTexture(const String& name, SharedPtr<ITexture>* out_value) override;
	
		virtual void SetDepthStencilState(EDepthStencilStateTemplate _template) override;
		virtual void SetBlendState(u32 slot, EBlendStateTemplate _template) override;
		virtual void SetRasterizerState(ERasterizerStateTemplate _template) override;

		virtual void  SetName(const String& name) override;
		virtual const String& GetName() const override;
		virtual SharedPtr<IGraphicsShader> GetShader() const override;
		virtual void  SetShader(SharedPtr<IGraphicsShader> shader) override;

		virtual const List<std::pair<EShaderDataType, String>>& GetPropertyList() const override;
		virtual SharedPtr<IShaderScript> GetScript() const override;
	public:
		virtual bool IsValid() const override;
		virtual List<SharedPtr<ITexture>> GetTextureList()	  const override;
		//virtual List<SharedPtr<ITexture>> GetCubeTextureList() const override;
		virtual const List<jbyte>& GetMaterialPropertyByteData() override;




		// 나중에 공용 Desc 로 변경 일단 Bind로 하자..(나중에 변경)
		virtual bool Bind() override;
		void Init(SharedPtr<IGraphicsShader> shader);
	private:
		template<class T>
		bool SetData(const String& name, const T& value)
		{
			// 적기
			if (mCBDatas.find(name) == mCBDatas.end())
			{
				return false;
			}

			u64 dataPos   = mCBDatas[name].first;
			u64 cbSize	  = mCBDatas[name].second;
			u64 valueSize = sizeof(T);

			if (cbSize != valueSize)
			{
				return false;
			}
			void* dataPtr = (void*)(mBtData.data() + dataPos);

			std::lock_guard<std::mutex> lock(mMutex);
			memcpy(dataPtr, &value, cbSize);
			return true;
		}
		template<class T>
		bool GetData(const String& name, T* out_value)
		{
			//
			if (mCBDatas.find(name) == mCBDatas.end())
			{
				return false;
			}
			u64 dataPos = mCBDatas[name].first;
			u64 cbSize = mCBDatas[name].second;
			u64 valueSize = sizeof(T);

			if (cbSize != valueSize)
			{
				return false;
			}
			void* dataPtr = (void*)(mBtData.data() + dataPos);
			memcpy(out_value, dataPtr, cbSize);
			return true;
		}


	public:
		virtual ~DirectX12Material() = default;
	};

}