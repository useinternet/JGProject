#include"RootSignatureManager.h"
#include"Shader/Shader.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Shader/SSAOShaderRootSignature.h"
#include"Shader/ShaderRootSignature.h"

using namespace JGRC;
using namespace std;
using namespace DirectX;


void RootSignatureManager::CreateManager(ID3D12Device* Device)
{
	CreateRootSig(ERootSigType::Common, Device);
	CreateRootSig(ERootSigType::CommonSkinned, Device);
	CreateRootSig(ERootSigType::SSAO, Device);
}
ShaderRootSignatureBase* RootSignatureManager::GetRootSig(ERootSigType type)
{
	return m_RootSigs[type];
}
void RootSignatureManager::CreateRootSig(ERootSigType type, ID3D12Device* Device)
{
	unique_ptr<ShaderRootSignatureBase> RootSig;
	switch (type)
	{
	case ERootSigType::Common:
		RootSig = make_unique<CommonShaderRootSignature>();
		break;
	case ERootSigType::CommonSkinned:
		RootSig = make_unique<CommonSkinnedShaderRootSignature>();
		break;
	case ERootSigType::SSAO:
		RootSig = make_unique<SSAOShaderRootSignature>();
		break;
	}
	RootSig->RootSign(Device);
	ShaderRootSignatureBase* result = RootSig.get();
	m_RootSigMems.push_back(move(RootSig));
	m_RootSigs[type] = result;
}