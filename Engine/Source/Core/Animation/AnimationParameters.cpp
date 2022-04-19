#include "pch.h"
#include "AnimationParameters.h"

namespace JG
{
	void AnimationParameters::SetBool(const String& name, bool value)
	{
	}

	void AnimationParameters::SetInt(const String& name, i32 value)
	{
	}

	void AnimationParameters::SetFloat(const String& name, f32 value)
	{
	}

	void AnimationParameters::SetVector2(const String& name, const JVector2& value)
	{
	}

	void AnimationParameters::SetVector3(const String& name, const JVector3& value)
	{
	}

	void AnimationParameters::SetVector4(const String& name, const JVector4& value)
	{
	}

	bool AnimationParameters::GetBool(const String& name, bool* out_value)
	{
		return false;
	}

	bool AnimationParameters::GetInt(const String& name, i32* out_value)
	{
		return false;
	}

	bool AnimationParameters::GetFloat(const String& name, f32* out_value)
	{
		return false;
	}

	bool AnimationParameters::GetVector2(const String& name, JVector2* out_value)
	{
		return false;
	}

	bool AnimationParameters::GetVector3(const String& name, JVector3* out_value)
	{
		return false;
	}

	bool AnimationParameters::GetVector4(const String& name, JVector4* out_value)
	{
		return false;
	}

	SharedPtr<AnimationParameters> AnimationParameters::Copy() const
	{
		// allUpdate ½Ã ¸ðµç 
		return SharedPtr<AnimationParameters>();
	}

	void AnimationParameters::Overwrite(SharedPtr<AnimationParameters> params)
	{



	}

	void AnimationParameters::SetData(const String& name, void* data, u64 dataSize)
	{
	}

	bool AnimationParameters::GetData(const String& name, void* out_data, u64 dataSize)
	{
		return false;
	}

}