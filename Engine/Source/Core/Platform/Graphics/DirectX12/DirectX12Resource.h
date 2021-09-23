#pragma once




#include "JGCore.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/ShaderDataForm.h"
#include "Graphics/Resource.h"



namespace JG
{
	class DescriptorAllocation;
	class DirectX12VertexBuffer : public IVertexBuffer
	{
		friend class DirectX12Mesh;
		friend class DirectX12SubMesh;
	private:
		u64   mElementSize  = 0;
		u64   mElementCount = 0;
		void* mCPUData  = nullptr;
		EBufferLoadMethod mLoadMethod;
		ComPtr<ID3D12Resource>  mD3DResource;
	public:
		DirectX12VertexBuffer() = default;
		virtual ~DirectX12VertexBuffer();
	public:
		virtual bool  SetData(const void* datas, u64 elementSize, u64 elementCount) override;
		virtual bool  IsValid() const override;
		virtual void SetBufferLoadMethod(EBufferLoadMethod type) override;
		virtual EBufferLoadMethod GetBufferLoadMethod() const override;
	protected:
		virtual void Bind() override;
		void Reset();
	public:
		void* GetData() const
		{
			return mCPUData;
		}
		u64 GetElementSize() const
		{
			return mElementSize;
		}
		u64 GetElementCount() const
		{
			return mElementCount;
		}
	};

	class DirectX12IndexBuffer : public IIndexBuffer
	{
		friend class DirectX12Mesh;
		friend class DirectX12SubMesh;
	private:
		u32* mCPUData    = nullptr;
		u32  mIndexCount = 0;
		EBufferLoadMethod mLoadMethod;
		ComPtr<ID3D12Resource>  mD3DResource;
	public:
		DirectX12IndexBuffer() = default;
		virtual ~DirectX12IndexBuffer();
	public:
		virtual bool SetData(const u32* datas, u64 count) override;
		virtual bool IsValid() const override;

		virtual void SetBufferLoadMethod(EBufferLoadMethod method) override;
		virtual EBufferLoadMethod GetBufferLoadMethod() const override;
	protected:
		virtual void Bind() override;
		void Reset();
	public:
		u32* GetData() const
		{
			return mCPUData;
		}
		virtual u32 GetIndexCount() const override
		{
			return mIndexCount;
		}
	};
	class DirectX12ComputeBuffer : public IComputeBuffer
	{
		ComPtr<ID3D12Resource> mD3DResource;
		EComputeBufferState mState = EComputeBufferState::Wait;
		void* mCPU      = nullptr;
		u64 mBufferSize = 0;
	public:
		virtual ~DirectX12ComputeBuffer() = default;
	public:
		virtual void SetData(u64 btSize) override;
		virtual bool GetData(void** out_data) override;
		virtual u64  GetDataSize() const override;
	public:
		virtual bool IsValid() const override;
		virtual EComputeBufferState GetState() const override;
	public:
		ID3D12Resource* Get() const {
			return mD3DResource.Get();
		}
	private:
		friend class DirectX12Computer;
		void ReserveCompletion();
	};
	class DirectX12Computer : public IComputer
	{
	private:
		String         mName;
		EComputerState mState = EComputerState::Wait;

		UniquePtr<ShaderData>     mShaderData;
		SharedPtr<ScheduleHandle> mScheduleHandle;

		Dictionary<String, SharedPtr<IComputeBuffer>> mComputeBuffers;
	public:
		virtual bool SetComputeBuffer(SharedPtr<IComputeBuffer> computeBuffer) override;
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
		virtual bool GetInt2(const String& name, JVector2Int* value) override;
		virtual bool GetInt3(const String& name, JVector3Int* value) override;
		virtual bool GetInt4(const String& name, JVector4Int* value) override;
		virtual bool GetUint(const String& name, u32* value) override;
		virtual bool GetUint2(const String& name, JVector2Uint* value) override;
		virtual bool GetUint3(const String& name, JVector3Uint* value) override;
		virtual bool GetUint4(const String& name, JVector4Uint* value) override;
		virtual bool GetFloat4x4(const String& name, JMatrix* out_value) override;
		virtual bool GetTexture(const String& name, u32 textureSlot, SharedPtr<ITexture>* out_value) override;
	public:
		void Init(SharedPtr<IShader> shader);
		virtual const String& GetName() const override;
		virtual void  SetName(const String& name) override;
		virtual EComputerState GetState() const override;
		virtual bool Dispatch(u32 groupX, u32 groupY, u32 groupZ) override;
	};






	class DirectX12Texture : public ITexture
	{
	private:
		ComPtr<ID3D12Resource> mD3DResource;
		TextureInfo mTextureInfo;

		mutable Dictionary<u64, DescriptorAllocation> mRTVs;
		mutable Dictionary<u64, DescriptorAllocation> mDSVs;
		mutable Dictionary<u64, DescriptorAllocation> mSRVs;
		mutable Dictionary<u64, DescriptorAllocation> mUAVs;

		mutable std::shared_mutex mRTVMutex;
		mutable std::shared_mutex mDSVMutex;
		mutable std::shared_mutex mSRVMutex;
		mutable std::shared_mutex mUAVMutex;
	public:
		DirectX12Texture() = default;
		virtual ~DirectX12Texture();
	public:
		virtual void SetName(const String& name) override;
		virtual TextureID          GetTextureID() const override;
		virtual const TextureInfo& GetTextureInfo() const override;
		virtual void SetTextureInfo(const TextureInfo& info) override;
		virtual void SetTextureMemory(const byte* pixels, i32 width, i32 height, i32 channels, u32 pixelPerUnit) override;
		virtual void SetClearColor(const Color& clearColor) override;
		virtual bool IsValid() const override;
	public:
		void Create(const String& name, const TextureInfo& info);
		void CreateFromMemory(const String& name, const byte* pixels, i32 width, i32 height, i32 comp, u32 pixelPerUnit = 1);
		void Reset();


		D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUAV() const;


		ID3D12Resource* Get() const {
			return mD3DResource.Get();
		}
		ID3D12Resource** GetAddress() {
			return mD3DResource.GetAddressOf();
		}
	private:
		SharedPtr<D3D12_RENDER_TARGET_VIEW_DESC> CreateRTVDesc(ETextureFlags flag) const;
		SharedPtr<D3D12_DEPTH_STENCIL_VIEW_DESC> CreateDSVDesc(ETextureFlags flag) const;
		SharedPtr<D3D12_SHADER_RESOURCE_VIEW_DESC>  CreateSRVDesc(ETextureFlags flag) const;
		SharedPtr<D3D12_UNORDERED_ACCESS_VIEW_DESC> CreateUAVDesc(ETextureFlags flag) const;
	};
}



