#include "pch.h"
#include "Dev.h"
#include <comdef.h>


namespace Dev
{
	std::wstring Com_Error(HRESULT hResult)
	{
		_com_error err(hResult);
		LPCTSTR errMsg = err.ErrorMessage();
		return errMsg;
	}
}