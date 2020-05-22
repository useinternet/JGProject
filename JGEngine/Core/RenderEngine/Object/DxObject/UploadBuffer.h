#pragma once
#include "DxObject.h"

namespace RE
{
	class UploadBuffer : public DxObject
	{
	public:
		// CPU 주소와 GPU 주소가 들어있는 구조체 
		struct Allocation
		{
			void* CPU;
			D3D12_GPU_VIRTUAL_ADDRESS GPU;
		};
	private:
		class Page
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> m_D3D12Resource;
			//
			void* m_CPUptr;
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUptr;
			//
			size_t m_PageSize;
			size_t m_Offset;
		public:
			Page(ID3D12Device* device, size_t sizeInBytes);
			~Page();

			/*
			현재 요청한 메모리 할당할수 있는 공간 여유가 있는지 체크 해준다. */
			bool HasSpace(size_t sizeInBytes, size_t alignment) const;

			/*
			페이지 공간을 할당한다.*/
			Allocation Allocate(size_t sizeInBytes, size_t alignment);

			// 페이지 리셋
			void Reset();

		};
		using PagePool = std::deque<std::shared_ptr<Page>>;

		PagePool m_PagePool;
		PagePool m_AvailablePages;

		std::shared_ptr<Page> m_CurrentPage;

		size_t m_PageSize;

	public:
		explicit UploadBuffer(size_t pageSize = 2 * 1024 * 1024);
		virtual ~UploadBuffer();
	public:
		size_t GetPageSize() const { return m_PageSize; }
		Allocation Allocate(size_t sizeInBytes, size_t alignment);
		void Reset();
	private:
		// 메모리 요청이 들어오면 이미 할당되고 버려진 페이지들이 할당되고 만약 없다면
		// 새로 페이지를 작성한다.
		std::shared_ptr<Page> RequestPage();
	};
}
