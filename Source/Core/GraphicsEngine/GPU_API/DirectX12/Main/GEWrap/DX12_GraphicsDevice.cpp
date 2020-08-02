#include "pch.h"
#include "DX12_GraphicsDevice.h"
#include "DX12_Texture.h"
#include "DirectXApi.h"


using namespace std;

namespace DX12
{
	
	void DX12_GraphicsDevice::Create(void* config_user_data)
	{
		
		DXDevice::Create();
	}

	void DX12_GraphicsDevice::BeginFrame()
	{
		DXDevice::BeginFrame();
	}

	void DX12_GraphicsDevice::EndFrame()
	{
		DXDevice::EndFrame();
	}

	void DX12_GraphicsDevice::Destroy()
	{
		DXDevice::Destroy();
	}


	bool DX12_GraphicsDevice::LoadTextures(const std::vector<std::wstring>& paths, std::vector<GE::Texture*>& tArray)
	{
		auto cmdKey = DXDevice::RequestCopyCommandKey();
		bool result = true;
		size_t tCount = paths.size();


		for (size_t i = 0; i < tCount; ++i)
		{
			Texture t = DXCommand::LoadTextureFromFile(cmdKey, paths[i], false);
			if (t.IsValid())
			{
				GE::Texture* ge_t = new DX12_Texture();
				*(Texture*)ge_t->GetUserTextureData() = move(t);
				tArray.push_back(ge_t);
			}
			else
			{
				tArray.push_back(nullptr);
				GELOG_ERROR("Fail Texture Load : %s", paths[i].c_str());
				result = false;
			}
		}

		return result;
	}
}

