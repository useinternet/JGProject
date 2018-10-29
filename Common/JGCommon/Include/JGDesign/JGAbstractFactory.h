#pragma once
namespace JGCommon
{

	class JGAbstractFactory
	{
	public:
		JGAbstractFactory() {}
		virtual ~JGAbstractFactory() {}


		template<typename type>
		virtual type* CreateClass() = 0;
	};

}

