#include "pch.h"
#include "RenderParam.h"
#include "Renderer.h"


namespace JG
{

	bool RP_Global_Tex::SetValue(SharedPtr<ITexture> value)
	{
		if (IsVaild() == false)
		{
			return false;
		}
		return mRenderer->SetGlobalRenderParamTex(GetName(), value);
	}
	SharedPtr<ITexture> RP_Global_Tex::GetValue() const
	{
		if (IsVaild() == false)
		{
			return nullptr;
		}
		return mRenderer->GetGlobalRenderParamTex(GetName());
	}

	RP_Global_Tex RP_Global_Tex::Create(const String& name, SharedPtr<ITexture> initTex, Renderer* renderer, bool isPrivate)
	{
		RP_Global_Tex tex;
		tex.mRenderer = renderer;
		tex.mName = name;
		tex.mIsVaild = renderer->RegisterGlobalRenderParamTex(name, initTex, isPrivate);
		return tex;
	}

	RP_Global_Tex RP_Global_Tex::Load(const String& name, Renderer* renderer)
	{
		auto result = renderer->GetGlobalRenderParamTex(name);
		if (result == nullptr)
		{
			return RP_Global_Tex();
		}
		RP_Global_Tex tex;
		tex.mRenderer = renderer;
		tex.mName     = name;
		tex.mIsVaild  = true;

		return tex;
	}


	bool RP_Local_Tex::SetValue(SharedPtr<ITexture> value)
	{
		if (IsVaild() == false)
		{
			return false;
		}
		return mRenderer->SetLocalRenderParamTex(Type_Key, GetName(), value);
	}
	SharedPtr<ITexture> RP_Local_Tex::GetValue() const
	{
		if (IsVaild() == false)
		{
			return nullptr;
		}
		return mRenderer->GetLocalRenderParamTex(Type_Key, GetName());
	}
	RP_Local_Tex RP_Local_Tex::Create(const String& name, SharedPtr<ITexture> initTex, const Type& type, Renderer* renderer, bool isPrivate)
	{
		RP_Local_Tex tex;
		tex.mRenderer = renderer;
		tex.mName = name;
		tex.Type_Key = type;
		tex.mIsVaild = renderer->RegisterLocalRenderParamTex(type, name, initTex, isPrivate);
		return tex;
	}
	RP_Local_Tex RP_Local_Tex::Load(const Type& type, const String& name, Renderer* renderer)
	{
		auto result = renderer->GetLocalRenderParamTex(type, name);
		if (result == nullptr)
		{
			return RP_Local_Tex();
		}
		RP_Local_Tex tex;
		tex.mRenderer = renderer;
		tex.mName     = name;
		tex.Type_Key  = type;
		tex.mIsVaild  = true;
		return tex;
	}
}
