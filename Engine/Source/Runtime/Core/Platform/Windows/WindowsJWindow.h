#pragma once
#include "Platform/JWindow.h"
#include "Memory/Memory.h"
#include <functional>

using WindowsWindowProcCallBack = std::function<LRESULT(HWND, uint32, WPARAM wParam, LPARAM lParam)>;
class PTaskArguments;
class PWindowsJWindow : public PJWindow
{
private:
	static HList<WindowsWindowProcCallBack> _sCallBackList;
	friend LRESULT WndProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

private:
	HJWHandle _handle;

public:
	virtual ~PWindowsJWindow() = default;
	virtual HJWHandle GetHandle() const override;

protected:
	virtual bool ConstructWindow() override;

private:
	void Update(const PTaskArguments& args);

public:
	static void AddWindowProcCallBack(const WindowsWindowProcCallBack& callBackFunc);
};


