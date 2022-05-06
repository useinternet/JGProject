#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
#include "InputLayout.h"
namespace JG
{
#pragma region Define

#define DEFINE_NOT_SUPPORTED_SRV \
	virtual ResourceViewPtr GetSRV() const override \
	{ \
		JG_LOG_ERROR("This Resource Not Supported Shader Resource View"); \
		return 0; \
	} \

#define DEFINE_NOT_SUPPORTED_UAV \
	virtual ResourceViewPtr GetUAV() const override \
	{ \
		JG_LOG_ERROR("This Resource Not Supported Unordered Access View"); \
		return 0; \
	} \

#define DEFINE_NOT_SUPPORTED_RTV \
	virtual ResourceViewPtr GetRTV() const override \
	{ \
		JG_LOG_ERROR("This Resource Not Supported Render Target View"); \
		return 0; \
	} \

#define DEFINE_NOT_SUPPORTED_DSV \
	virtual ResourceViewPtr GetDSV() const override \
	{ \
		JG_LOG_ERROR("This Resource Not Supported Depth Stencil View"); \
		return 0; \
	} \


#pragma endregion




	class ITexture;
	class IComputeShader;
	class IResource
	{
	private:
		String mName   = "IResource";
	public:
		IResource() = default;
		virtual ~IResource() = default;
		virtual void SetName(const String& name)
		{
			mName = name;
		}
		const String& GetName() const
		{
			return mName;
		}
		
	public:
		virtual PrimitiveResourcePtr      GetPrimitiveResourcePtr()      const = 0;
		virtual ResourceGPUVirtualAddress GetResourceGPUVirtualAddress() const = 0;
		virtual ResourceViewPtr GetSRV() const = 0;
		virtual ResourceViewPtr GetUAV() const = 0;
		virtual ResourceViewPtr GetRTV() const = 0;
		virtual ResourceViewPtr GetDSV() const = 0;
		virtual bool IsValid() const = 0;
	private:
		IResource(const IResource&) = delete;
		const IResource& operator=(const IResource&) = delete;
	};


	class IVertexBuffer : public IResource
	{
	public:
		virtual ~IVertexBuffer() = default;
	protected:
		virtual void SetBufferLoadMethod(EBufferLoadMethod method) = 0;
	public:
		virtual bool SetData(const void* datas, u64 elementSize, u64 elementCount) = 0;
		virtual EBufferLoadMethod GetBufferLoadMethod() const = 0;
		virtual u64 GetVertexCount() const = 0;
		virtual u64 GetVertexSize() const = 0;
	public:
		static SharedPtr<IVertexBuffer> Create(const String& name, EBufferLoadMethod method);
	};

	class IIndexBuffer : public IResource
	{
	public:
		virtual ~IIndexBuffer() = default;
	protected:
		virtual void SetBufferLoadMethod(EBufferLoadMethod method) = 0;
	public:
		virtual bool SetData(const u32* datas, u64 count) = 0;
		virtual EBufferLoadMethod GetBufferLoadMethod() const = 0;
		virtual u32 GetIndexCount() const = 0;
	public:
		static SharedPtr<IIndexBuffer> Create(const String& name, EBufferLoadMethod method);
	};

	class IByteAddressBuffer : public IResource
	{
	public:
		virtual ~IByteAddressBuffer() = default;
	public:
		virtual bool SetData(u64 elementCount, const void* initDatas = nullptr) = 0;
	public:
		static SharedPtr<IByteAddressBuffer> Create(const String& name, u64 elementCount);
	};


	class IStructuredBuffer : public IResource
	{
	public:
		virtual ~IStructuredBuffer() = default;
	public:
		virtual bool SetData(u64 elementSize, u64 elementCount, const  void* initDatas = nullptr) = 0;
		virtual void SetDataByIndex(u64 index, void* data) const = 0;
	public:
		virtual u64      GetDataSize()     const = 0;
		virtual u64      GetElementCount() const = 0;
		virtual u64		 GetElementSize()  const = 0;
		virtual void*    GetDataPtr() const = 0;

	protected:
		virtual void SetBufferLoadMethod(EBufferLoadMethod type) = 0;
		virtual EBufferLoadMethod GetBufferLoadMethod() const = 0;
	public:
		static SharedPtr<IStructuredBuffer> Create(const String& name, u64 elementSize, u64 elementCount, EBufferLoadMethod method);
	};

	class IReadBackBuffer : public IResource
	{
	public:
		virtual ~IReadBackBuffer() = default;
	public:
		virtual bool GetData(void* out_data, u64 out_data_size) = 0;
		virtual u64  GetDataSize() const = 0;
	public:
		static SharedPtr<IReadBackBuffer> Create(const String& name, u64 dataSize);
	};
	
	class TextureAssetStock;
	class ITexture : public IResource
	{
	public:
		ITexture() = default;
		virtual ~ITexture() = default;
	public:
		virtual TextureID          GetTextureID()   const = 0;
		virtual const TextureInfo& GetTextureInfo() const = 0;
		virtual void  SetTextureInfo(const TextureInfo& info) = 0;
		virtual void  SetTextureMemory(
			const jbyte* pixels, i32 width, i32 height, i32 channels, u32 pixelPerUnit = 1,
			u32 arraySize = 1, u32 mipLevel = 1, ETextureFlags flags = ETextureFlags::None, ETextureFormat format = ETextureFormat::R8G8B8A8_Unorm) = 0;
		virtual void  SetClearColor(const Color& clearColor) = 0;
	private:
		ITexture(const ITexture& texture) = delete;
		const ITexture& operator=(const ITexture& texture) = delete;
	public:
		static SharedPtr<ITexture> Create(const String& name);
		static SharedPtr<ITexture> Create(const String& name, const TextureInfo& info);
		static SharedPtr<ITexture> NullTexture();

	private:

		friend class Application;
		static void CreateNullTexture();
		static void DestroyNullTexture();
	};
}

