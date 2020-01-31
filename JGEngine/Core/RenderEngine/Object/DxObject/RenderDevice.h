#pragma once
#include "DxObject.h"
#include "GUIAllocation.h"


namespace RE
{
	class Resource;
	class DescriptorAllocator;
	class DescriptorHandle;
	class CommandQueue;
	class CommandList;
	class Texture;
	class RenderTarget;
	class GUI;
	class Shader;
	class ShaderLibManager;
	class DxScreen;
	struct RENDERENGINE_API RenderDeviceDesc
	{
		std::shared_ptr<GUI> gui;
	};

	using RDFrameSubmission = std::function<void(CommandList*)>;
	class RENDERENGINE_API RenderDevice : public DxObject
	{
		static const uint32_t csm_Engine_BufferCount = 3;
	public:
		RenderDevice(const RenderDeviceDesc& desc);
		~RenderDevice();
		
		// Frame Function
		void Update();


		/* Submit Function         쓰레드 0       쓰레드 1        쓰레드 2       쓰레드 3


 실행 : priority_number 0       command_id 0   command_id 1   command_id 2   command_id 3

 실행 : priority_number 1       command_id 0   command_id 1   command_id 2   command_id 3

 실행 : priority_number 2       command_id 0   command_id 1   command_id 2   command_id 3
		
		*/
		void SubmitToRender(uint32_t priority_number, const RDFrameSubmission& func);
		void SubmitToCompute(uint32_t priority_number,const RDFrameSubmission& func);
		void SubmitToCopy(uint32_t priority_number,    const RDFrameSubmission& func);

		// Allocate Function
		DescriptorHandle SrvAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle UavAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle CbvAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle RtvAllocate(uint32_t numDescriptor = 1);
		DescriptorHandle DsvAllocate(uint32_t numDescriptor = 1);

		// GUI Allocate

		// Etc...
		bool CompileShader(Shader& shader, const ShaderCompiler& compiler);

		// DebugInfo
		//void GetGUIAllocatorDebugInfo(Debug::GUIAllocatorInfo& out_debug_info);
		//void GetSrvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info);
		//void GetUavDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info);
		//void GetCbvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info);
		//void GetRtvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info);
		//void GetDsvDescriptorAllocatorDebugInfo(Debug::DescritporAllocatorInfo& out_debug_info);


		DxScreen* CreateDxScreen(HWND hWnd, uint32_t width, uint32_t height,
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		void DestroyDxScreen(HWND hWnd);
		void DestroyDxScreen(DxScreen* screen);
		DxScreen* FindDxScreen(HWND hWnd);

		void Flush();
	public:
		ID3D12Device* GetD3DDevice();
		IDXGIFactory4* GetDxgiFactory();
		CommandQueue* GetDirectCmdQueue();
		CommandQueue* GetComputeCmdQueue();
		CommandQueue* GetCopyCmdQueue();
		uint64_t GetFrameCount() const;
		uint32_t GetValueIndex() const;
		const RenderDeviceDesc& GetDesc() const;
	private:
		void BeginFrame();
		std::vector<CommandList*> RenderUpdate();
		std::vector<CommandList*> ComputeUpdate();
		std::vector<CommandList*> CopyUpdate();
		void EndFrame();
	private:
		struct FrameResource
		{
			uint64_t DirectFenceValue;
			uint64_t ComputeFenceValue;
			uint64_t CopyFenceValue;
			uint64_t EngineFrameCount;

			FrameResource() :
				DirectFenceValue(0),
				ComputeFenceValue(0),
				CopyFenceValue(0),
				EngineFrameCount(0) { }
		};
	private:
		// DXGI or D3D 's Objects
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
		Microsoft::WRL::ComPtr<ID3D12Device>  m_Device;
		
		//  타입별 CommandQueue 및 프레임 리소스
		std::shared_ptr<CommandQueue> m_DirectCommandQueue;
		std::shared_ptr<CommandQueue> m_ComputeCommandQueue;
		std::shared_ptr<CommandQueue> m_CopyCommandQueue;
		std::vector<FrameResource> m_FrameResources;

		// 메모리 할당자
		std::shared_ptr<DescriptorAllocator>  m_SrvAllocator;
		std::shared_ptr<DescriptorAllocator>  m_UavAllocator;
		std::shared_ptr<DescriptorAllocator>  m_CbvAllocator;
		std::shared_ptr<DescriptorAllocator>  m_RtvAllocator;
		std::shared_ptr<DescriptorAllocator>  m_DsvAllocator;


		// 장치 프레임 업데이트 커맨드 리스트 관리자
		using RDFrameSubmissionPool = std::map<uint32_t, std::vector<RDFrameSubmission>>;
		RDFrameSubmissionPool m_RenderFrameSubmissionPool;
		RDFrameSubmissionPool m_ComputeFrameSubmissionPool;
		RDFrameSubmissionPool m_CopyFrameSubmissionPool;

		// 장치 변수 및 정보
		RenderDeviceDesc m_RenderDeviceDesc;
		uint64_t m_Frame;
		uint32_t m_ValueIndex;
		//
		std::unordered_map<DxScreen*, std::shared_ptr<DxScreen>> m_DxScreenPool;
		std::unordered_map<HWND, DxScreen*> m_DxScreenMapByHWND;
	};
}