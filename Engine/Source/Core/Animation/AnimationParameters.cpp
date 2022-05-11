#include "AnimationParameters.h"
#include "AnimationParameters.h"
#include "AnimationParameters.h"
#include "pch.h"
#include "AnimationParameters.h"

namespace JG
{
	bool AnimationParameters::SetBool(const String& name, bool value)
	{
		return SetData(EAnimationParameterType::Bool, name, &value);
	}

	bool AnimationParameters::SetInt(const String& name, i32 value)
	{
		return SetData(EAnimationParameterType::Int, name, &value);
	}

	bool AnimationParameters::SetFloat(const String& name, f32 value)
	{
		return SetData(EAnimationParameterType::Float, name, &value);
	}


	bool AnimationParameters::GetBool(const String& name, bool* out_value)
	{
		return GetData(EAnimationParameterType::Bool, name, out_value);
	}

	bool AnimationParameters::GetInt(const String& name, i32* out_value)
	{
		return GetData(EAnimationParameterType::Int, name, out_value);
	}

	bool AnimationParameters::GetFloat(const String& name, f32* out_value)
	{
		return GetData(EAnimationParameterType::Float, name, out_value);
	}


	SharedPtr<AnimationParameters> AnimationParameters::Copy() const
	{
		SharedPtr<AnimationParameters> result = CreateSharedPtr<AnimationParameters>();
		result->mParamDatas = mParamDatas;
		return result;
	}


	bool AnimationParameters::SetData(EAnimationParameterType type, const String& name, const void* data)
	{
		if (type == EAnimationParameterType::Unknown)
		{
			return false;
		}
		if (mParamDatas.find(name) != mParamDatas.end())
		{
			
			ParamData& paramData = mParamDatas[name];

			if (paramData.ParamType != type)
			{
				return false;
			}
			u64 dataSize = GetTypeDataSize(type);

			memcpy(paramData.Data.data(), data, dataSize);
		}
		else
		{
			u64 dataSize = GetTypeDataSize(type);


			ParamData& paramData = mParamDatas[name];
			paramData.Name = name;
			paramData.Data.resize(dataSize);
			paramData.ParamType = type;
			memcpy(paramData.Data.data(), data, dataSize);
		}
		
		return true;
	}

	bool AnimationParameters::GetData(EAnimationParameterType type, const String& name,  void* out_data)
	{
		if (mParamDatas.find(name) == mParamDatas.end() || type == EAnimationParameterType::Unknown || out_data == nullptr)
		{
			return false;
		}
		ParamData& paramData = mParamDatas[name];
		if (paramData.ParamType != type)
		{
			return false;
		}
		u64 dataSize = GetTypeDataSize(type);

		memcpy(out_data, paramData.Data.data(), dataSize);


		return true;
	}

	u64 AnimationParameters::GetTypeDataSize(EAnimationParameterType type)
	{
		switch (type)
		{
		case EAnimationParameterType::Bool:		return sizeof(bool);
		case EAnimationParameterType::Float:	return sizeof(f32);
		case EAnimationParameterType::Int:		return sizeof(i32);
		default:
			return 0;
		}
	}

}