#include "pch.h"
#include "JGGraphics.h"
#include "GraphicsAPI.h"
#include "RootSignature.h"


namespace JG
{
	SharedPtr<IRootSignatureCreater> IRootSignatureCreater::Create()
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		return api->CreateRootSignatureCreater();
	}
}