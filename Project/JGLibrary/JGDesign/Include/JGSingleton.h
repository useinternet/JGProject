#pragma once

template<typename ClassType>
class JGSingleton
{
private:
	static ClassType* instance;
protected:
	JGSingleton() {}
public:
	virtual ~JGSingleton() {}
	ClassType* Instance()
	{
		if (instance == nullptr)
		{
			instance = new ClassType;
		}
		return instance;
	}
	void Release()
	{
		delete instance;
		instance = nullptr;
	}
};
template<typename ClassType> ClassType* JGSingleton<ClassType>::instance = nullptr;