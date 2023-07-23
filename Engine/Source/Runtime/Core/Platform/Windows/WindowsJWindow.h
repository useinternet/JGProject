#pragma once
#include "Platform/JWindow.h"
#include "Memory/Memory.h"
#include <functional>


class PTaskArguments;
class PWindowsJWindow : public PJWindow
{
	friend LRESULT WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

private:
	HJWHandle _handle;

public:
	virtual ~PWindowsJWindow() = default;
	virtual HJWHandle GetHandle() const override;
	virtual HVector2Int GetClientSize() const override;
protected:
	virtual bool ConstructWindow() override;

private:
	void Update();
};


