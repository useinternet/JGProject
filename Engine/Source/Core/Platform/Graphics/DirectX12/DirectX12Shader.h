#pragma once
#include "Graphics/Shader.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/ShaderDataForm.h"
namespace JG
{
	class IComputeBuffer;
	class GraphicsPipelineState;
	class RootSignature;
	class ITexture;
	class ShaderDataForm;


	struct CompileConfig
	{
		String Entry;
		String Target;
		CompileConfig(const String& entry, const String& target) : Entry(entry), Target(target) {}
	};

	class DirectX12GraphicsShader : public IGraphicsShader
	{
		ComPtr<ID3DBlob> mVSData;
		ComPtr<ID3DBlob> mDSData;
		ComPtr<ID3DBlob> mHSData;
		ComPtr<ID3DBlob> mGSData;
		ComPtr<ID3DBlob> mPSData;
		EShaderFlags     mFlags;
		String mName;
		String mSourceCode;
		String mFullSourceCode;
		bool mIsCompileSuccess = false;
		List<std::pair<EShaderDataType, String>> mPropertyList;
		List<SharedPtr<IShaderScript>>			 mShaderScriptList;

		SortedDictionary<u64, String> mSortedTextureMap;
		SortedDictionary<u64, String> mSortedTextureCubeMap;
	public:
		const String& GetName() const override;
		void SetName(const String& name);


		virtual bool  Compile(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags, String* error) override;
		virtual const String& GetShaderCode() const override;
		virtual const String& GetFullShaderCode() const override;
		virtual EShaderFlags  GetFlags() const override;
		virtual bool IsSuccessed() const override;
		virtual const List<std::pair<EShaderDataType, String>>& GetPropertyList() const override;
		virtual const List<SharedPtr<IShaderScript>>& GetScriptList() const override;
	public:
		void ForEach_TextureSlot(const std::function<void(const String&)>& action);
		void ForEach_TextureCubeSlot(const std::function<void(const String&)>& action);
	public:
		ID3DBlob* GetVSData() const {
			return mVSData.Get();
		}
		ID3DBlob* GetDSData() const {
			return mDSData.Get();
		}
		ID3DBlob* GetHSData() const {
			return mHSData.Get();
		}
		ID3DBlob* GetGSData() const {
			return mGSData.Get();
		}
		ID3DBlob* GetPSData() const {
			return mPSData.Get();
		}
	private:
		bool Compile(const String& code, String* error);
		bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
		void InsertScript(String& code, SharedPtr<IShaderScript> script);
		bool InsertScriptInternal(String& code, SharedPtr<IShaderScript> script);
		bool ExtractScriptContents(const String& code, const String& key, String& out_code);
	};


	class DirectX12ComputeShader : public IComputeShader
	{
	private:
		ComPtr<ID3DBlob> mCSData;
		String mName;
		String			 mSourceCode;
		bool			 mIsCompileSuccess = false;
		SharedPtr<ShaderDataForm> mShaderDataForm;
	public:
		const String& GetName() const override;
		void SetName(const String& name);
		virtual bool  Compile(const String& sourceCode, String* error) override;
		virtual const String& GetShaderCode() const override;
		virtual bool  IsSuccessed() const override;
	public:
		SharedPtr<ShaderDataForm> GetShaderDataForm() const {
			return mShaderDataForm;
		}
		ID3DBlob* GetCSData() const {
			return mCSData.Get();
		}
	private:
		bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	};

}

