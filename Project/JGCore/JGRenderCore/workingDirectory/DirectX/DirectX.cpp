#include"DirectX.h"
#include"JGDevice.h"
#include"JGSwapChain.h"
#include"JGRenderTarget.h"
#include"JGRenderState.h"
using namespace JGRenderCore;
using namespace std;

DirectX::DirectX() 
{ 
	m_Device       = make_unique<JGDevice>();
	m_SwapChain    = make_unique<JGSwapChain>();
	m_RenderTarget = make_unique<JGRenderTarget>();
}
DirectX::~DirectX() { }