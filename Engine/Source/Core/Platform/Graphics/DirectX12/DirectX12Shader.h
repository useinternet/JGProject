#pragma once
#include "Graphics/Shader.h"
#include "Utill/DirectX12Helper.h"
namespace JG
{
	class IComputeBuffer;
	class GraphicsPipelineState;
	class RootSignature;
	class ITexture;
	class ShaderScriptCodeAnalyzer;

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
		List<SharedPtr<IShaderScript>>	mShaderScriptList;

		UniquePtr<ShaderScriptCodeAnalyzer> mScriptCodeAnalyzer;
		String mName;
		String mSourceCode;
		String mFullSourceCode;
		String mErrorMessage;
		bool mIsCompileSuccess = false;

	public:
		const String& GetName() const override;
	

		virtual const String& GetShaderCode() const override;
		virtual const String& GetFullShaderCode() const override;
		virtual const String& GetErrorMessage()   const override;
		virtual EShaderFlags  GetFlags() const override;
		virtual bool IsSuccessed() const override;
		virtual const List<std::pair<EShaderDataType, String>>& GetPropertyList() const override;
		virtual const List<SharedPtr<IShaderScript>>& GetScriptList() const override;
	public:
		void SetName(const String& name);
		bool Init(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags);
		void ForEach_TextureSlot(const std::function<void(const String&)>& action);

		ID3DBlob* GetVSData() const {
			if (IsSuccessed() == false)
			{
				return nullptr;
			}
			return mVSData.Get();
		}
		ID3DBlob* GetDSData() const {
			if (IsSuccessed() == false)
			{
				return nullptr;
			}
			return mDSData.Get();
		}
		ID3DBlob* GetHSData() const {
			if (IsSuccessed() == false)
			{
				return nullptr;
			}
			return mHSData.Get();
		}
		ID3DBlob* GetGSData() const {
			if (IsSuccessed() == false)
			{
				return nullptr;
			}
			return mGSData.Get();
		}
		ID3DBlob* GetPSData() const {
			if (IsSuccessed() == false)
			{
				return nullptr;
			}
			return mPSData.Get();
		}
	private:
		bool Compile(const String& code, String* error);
		bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	};
	class DirectX12ComputeShader : public IComputeShader
	{
	private:
		ComPtr<IDxcBlob> mCSData;
		String mName;
		String mErrorMessage;
		String mSourceCode;
		bool   mIsCompileSuccess = false;
	public:
		virtual const String& GetName() const override;
		virtual const String& GetErrorMessage() const override;
		virtual const String& GetShaderCode() const override;
		virtual bool  IsSuccessed() const override;
	public:
		void SetName(const String& name);
		bool Init(const String& sourceCode);
		IDxcBlob* GetCSData() const {
			if (IsSuccessed() == false)
			{
				return nullptr;
			}
			return mCSData.Get();
		}
	private:
		bool Compile(ComPtr<IDxcBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	};



	class DirectX12ClosestHitShader : public IClosestHitShader
	{
		ComPtr<IDxcBlob> mBlobData;
		String mName;
		String mSourceCode;
		String mFullSourceCode;
		String mHitGroupName;
		String mEntryPoint;
		String mErrorMessage;
		bool   mIsCompileSuccess = false;
		UniquePtr<ShaderScriptCodeAnalyzer> mScriptCodeAnalyzer;
	public:
		virtual const String& GetName() const override;
		virtual const String& GetEntryPoint()   const override;
		virtual const String& GetHitGroupName() const override;
		virtual const String& GetShaderCode() const override;
		virtual const String& GetFullShaderCode() const override;
		virtual const String& GetErrorMessage() const override;
		virtual bool  IsSuccessed() const override;
	public:
		bool Init(const String& sourceCode, SharedPtr<IShaderScript> script);
		IDxcBlob* GetData() const {
			if (IsSuccessed() == false)
			{
				return nullptr;
			}
			return mBlobData.Get();
		}
		void SetName(const String& name);
	};



	class ShaderScriptCodeAnalyzer
	{
		List<std::pair<EShaderDataType, String>> mPropertyList;
		u64 mStartCBReigsterNum = 0;
		u64 mStartCBRegisterSpace = 0;
		u64 mStartTexRegisterNum = 0;
		u64 mStartTexRegisterSpace = 0;

		SortedDictionary<u64, String> mSortedTextureMap;
	public:
		ShaderScriptCodeAnalyzer(
			u64 start_cb_register_num, u64 start_cb_register_space,
			u64 start_tex_register_num, u64 start_tex_register_space) :
			mStartCBReigsterNum(start_cb_register_num), mStartCBRegisterSpace(start_cb_register_space),
			mStartTexRegisterNum(start_tex_register_num), mStartTexRegisterSpace(start_cb_register_space) {}
		void InsertScript(String& code, SharedPtr<IShaderScript> script);
		const List<std::pair<EShaderDataType, String>>& GetPropertyList() const;
		const SortedDictionary<u64, String>& GetSortedTextureMap() const;
	private:
		bool InsertScriptInternal(String& code, SharedPtr<IShaderScript> script);
		bool ExtractScriptContents(const String& code, const String& key, String& out_code);
	};
}

