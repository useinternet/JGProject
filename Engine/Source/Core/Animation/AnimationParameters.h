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
			bool IsDirty = false;
		};

		Dictionary<String, ParamData> mParamDatas;
	public:
		void SetBool(const String& name, bool value);
		void SetInt(const String& name, i32 value);
		void SetFloat(const String& name, f32 value);
		void SetVector2(const String& name, const JVector2& value);
		void SetVector3(const String& name, const JVector3& value);
		void SetVector4(const String& name, const JVector4& value);


		bool GetBool(const String& name, bool* out_value);
		bool GetInt(const String& name, i32* out_value);
		bool GetFloat(const String& name, f32* out_value);
		bool GetVector2(const String& name, JVector2* out_value);
		bool GetVector3(const String& name, JVector3* out_value);
		bool GetVector4(const String& name, JVector4* out_value);


		// Copy
		SharedPtr<AnimationParameters> Copy() const;
		// Dirty한 부분 이나, 없는 부분만 채워 너음
		void Overwrite(SharedPtr<AnimationParameters> params);
	private:
		void SetData(const String& name, void* data, u64 dataSize);
		bool GetData(const String& name, void* out_data, u64 dataSize);

	};
}