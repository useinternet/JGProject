#pragma once


#include"Dx12Include.h"
#include"DescriptorHeap.h"
#include<unordered_map>
#include<vector>
namespace GR
{
	namespace Dx12
	{
		class GraphicsDevice;
		class Commander;
		class ComputeCommander;
		class CommandExecutor;
		enum class GPUResourceUsage
		{
			Default,
			RenderTarget,
			DepthStencil
		};
		class GPUResource
		{
			friend GraphicsDevice;
			friend Commander;
			friend CommandExecutor;
		protected:
			ComPtr<ID3D12Resource> m_Resource;
			D3D12_RESOURCE_STATES  m_UsageState;
			D3D12_RESOURCE_STATES  m_TransitioningState;
		public:
			GPUResource() :
				m_UsageState(D3D12_RESOURCE_STATE_COMMON),
				m_TransitioningState((D3D12_RESOURCE_STATES) - 1) {}
			GPUResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrState) :
				m_Resource(pResource),
				m_UsageState(CurrState),
				m_TransitioningState((D3D12_RESOURCE_STATES)-1) {}

			GPUResource(const GPUResource& copy) = default;
			GPUResource(GPUResource&& rhs) = default;
			GPUResource& operator=(const GPUResource& rhs) = default;
			GPUResource& operator=(GPUResource&& rhs) = default;


			virtual ~GPUResource() = default;


			bool IsValid() const {
				return m_Resource != nullptr;
			}

			ID3D12Resource* operator->() { return m_Resource.Get(); }
			const ID3D12Resource* operator->() const { return m_Resource.Get(); }

			ID3D12Resource* GetResource() { return m_Resource.Get(); }
			const ID3D12Resource* GetResource() const { return m_Resource.Get(); }
		};


		class GPUBuffer : public GPUResource
		{
		private:
			friend GraphicsDevice;
			friend Commander;
			friend CommandExecutor;
		public:
			virtual ~GPUBuffer() {}
		private:
			virtual void CreateView(uint32_t numElements, uint32_t elementSize) {}
		};

		class VertexBuffer : public GPUBuffer
		{
		private:
			uint32_t m_NumVertices;
			uint32_t m_VertexStride;

			D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		public:
			virtual ~VertexBuffer() {}
			D3D12_VERTEX_BUFFER_VIEW GetView() const {
				return m_VertexBufferView;
			}
		private:
			virtual void CreateView(uint32_t numElements, uint32_t elementSize) override;
		};
		class IndexBuffer : public GPUBuffer
		{
			uint32_t m_NumIndices;

			DXGI_FORMAT m_IndexFormat;
			D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		public:
			virtual ~IndexBuffer() {}
			D3D12_INDEX_BUFFER_VIEW GetView() const {
				return m_IndexBufferView;
			}
		private:
			virtual void CreateView(uint32_t numElements, uint32_t elementSize = sizeof(uint32_t)) override;
		};
		class ConstantBuffer : public GPUBuffer
		{
		public:
			virtual ~ConstantBuffer() {}
		};
		class ByteAddressBuffer : public GPUBuffer
		{
		public:
			virtual ~ByteAddressBuffer() {}

		};
		class StructuredBuffer : public GPUBuffer
		{
		public:
			virtual ~StructuredBuffer() {}
		};

		class CPUResource
		{
		public:
			virtual ~CPUResource() {}
		};
		class CPUBuffer : public CPUResource
		{
			ComPtr<ID3DBlob> m_data;
		public:
			virtual ~CPUBuffer();
			ID3DBlob* operator->() { return m_data.Get(); }
			const ID3DBlob* operator->() const { return m_data.Get(); }


			ID3DBlob* GetData() { return m_data.Get(); }

			const ID3DBlob* GetData() const { return m_data.Get(); }
		};




		class Texture : public GPUResource
		{
			friend GraphicsDevice;
			friend Commander;
			friend ComputeCommander;
			friend CommandExecutor;
		private:
			mutable std::unordered_map<uint32_t, DescriptorAllocation> m_SRVs;
			mutable std::unordered_map<uint32_t, DescriptorAllocation> m_UAVs;
		private:
			std::unique_ptr<D3D12_SHADER_RESOURCE_VIEW_DESC>  m_CurrentSrvDesc;
			std::unique_ptr<D3D12_UNORDERED_ACCESS_VIEW_DESC> m_CurrentUavDesc;
		private:
			D3D12_CPU_DESCRIPTOR_HANDLE GetSRV(D3D12_SHADER_RESOURCE_VIEW_DESC* desc) const;
			D3D12_CPU_DESCRIPTOR_HANDLE GetUAV(D3D12_UNORDERED_ACCESS_VIEW_DESC* desc) const;
		public:
			Texture() : GPUResource(), m_CurrentSrvDesc(nullptr), m_CurrentUavDesc(nullptr) {}
			virtual ~Texture() = default;


			Texture(const Texture& copy);
			Texture(Texture&& rhs);
			Texture& operator=(const Texture& rhs);
			Texture& operator=(Texture&& rhs);


			void SetSRVDesc(D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
			void SetUAVDesc(D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);
			D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;
			D3D12_CPU_DESCRIPTOR_HANDLE GetUAV() const;
		};

		class ColorTexture : public Texture
		{
			friend GraphicsDevice;
			friend Commander;
			friend CommandExecutor;

		private:
			mutable std::unordered_map<uint32_t, DescriptorAllocation> m_RTVs;
			std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_CurrentRtvDesc;
			float4 m_ClearColor;
		private:
			D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(D3D12_RENDER_TARGET_VIEW_DESC* desc) const;
		public:
			ColorTexture() :
				Texture(),
				m_CurrentRtvDesc(nullptr), 
				m_ClearColor({ 0.0f,0.0f,0.0f,0.0f }) {}
			virtual ~ColorTexture() = default;

			ColorTexture(const ColorTexture& copy);
			ColorTexture(ColorTexture&& rhs);
			ColorTexture& operator=(const ColorTexture& rhs);
			ColorTexture& operator=(ColorTexture&& rhs);


			void SetRTVDesc(D3D12_RENDER_TARGET_VIEW_DESC* desc);
			D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const;
			
			const float4& GetClearColor() const {
				return m_ClearColor;
			}
			void SetClearColor(const float4& color) {
				m_ClearColor = color;
			}
		};

		class DepthTexture : public Texture
		{
			friend GraphicsDevice;
			friend Commander;
			friend CommandExecutor;
		private:
			mutable std::unordered_map<uint32_t, DescriptorAllocation> m_DSVs;
			std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_CurrentDsvDesc;
			float   m_ClearDepth;
			uint8_t m_ClearStencil;
		private:
			D3D12_CPU_DESCRIPTOR_HANDLE GetDSV(D3D12_DEPTH_STENCIL_VIEW_DESC* desc) const;
		public:
			DepthTexture() :
				Texture(), m_ClearDepth(1.0f),
				m_ClearStencil(0), m_CurrentDsvDesc(nullptr) {}

			virtual ~DepthTexture() = default;
			DepthTexture(const DepthTexture& copy);
			DepthTexture(DepthTexture&& rhs);
			DepthTexture& operator=(const DepthTexture& rhs);
			DepthTexture& operator=(DepthTexture&& rhs);




			void SetDSVDesc(D3D12_DEPTH_STENCIL_VIEW_DESC* desc);
			D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;




			float GetClearDepth() const {
				return m_ClearDepth;
			}
			uint8_t GetClearStencil() const {
				return m_ClearStencil;
			}
			void SetClearDepthStencil(float depth, uint8_t stencil) {
				m_ClearDepth = depth;
				m_ClearStencil = stencil;
			}
			void SetClearDepth(float depth) {
				m_ClearDepth = depth;
			}
			void SetClearStencil(uint8_t stencil) {
				m_ClearStencil = stencil;
			}
		};

		
		class Shader
		{
			friend GraphicsDevice;
		public:
			enum ShaderStage
			{
				VS,
				HS,
				DS,
				GS,
				PS,
				CS,
				NUM_SHADERSTAGE
			};
		private:
			ComPtr<ID3DBlob> m_Blob;
			std::string      m_ErrorString;
		public:
			virtual ~Shader() {}
			virtual ShaderStage GetStage() const = 0;

			inline BYTE* GetBufferPointer() const { return reinterpret_cast<BYTE*>(m_Blob->GetBufferPointer()); }
			inline uint64_t GetSize() const { return m_Blob->GetBufferSize(); }

			bool IsCompileSuccessed() const {
				
				return m_Blob != nullptr;
			}
			const std::string GetErrorString() const {
				return m_ErrorString;
			}
		};
		class VertexShader : public Shader
		{
		public:
			virtual ~VertexShader() {}
			virtual ShaderStage GetStage() const override { return VS; }
		};
		class HullShader : public Shader
		{
		public:
			virtual ~HullShader() {}
			virtual ShaderStage GetStage() const override { return HS; }
		};
		class DomainShader : public Shader
		{
		public:
			virtual ~DomainShader() {}
			virtual ShaderStage GetStage() const override { return DS; }
		};
		class GeometryShader : public Shader
		{
		public:
			virtual ~GeometryShader() {}
			virtual ShaderStage GetStage() const override { return GS; }
		};
		class PixelShader : public Shader
		{
		public:
			virtual ~PixelShader() {}
			virtual ShaderStage GetStage() const override { return PS; }
		};
		class ComputeShader : public Shader
		{
		public:
			virtual ~ComputeShader() {}
			virtual ShaderStage GetStage() const override { return CS; }
		};
		










	}
	

}