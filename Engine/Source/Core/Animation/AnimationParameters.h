#pragma once
#include "JGCore.h"
#include "AnimationDefines.h"


namespace JG
{
	class AnimationParameters
	{
		struct ParamData
		{
			String Name;
			List<byte> Data;
			EAnimationParameterType ParamType;
		};

		Dictionary<String, ParamData> mParamDatas;
	public:
		bool SetBool(const String& name, bool value);
		bool SetInt(const String& name, i32 value);
		bool SetFloat(const String& name, f32 value);


		bool GetBool(const String& name, bool* out_value);
		bool GetInt(const String& name, i32* out_value);
		bool GetFloat(const String& name, f32* out_value);


		// Copy
		SharedPtr<AnimationParameters> Copy() const;
	private:
		bool SetData(EAnimationParameterType type, const String& name, const void* data);
		bool GetData(EAnimationParameterType type, const String& name, void* out_data);

		u64 GetTypeDataSize(EAnimationParameterType type);
	};
}