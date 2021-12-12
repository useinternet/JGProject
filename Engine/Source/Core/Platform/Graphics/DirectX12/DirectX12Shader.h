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
	//class DirectX12Shader : public IShader
	//{
	//private:
	//	String mName;
	//	String mOriginCode;
	//	String mSourceCode;
	//	UniquePtr<ShaderDataForm> mShaderDataForm;
	//	ComPtr<ID3DBlob> mVSData;
	//	ComPtr<ID3DBlob> mDSData;
	//	ComPtr<ID3DBlob> mHSData;
	//	ComPtr<ID3DBlob> mGSData;
	//	ComPtr<ID3DBlob> mPSData;
	//	ComPtr<ID3DBlob> mCSData;
	//	EShaderFlags     mFlags;
	//	bool mIsCompileSuccess = false;
	//	List<std::pair<EShaderDataType, String>> mPropertyList;
	//public:
	//	virtual ~DirectX12Shader() = default;
	//public:
	//	virtual bool Compile(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags, String* error) override;
	//	virtual bool Bind(u64 commandID) override;
	//public:
	//	virtual void  SetName(const String& name) override;
	//	virtual const String& GetName() const override;
	//	virtual const String& GetOriginCode() const override;
	//	virtual EShaderFlags GetFlags() const override
	//	{
	//		return mFlags;
	//	}

	//private:
	//	bool GraphicsCompile(const String& code, String* error);
	//	void GraphicsBind(u64 commandID, SharedPtr<RootSignature> RootSig);
	//	bool ComputeCompile(const String& code, String* error);
	//	void ComputeBind(u64 commandID, SharedPtr<RootSignature> RootSig);

	//	void InsertScript(String& code, const List<SharedPtr<IShaderScript>>& scriptList);
	//	bool InsertScriptInternal(String& code, SharedPtr<IShaderScript> script);
	//	bool ExtractScriptContents(const String& code, const String& key, String& out_code);
	//public:
	//	ID3DBlob* GetVSData() const {
	//		return mVSData.Get();
	//	}
	//	ID3DBlob* GetDSData() const {
	//		return mDSData.Get();
	//	}
	//	ID3DBlob* GetHSData() const {
	//		return mHSData.Get();
	//	}
	//	ID3DBlob* GetGSData() const {
	//		return mGSData.Get();
	//	}
	//	ID3DBlob* GetPSData() const {
	//		return mPSData.Get();
	//	}
	//	ID3DBlob* GetCSData() const {
	//		return mCSData.Get();
	//	}
	//	ShaderDataForm* GetShaderDataForm() const {
	//		return mShaderDataForm.get();
	//	}
	//	const List<std::pair<EShaderDataType, String>>& GetPropertyList() const;
	//private:
	//	bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	//};

	class DirectX12GraphicsShader : public IGraphicsShader
	{
		ComPtr<ID3DBlob> mVSData;
		ComPtr<ID3DBlob> mDSData;
		ComPtr<ID3DBlob> mHSData;
		ComPtr<ID3DBlob> mGSData;
		ComPtr<ID3DBlob> mPSData;
		EShaderFlags     mFlags;
		String mSourceCode;
		String mFullSourceCode;
		bool mIsCompileSuccess = false;
		List<std::pair<EShaderDataType, String>> mPropertyList;
	public:
		virtual bool          Compile(const String& sourceCode, const List<SharedPtr<IShaderScript>>& scriptList, EShaderFlags flags, String* error) override;
		virtual const String& GetShaderCode() const override;
		virtual const String& GetFullShaderCode() const override;
		virtual EShaderFlags  GetFlags() const override;
		virtual bool IsSuccessed() const override;
		virtual const List<std::pair<EShaderDataType, String>>& GetPropertyList() const override;
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
		void InsertScript(String& code, const List<SharedPtr<IShaderScript>>& scriptList);
		bool InsertScriptInternal(String& code, SharedPtr<IShaderScript> script);
		bool ExtractScriptContents(const String& code, const String& key, String& out_code);
	};


	class DirectX12ComputeShader : public IComputeShader
	{
	private:
		ComPtr<ID3DBlob> mCSData;
		String			 mSourceCode;
		bool			 mIsCompileSuccess = false;
		UniquePtr<ShaderDataForm> mShaderDataForm;
	public:
		virtual bool  Compile(const String& sourceCode, String* error) override;
		virtual const String& GetShaderCode() const override;
		virtual bool  IsSuccessed() const override;
	private:
		bool Compile(ComPtr<ID3DBlob>& blob, const String& sourceCode, const CompileConfig& config, String* error);
	};

}

