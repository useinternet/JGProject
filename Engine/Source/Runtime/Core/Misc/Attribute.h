#pragma once
#include "Misc/Delegate.h"



template<class T>
class HAttribute
{
public:
	JG_DECLARE_DELEGATE_RET(HOnProvideData, T);

private:
	T _data;
	HOnProvideData _provideData;

public:
	HAttribute() = default;
	HAttribute(const T& inData)
	{
		SetValue(inData);
	}

	HAttribute(const HOnProvideData& inDelegate)
	{
		Bind(inDelegate);
	}

	HAttribute<T>& operator=(const T& inData)
	{
		SetValue(inData);

		return *this;
	}

	HAttribute<T>& operator=(const HOnProvideData& inDelegate)
	{
		Bind(inDelegate);

		return *this;
	}

	void Bind(const HOnProvideData& inDelegate)
	{
		_provideData = inDelegate;
	}

	void UnBind()
	{
		_provideData.Clear();
	}

	T GetValue() const
	{
		T value;
		GetValue(value);
		return value;
	}

	void GetValue(T& outValue) const
	{
		if (_provideData.IsBound())
		{
			outValue = _provideData.Execute();
		}
		else
		{
			outValue = _data;
		}
	}

	void SetValue(const T& inData)
	{
		UnBind();
		_data = inData;
	}
};