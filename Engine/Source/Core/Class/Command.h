#pragma once
#include "JGCore.h"



namespace JG
{




	template<class ... Args> 
	class CommandBase
	{
	protected:
		using Function = std::function<void(Args ... args)>;
		List<ISubscriber*> mSubscriberList;
		List<Function> mFunctionList;
	public:
		void Subscribe(ISubscriber* subscriber, const Function& func)
		{
			mSubscriberList.push_back(subscriber);
			mFunctionList.push_back(func);
		}
		void UnSubscribe(ISubscriber* subscriber)
		{
			u64 cnt = mSubscriberList.size();
			for (u64 i = 0; i < cnt;)
			{
				if (subscriber == mSubscriberList[i])
				{
					std::swap(mSubscriberList.end() - 1, mSubscriberList.begin() + i);
					std::swap(mFunctionList.end() - 1, mFunctionList.begin() + i);
					mSubscriberList.pop_back();
					mFunctionList.pop_back();
					--cnt;
				}
				else ++i;
			}
		}
		void Clear()
		{
			mSubscriberList.clear();
			mFunctionList.clear();
		}
	};
	template<class ... Args>
	class ICommand
	{
	public:
		virtual void Execute(Args... args) = 0;
	};


	template<class ... Args>
	class Command : public CommandBase<Args...>, public ICommand<Args...>
	{
	public:
		virtual void Execute(Args... args) override
		{
			u64 cnt = mSubscriberList.size();
			for (u64 i = 0; i < cnt; ++i)
			{
				if (mFunctionList[i] != nullptr)
				{
					mFunctionList[i](args...);
				}
				
			}
		}
	};

	template<class T>
	class CommandProperty : public CommandBase<T>
	{
		T Value;
	public:
		void SetValue(const T& value)
		{
			if (Value != value)
			{
				u64 cnt = mSubscriberList.size();
				for (u64 i = 0; i < cnt; ++i)
				{
					if (mFunctionList[i] != nullptr)
					{
						mFunctionList[i](value);
					}
				}
				Value = value;
			}
		}
		const T& GetValue() const
		{
			return Value;
		}



	};
}