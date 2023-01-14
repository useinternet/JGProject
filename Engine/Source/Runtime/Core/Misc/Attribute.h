#pragma once
#include "Misc/Delegate.h"



template<class T>
class HAttribute
{
public:
	JG_DECLARE_DELEGATE_RETVAL(T, POnProvideData);

private:
	T _data;
	POnProvideData _provideData;

public:
	HAttribute() = default;
	HAttribute(const T& inData)
	{
		SetValue(inData);
	}

	HAttribute(const POnProvideData& inDelegate)
	{
		Bind(inDelegate);
	}

	HAttribute<T>& operator=(const T& inData)
	{
		SetValue(inData);
	}

	HAttribute<T>& operator=(const POnProvideData& inDelegate)
	{
		Bind(inDelegate);
	}

	void Bind(const POnProvideData& inDelegate)
	{
		_provideData = inDelegate;
	}

	void UnBind()
	{
		_provideData.Reset();
	}

	void GetValue(T& outValue) const
	{
		if (_provideData.IsBound())
		{
			outValue = _provideData.Execute();
		}
		else
		{
			_data = outValue;
		}
	}

	void SetValue(const T& inData)
	{
		UnBind();
		_data = inData;
	}
};