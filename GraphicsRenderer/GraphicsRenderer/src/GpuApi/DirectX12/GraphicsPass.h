#pragma once
#include"Dx12Include.h"
#include"RenderTarget.h"
#include"RootSignature.h"
#include"Viewport.h"
#include"ScissorRect.h"
#include"ShaderType.h"
#include"PipelineState.h"
namespace GR
{
	namespace Dx12
	{
		class GraphicsCommander;
		class GraphicsPass
		{
		public:
			virtual void Init(uint32_t width, uint32_t height) = 0;
			virtual void Resize(uint32_t width, uint32_t height) = 0;
			virtual void Execute(GraphicsCommander* commander, const PassCB& passcb) = 0;
			virtual std::string ToString() { return "None";  }
		};
	}
}