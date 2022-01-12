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
		return mRenderParamManager->SetGlobalRenderParamTex(GetName(), value);
	}
	SharedPtr<ITexture> RP_Global_Tex::GetValue() const
	{
		if (IsVaild() == false)
		{
			return nullptr;
		}
		return mRenderParamManager->GetGlobalRenderParamTex(GetName());
	}

	RP_Global_Tex RP_Global_Tex::Create(const String& name, SharedPtr<ITexture> initTex, RenderParamManager* rpManger, bool isPrivate)
	{
		RP_Global_Tex tex;
		tex.mRenderParamManager = rpManger;
		tex.mName = name;
		tex.mIsValid = rpManger->RegisterGlobalRenderParamTex(name, initTex, isPrivate);
		return tex;
	}

	RP_Global_Tex RP_Global_Tex::Load(const String& name, RenderParamManager* rpManger)
	{
		if (rpManger == nullptr)
		{
			return RP_Global_Tex();
		}
		auto result = rpManger->GetGlobalRenderParamTex(name);
		if (result == nullptr)
		{
			return RP_Global_Tex();
		}
		RP_Global_Tex tex;
		tex.mRenderParamManager = rpManger;
		tex.mName     = name;
		tex.mIsValid  = true;

		return tex;
	}

	bool RP_Global_SB::SetValue(SharedPtr<IStructuredBuffer> value)
	{
		if (IsVaild() == false)
		{
			return false;
		}
		return mRenderParamManager->SetGlobalRenderParamSB(GetName(), value);
	}
	SharedPtr<IStructuredBuffer> RP_Global_SB::GetValue() const
	{
		if (IsVaild() == false)
		{
			return nullptr;
		}
		return mRenderParamManager->GetGlobalRenderParamSB(GetName());
	}

	RP_Global_SB RP_Global_SB::Create(const String& name, SharedPtr<IStructuredBuffer> initSB, RenderParamManager* rpManger, bool isPrivate)
	{
		RP_Global_SB sb;
		sb.mRenderParamManager = rpManger;
		sb.mName = name;
		sb.mIsValid = rpManger->RegisterGlobalRenderParamSB(name, initSB, isPrivate);
		return sb;
	}

	RP_Global_SB RP_Global_SB::Load(const String& name, RenderParamManager* rpManger)
	{
		if (rpManger == nullptr)
		{
			return RP_Global_SB();
		}
		auto result = rpManger->GetGlobalRenderParamSB(name);
		if (result == nullptr)
		{
			return RP_Global_SB();
		}
		RP_Global_SB sb;
		sb.mRenderParamManager = rpManger;
		sb.mName = name;
		sb.mIsValid = true;

		return sb;
	}
	bool RP_Global_BAB::SetValue(SharedPtr<IByteAddressBuffer> value)
	{
		if (IsVaild() == false)
		{
			return false;
		}
		return mRenderParamManager->SetGlobalRenderParamBAB(GetName(), value);
	}

	SharedPtr<IByteAddressBuffer> RP_Global_BAB::GetValue() const
	{
		if (IsVaild() == false)
		{
			return nullptr;
		}
		return mRenderParamManager->GetGlobalRenderParamBAB(GetName());
	}

	RP_Global_BAB RP_Global_BAB::Create(const String& name, SharedPtr<IByteAddressBuffer> initBAB, RenderParamManager* rpManger, bool isPrivate)
	{
		RP_Global_BAB bab;
		bab.mRenderParamManager = rpManger;
		bab.mName = name;
		bab.mIsValid = rpManger->RegisterGlobalRenderParamBAB(name, initBAB, isPrivate);
		return bab;
	}

	RP_Global_BAB RP_Global_BAB::Load(const String& name, RenderParamManager* rpManger)
	{
		if (rpManger == nullptr)
		{
			return RP_Global_BAB();
		}
		auto result = rpManger->GetGlobalRenderParamBAB(name);
		if (result == nullptr)
		{
			return RP_Global_BAB();
		}
		RP_Global_BAB bab;
		bab.mRenderParamManager = rpManger;
		bab.mName = name;
		bab.mIsValid = true;

		return bab;
	}

	bool RenderParamManager::RegisterGlobalRenderParamTex(const String& name, SharedPtr<ITexture> initTexture, bool isPrivate)
	{
		if (mGlobalParamTexDic.find(name) != mGlobalParamTexDic.end())
		{
			return false;
		}

		auto& _pair = mGlobalParamTexDic[name];
		_pair.first = initTexture;
		_pair.second = isPrivate;


		return true;
	}
	bool RenderParamManager::SetGlobalRenderParamTex(const String& name, SharedPtr<ITexture> tex)
	{
		if (mGlobalParamTexDic.find(name) == mGlobalParamTexDic.end())
		{
			return false;
		}

		auto& _pair = mGlobalParamTexDic[name];
		if (_pair.second)
		{
			return false;
		}

		std::lock_guard<std::mutex> lock(mRenderParamMutex);
		_pair.first = tex;
		return true;
	}
	SharedPtr<ITexture> RenderParamManager::GetGlobalRenderParamTex(const String& name)
	{
		if (mGlobalParamTexDic.find(name) == mGlobalParamTexDic.end())
		{
			return nullptr;
		}

		std::lock_guard<std::mutex> lock(mRenderParamMutex);
		auto& _pair = mGlobalParamTexDic[name];
		return _pair.first;
	}

	bool RenderParamManager::RegisterGlobalRenderParamSB(const String& name, SharedPtr<IStructuredBuffer> initSB, bool isPrivate)
	{
		if (mGlobalParamSBDic.find(name) != mGlobalParamSBDic.end())
		{
			return false;
		}

		auto& _pair = mGlobalParamSBDic[name];
		_pair.first = initSB;
		_pair.second = isPrivate;


		return true;
	}

	bool RenderParamManager::SetGlobalRenderParamSB(const String& name, SharedPtr<IStructuredBuffer> sb)
	{
		if (mGlobalParamSBDic.find(name) == mGlobalParamSBDic.end())
		{
			return false;
		}

		auto& _pair = mGlobalParamSBDic[name];
		if (_pair.second)
		{
			return false;
		}

		std::lock_guard<std::mutex> lock(mRenderParamMutex);
		_pair.first = sb;
		return true;
	}

	SharedPtr<IStructuredBuffer> RenderParamManager::GetGlobalRenderParamSB(const String& name)
	{
		if (mGlobalParamSBDic.find(name) == mGlobalParamSBDic.end())
		{
			return nullptr;
		}

		std::lock_guard<std::mutex> lock(mRenderParamMutex);
		auto& _pair = mGlobalParamSBDic[name];
		return _pair.first;
	}

	bool RenderParamManager::RegisterGlobalRenderParamBAB(const String& name, SharedPtr<IByteAddressBuffer> initBAB, bool isPrivate)
	{
		if (mGlobalParamBABDic.find(name) != mGlobalParamBABDic.end())
		{
			return false;
		}

		auto& _pair = mGlobalParamBABDic[name];
		_pair.first = initBAB;
		_pair.second = isPrivate;


		return true;
	}

	bool RenderParamManager::SetGlobalRenderParamBAB(const String& name, SharedPtr<IByteAddressBuffer> bab)
	{
		if (mGlobalParamBABDic.find(name) == mGlobalParamBABDic.end())
		{
			return false;
		}

		auto& _pair = mGlobalParamBABDic[name];
		if (_pair.second)
		{
			return false;
		}

		std::lock_guard<std::mutex> lock(mRenderParamMutex);
		_pair.first = bab;
		return true;
	}

	SharedPtr<IByteAddressBuffer> RenderParamManager::GetGlobalRenderParamBAB(const String& name)
	{
		if (mGlobalParamBABDic.find(name) == mGlobalParamBABDic.end())
		{
			return nullptr;
		}

		std::lock_guard<std::mutex> lock(mRenderParamMutex);
		auto& _pair = mGlobalParamBABDic[name];
		return _pair.first;
	}
}
