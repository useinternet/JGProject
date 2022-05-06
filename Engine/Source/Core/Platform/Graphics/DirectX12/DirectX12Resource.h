#pragma once




#include "JGCore.h"
#include "Utill/DirectX12Helper.h"
#include "Utill/ShaderDataForm.h"
#include "Utill/DescriptorAllocator.h"
#include "Graphics/Resource.h"



namespace JG
{
	class ComputeCommandList;
	class IComputeShader;
	class DirectX12VertexBuffer : public IVertexBuffer
	{
	private:
		u64   mElementSize  = 0;
		u64   mElementCount = 0;
		void* mCPUData  = nullptr;
		EBufferLoadMethod mLoadMethod;
		ComPtr<ID3D12Resource>  mD3DResource;


		mutable bool mSRVDirty = true;
		mutable bool mUAVDirty = true;
		mutable std::mutex mSRV_UAVMutex;

		mutable DescriptorAllocation mSRV;
		mutable DescriptorAllocation mUAV;
	public:
		DirectX12VertexBuffer() = default;
		virtual ~DirectX12VertexBuffer();
	public:
		// --- Resource Interface --- 
		virtual bool  IsValid() const override;
		virtual PrimitiveResourcePtr GetPrimitiveResourcePtr() const override;
		virtual ResourceGPUVirtualAddress GetResourceGPUVirtualAddress() const override;
		// ------------------------

		virtual bool SetData(const void* datas, u64 elementSize, u64 elementCount) override;
		virtual void SetBufferLoadMethod(EBufferLoadMethod type) override;
		virtual EBufferLoadMethod GetBufferLoadMethod() const override;


		virtual u64 GetVertexCount() const override;
		virtual u64 GetVertexSize() const override;


		ID3D12Resource* Get() const {
			return mD3DResource.Get();
		}
	protected:
		void Reset();
		void SendResourceViewDirty();
	public:
		void* GetData() const
		{
			return mCPUData;
		}
		
		virtual ResourceViewPtr GetSRV() const override;
		virtual ResourceViewPtr GetUAV() const override;

		DEFINE_NOT_SUPPORTED_RTV
		DEFINE_NOT_SUPPORTED_DSV
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

		mutable bool mSRVDirty = true;
		mutable bool mUAVDirty = true;
		mutable DescriptorAllocation mSRV;
		mutable DescriptorAllocation mUAV;
		mutable std::mutex mSRV_UAVMutex;
	public:
		DirectX12IndexBuffer() = default;
		virtual ~DirectX12IndexBuffer();
	public:
		// --- Resource Interface --- 
		virtual bool IsValid() const override;

		virtual PrimitiveResourcePtr GetPrimitiveResourcePtr() const override;

		virtual ResourceGPUVirtualAddress GetResourceGPUVirtualAddress() const override;

		// ------------------------

		virtual bool SetData(const u32* datas, u64 count) override;
		virtual void SetBufferLoadMethod(EBufferLoadMethod method) override;
		virtual EBufferLoadMethod GetBufferLoadMethod() const override;

		ID3D12Resource* Get() const {
			return mD3DResource.Get();
		}
	protected:
		void Reset();
		void SendResourceViewDirty();
	public:
		u32* GetData() const
		{
			return mCPUData;
		}
		virtual u32 GetIndexCount() const override
		{
			return mIndexCount;
		}
		virtual ResourceViewPtr GetSRV() const override;
		virtual ResourceViewPtr GetUAV() const override;

		DEFINE_NOT_SUPPORTED_RTV
		DEFINE_NOT_SUPPORTED_DSV
	};

	class DirectX12ByteAddressBuffer : public IByteAddressBuffer
	{
		const u64 mElementSize = 4;
		u64   mElementCount = 0;

		ComPtr<ID3D12Resource>  mD3DResource;

		UniquePtr<DescriptorAllocation> mUAV;
		UniquePtr<DescriptorAllocation> mSRV;
	public:
		DirectX12ByteAddressBuffer() = default;
		virtual ~DirectX12ByteAddressBuffer() = default;
	public:
		// --- Resource Interface --- 
		virtual bool IsValid() const override;

		virtual PrimitiveResourcePtr GetPrimitiveResourcePtr() const override;

		virtual ResourceGPUVirtualAddress GetResourceGPUVirtualAddress() const override;

		// ------------------------


		virtual bool SetData(u64 elementCount, const void* initDatas = nullptr) override;
	public:
		virtual ResourceViewPtr GetSRV() const override;
		virtual ResourceViewPtr GetUAV() const override;

		DEFINE_NOT_SUPPORTED_RTV
		DEFINE_NOT_SUPPORTED_DSV
		ID3D12Resource* Get() const;
	private:
		void CreateViews();
		void Reset();
	};


	class DirectX12StructuredBuffer : public IStructuredBuffer
	{
		void* mCPUData = nullptr;
		u64 mElementSize  = 0;
		u64 mElementCount = 0;
		EBufferLoadMethod		mLoadMethod;
		ComPtr<ID3D12Resource>  mD3DResource;
	public:
		DirectX12StructuredBuffer() = default;
		virtual ~DirectX12StructuredBuffer();
	public:
		// --- Resource Interface --- 
		virtual bool IsValid() const override;

		virtual PrimitiveResourcePtr GetPrimitiveResourcePtr() const override;

		virtual ResourceGPUVirtualAddress GetResourceGPUVirtualAddress() const override;

		// ------------------------


		virtual bool SetData(u64 elementSize, u64 elementCount, const  void* initDatas = nullptr) override;
		virtual void SetDataByIndex(u64 index, void* data) const override;
		virtual u64 GetDataSize() const override;
		virtual u64 GetElementCount() const override;
		virtual u64 GetElementSize() const override;
		virtual void* GetDataPtr() const override;

		virtual void SetBufferLoadMethod(EBufferLoadMethod method) override;
		virtual EBufferLoadMethod GetBufferLoadMethod() const override;
		ID3D12Resource* Get() const;


		DEFINE_NOT_SUPPORTED_SRV
		DEFINE_NOT_SUPPORTED_UAV
		DEFINE_NOT_SUPPORTED_RTV
		DEFINE_NOT_SUPPORTED_DSV
	private:
		void Reset();
	};


	class DirectX12ReadBackBuffer : public IReadBackBuffer
	{
		ComPtr<ID3D12Resource>  mD3DResource;
		void* mCPU        = nullptr;
		u64   mBufferSize = 0;
	public:
		virtual ~DirectX12ReadBackBuffer();
	public:
		// --- Resource Interface --- 
		virtual bool IsValid() const override;

		virtual PrimitiveResourcePtr GetPrimitiveResourcePtr() const override;

		virtual ResourceGPUVirtualAddress GetResourceGPUVirtualAddress() const override;

		// ------------------------
		virtual bool GetData(void* out_data, u64 out_data_size) override;
		virtual u64 GetDataSize() const override;
	public:
		DEFINE_NOT_SUPPORTED_SRV
		DEFINE_NOT_SUPPORTED_UAV
		DEFINE_NOT_SUPPORTED_RTV
		DEFINE_NOT_SUPPORTED_DSV
		ID3D12Resource* Get() const {
			return mD3DResource.Get();
		}
		void Init(u64 dataSize);
	private:
		void Reset();
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
		// --- Resource Interface --- 
		virtual bool IsValid() const override;

		virtual PrimitiveResourcePtr GetPrimitiveResourcePtr() const override;

		virtual ResourceGPUVirtualAddress GetResourceGPUVirtualAddress() const override;

		// ------------------------

		virtual void SetName(const String& name) override;
		virtual TextureID          GetTextureID() const override;
		virtual const TextureInfo& GetTextureInfo() const override;
		virtual void SetTextureInfo(const TextureInfo& info) override;
		virtual void SetTextureMemory(const jbyte* pixels, i32 width, i32 height, i32 channels, u32 pixelPerUnit,
			u32 arraySize = 1, u32 mipLevel = 1, ETextureFlags flags = ETextureFlags::None, ETextureFormat format = ETextureFormat::R8G8B8A8_Unorm) override;
		virtual void SetClearColor(const Color& clearColor) override;
	public:
		void Create(const String& name, const TextureInfo& info);
		void CreateFromMemory(const String& name, const jbyte* pixels, i32 width, i32 height, i32 comp, u32 pixelPerUnit = 1);
		void Reset();


		virtual ResourceViewPtr GetSRV() const override;
		virtual ResourceViewPtr GetUAV() const override;
		virtual ResourceViewPtr GetRTV() const override;
		virtual ResourceViewPtr GetDSV() const override;

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



