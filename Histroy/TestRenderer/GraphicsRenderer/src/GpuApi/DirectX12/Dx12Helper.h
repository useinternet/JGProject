#pragma once



#include"Dx12Include.h"
namespace GR
{
	namespace Dx12
	{
		ComPtr<IDXGIFactory4> CreateDXGIFactory();


		ComPtr<ID3D12Device> CreateD3DDevice(
			ComPtr<IDXGIFactory4> factory,
			bool is_UseWrapDevice,
			DXGI_ADAPTER_DESC1* OutadapterDesc = nullptr);


		ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
			HWND hWnd, 
			ComPtr<IDXGIFactory4> factory,
			ComPtr<ID3D12CommandQueue> cmdQue,
			DXGI_FORMAT format,
			uint32_t width, 
			uint32_t height, 
			uint32_t bufferCount);



		ComPtr<ID3D12CommandQueue> CreateD3DCommandQueue(
			ComPtr<ID3D12Device> device, 
			D3D12_COMMAND_LIST_TYPE type);


		ComPtr<ID3D12GraphicsCommandList> CreateD3DCommandList(
			ComPtr<ID3D12Device> device,
			ComPtr<ID3D12CommandAllocator> cmdAllocator,
			D3D12_COMMAND_LIST_TYPE type);


		ComPtr<ID3D12CommandAllocator> CreateD3DCommandAllocator(
			ComPtr<ID3D12Device> device, 
			D3D12_COMMAND_LIST_TYPE type);


		ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(
			ComPtr<ID3D12Device> device,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			D3D12_DESCRIPTOR_HEAP_FLAGS flag, 
			uint32_t numDescriptor);

		ComPtr<ID3D12RootSignature> CreateD3DRootSignature(ComPtr<ID3D12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC);


		bool CheckTearingSupport(ComPtr<IDXGIFactory4> factory);


		ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

		ComPtr<ID3DBlob> CompileShader(
			const std::wstring filename,
			const D3D_SHADER_MACRO* defines,
			const std::string& entrypoint,
			const std::string& target,
			std::string* errorCode);



		namespace Hash
		{
			struct pair_hash {
			public:
				template <typename T, typename U>
				std::size_t operator()(const std::pair<T, U> &x) const
				{
					return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
				}
			};

			inline size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash)
			{
#if ENABLE_SSE_CRC32
				const uint64_t* Iter64 = (const uint64_t*)Math::AlignUp(Begin, 8);
				const uint64_t* const End64 = (const uint64_t* const)Math::AlignDown(End, 8);

				// If not 64-bit aligned, start with a single u32
				if ((uint32_t*)Iter64 > Begin)
					Hash = _mm_crc32_u32((uint32_t)Hash, *Begin);

				// Iterate over consecutive u64 values
				while (Iter64 < End64)
					Hash = _mm_crc32_u64((uint64_t)Hash, *Iter64++);

				// If there is a 32-bit remainder, accumulate that
				if ((uint32_t*)Iter64 < End)
					Hash = _mm_crc32_u32((uint32_t)Hash, *(uint32_t*)Iter64);
#else
				// An inexpensive hash for CPUs lacking SSE4.2
				for (const uint32_t* Iter = Begin; Iter < End; ++Iter)
					Hash = 16777619U * Hash ^ *Iter;
#endif

				return Hash;
			}
			template <typename T> inline size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
			{
				static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
				return HashRange((uint32_t*)StateDesc, (uint32_t*)(StateDesc + Count), Hash);
			}


			inline uint32_t D3D_Macro_Hash(const std::vector<D3D_SHADER_MACRO>& vec)
			{
				uint32_t seed = (uint32_t)vec.size();
				for (auto& i : vec)
				{
					if (i.Definition == nullptr || i.Name == nullptr)
						continue;

					uint32_t pair_hash_code = (uint32_t)pair_hash()(std::pair< std::string, std::string>(i.Name, i.Definition));
					seed ^= pair_hash_code + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				}
				return seed;
			}

		}
		namespace Math
		{
			template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask)
			{
				return (T)(((size_t)value + mask) & ~mask);
			}

			template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask)
			{
				return (T)((size_t)value & ~mask);
			}

			template <typename T> __forceinline T AlignUp(T value, size_t alignment)
			{
				return AlignUpWithMask(value, alignment - 1);
			}

			template <typename T> __forceinline T AlignDown(T value, size_t alignment)
			{
				return AlignDownWithMask(value, alignment - 1);
			}

			template <typename T> __forceinline bool IsAligned(T value, size_t alignment)
			{
				return 0 == ((size_t)value & (alignment - 1));
			}

			template <typename T> __forceinline T DivideByMultiple(T value, size_t alignment)
			{
				return (T)((value + alignment - 1) / alignment);
			}

			template <typename T> __forceinline bool IsPowerOfTwo(T value)
			{
				return 0 == (value & (value - 1));
			}

			template <typename T> __forceinline bool IsDivisible(T value, T divisor)
			{
				return (value / divisor) * divisor == value;
			}

			__forceinline uint8_t Log2(uint64_t value)
			{
				unsigned long mssb; // most significant set bit
				unsigned long lssb; // least significant set bit

				// If perfect power of two (only one set bit), return index of bit.  Otherwise round up
				// fractional log by adding 1 to most signicant set bit's index.
				if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
					return uint8_t(mssb + (mssb == lssb ? 0 : 1));
				else
					return 0;
			}

			template <typename T> __forceinline T AlignPowerOfTwo(T value)
			{
				return value == 0 ? 0 : 1 << Log2(value);
			}

		}
	}
}
