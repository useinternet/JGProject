#include "PCH/PCH.h"
#include "JWindow.h"

PJWindow::PJWindow(const HJWindowArguments& args)
{
	_args = args;
	ConstructWindow(_args);
}

bool PJWindow::IsValid() const
{
	return GetHandle() != 0;
}

const HJWindowArguments& PJWindow::GetArguments() const
{
	return _args;
}