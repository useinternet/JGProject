#pragma once

#include "JGCore.h"


namespace JG
{
	class IComputeContext;
	class GaussianFilter
	{
	public:
		struct Input
		{

		};
		struct Output
		{

		};
	private:



	public:
		Output Execute(SharedPtr<IComputeContext> context, const Input& input);


	};
}