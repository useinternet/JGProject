#pragma once
namespace JGDesign
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

