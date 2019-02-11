#include"CommandListManager.h"
#include"GpuCpuSynchronizer.h"
using namespace JGRC;


void CommandListManager::FlushCommandQueue(GpuCpuSynchronizer* GCS)
{
	GCS->IncreaseOffset();

	ThrowIfFailed(m_CommandQueue->Signal(GCS->GetFence(), GCS->GetOffset()));
	if (GCS->GetFence()->GetCompletedValue() < GCS->GetOffset())
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(GCS->GetFence()->SetEventOnCompletion(GCS->GetOffset(), eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}