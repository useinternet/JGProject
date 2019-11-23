#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <memory>

struct JWindowDesc
{
	std::string name = "JWindow";
	uint32_t width   = 0;
	uint32_t height  = 0;
};


class JWindow
{
public:
	HWND GetHandle() const;
	JWindowDesc GetDesc();
	void SetClientSize(uint32_t width, uint32_t height);
	void SetPosition(uint32_t x, uint32_t y);

	void AddClientSize(uint32_t width, uint32_t height);
	void AddPosition(uint32_t x, uint32_t y);
	bool Init(const JWindowDesc& desc, uint32_t startX, uint32_t startY);
private:
	HWND m_hWnd;
	JWindowDesc m_Desc;

};



class JWindowManager
{

public:
	JWindowManager();
	~JWindowManager();
	static JWindow* Create(const JWindowDesc& desc, uint32_t startX = 0, uint32_t startY = 0);
	static JWindow* Find(HWND hWnd);
	static JWindow* Find(const std::string& name);
	static void Destroy(HWND hWnd);
	static void Destroy(const std::string& name);
	static void Destroy(JWindow* window);
private:
	std::unordered_map<JWindow*, std::shared_ptr<JWindow>> JWindowPool;
	std::unordered_map<std::string, JWindow*> JWindowMapByName;
	std::unordered_map<HWND, JWindow*>        JWindowMapByHWND;


};