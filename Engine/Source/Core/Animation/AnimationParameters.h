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
			List<jbyte> Data;
			EAnimationParameterType ParamType;
		};
		
		Dictionary<String, ParamData> mParamDatas;
	public:
		bool SetBool(const String& name, bool value);
		bool SetInt(const String& name, i32 value);
		bool SetFloat(const String& name, f32 value);
		bool SetTrigger(const String& name, bool value = true);

		bool GetBool(const String& name, bool* out_value);
		bool GetInt(const String& name, i32* out_value);
		bool GetFloat(const String& name, f32* out_value);

		void ForEach_Trigger(const std::function<void(const String& name)>& action);
		void ForEach_Bool(const std::function<void(const String& name)>& action);
		void ForEach_Float(const std::function<void(const String& name)>& action);
		void ForEach_Int(const std::function<void(const String& name)>& action);

		// Copy
		SharedPtr<AnimationParameters> Copy() const;
		bool SetData(EAnimationParameterType type, const String& name, const void* data);
		bool GetData(EAnimationParameterType type, const String& name, void* out_data);
		
		u64 GetTypeDataSize(EAnimationParameterType type);
	};
}