#pragma once
#include "GEObject.h"



namespace GE
{
	class PassFlag
	{
	public:
		uint64_t flag1 = 0;
		uint64_t flag2 = 0;
		uint64_t flag3 = 0;
		uint64_t flag4 = 0;
	public:
		bool Contain(const PassFlag& flag) const
		{
			return (flag1 & flag.flag1) || (flag2 & flag.flag2) || (flag3 & flag.flag3) || (flag4 & flag.flag4);
		}
		PassFlag& operator|=(const PassFlag& flag) {

			flag1 |= flag.flag1;
			flag2 |= flag.flag2;
			flag3 |= flag.flag3;
			flag4 |= flag.flag4;
			return *this;
		}
		PassFlag& operator&=(const PassFlag& flag) {
			flag1 &= flag.flag1;
			flag2 &= flag.flag2;
			flag3 &= flag.flag3;
			flag4 &= flag.flag4;
			return *this;
		}
		PassFlag& operator~()
		{
			flag1 = ~flag1;
			flag2 = ~flag2;
			flag3 = ~flag3;
			flag4 = ~flag4;
			return *this;
		}
	};

	inline PassFlag operator|(const PassFlag& flag1, const PassFlag& flag2)
	{
		PassFlag flag;
		flag.flag1 = flag1.flag1 | flag2.flag1;
		flag.flag2 = flag1.flag2 | flag2.flag2;
		flag.flag3 = flag1.flag3 | flag2.flag3;
		flag.flag4 = flag1.flag4 | flag2.flag4;
		return flag;
	}
	inline PassFlag operator&(const PassFlag& flag1, const PassFlag& flag2)
	{
		PassFlag flag;
		flag.flag1 = flag1.flag1 & flag2.flag1;
		flag.flag2 = flag1.flag2 & flag2.flag2;
		flag.flag3 = flag1.flag3 & flag2.flag3;
		flag.flag4 = flag1.flag4 & flag2.flag4;
		return flag;
	}

	class RenderingPass
	{
	public:
		virtual bool SetUp(void* userData) = 0;
		virtual bool Run(void* userData) = 0;



		virtual void  SetName(const std::string& name) = 0;
		virtual const std::string& GetName() const = 0;
		virtual GE::PassFlag GetFlags() const {
			return m_Flag;
		}
		virtual ~RenderingPass() {}

	protected:
		GE::PassFlag   m_Flag;
	};
}