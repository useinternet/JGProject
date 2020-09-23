#include "pch.h"
#include "GraphicPassDefine.h"
#include "../../DX12_GraphicsRenderer.h"
using namespace std;

namespace DX12
{
	std::unique_ptr<RootSignature> GraphicPassMap::RootSig;
	bool GPRunData::GetPSO(GE::SceneObject* obj, GraphicsPipelineState& pso)
	{
		return OwnerRenderer->GetPSO(obj, pso);
	}
	std::unique_ptr<DX12_GraphicPass> DX12_GraphicPass::Copy(std::wstring& errorCode) const
	{
		auto pass = std::make_unique<DX12_GraphicPass>();
		pass->SetUp = SetUp;
		pass->Run = Run;
		pass->SetUp(pass->RT, pass->PSO, errorCode);
		return std::move(pass);
	}

}