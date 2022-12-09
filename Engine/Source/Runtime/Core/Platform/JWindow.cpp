#include "PCH/PCH.h"
#include "JWindow.h"

bool PJWindow::IsValid() const
{
	return GetHandle() != 0;
}

const HJWindowArguments& PJWindow::GetArguments() const
{
	return _args;
}


HJWindowCreater::HJWindowCreater(PJWindow* pWindow, const HJWindowArguments& args)
{
	_bIsSuccess = true;

	if (pWindow == nullptr)
	{
		_bIsSuccess = false;
		return;
	}

	pWindow->_args = args;
	_bIsSuccess = pWindow->ConstructWindow();
}

bool HJWindowCreater::IsSuccess() const
{
	return _bIsSuccess;
}