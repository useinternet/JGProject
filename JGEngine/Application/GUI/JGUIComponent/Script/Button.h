#pragma once

#include "GUI/JGUIComponent/JGUIElementComponent.h"


class JGENGINE_API Button : public JGUIScriptComponent
{
public:
	enum
	{
		Normal,
		Hightlight,
		Pressed,
		Disabled,
		BtStateCount
	};
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	void BindOnClick(const std::function<void()>& func);
	void SetImage(int btState, const std::string& image);
	void SetImage(const std::string& image);
	void SetColor(int btState, const JColor& color);
	void Reset();



	const JColor& GetColor(int btState) const {
		return m_BtColors.at(btState);
	}
	const std::string& GetImage(int btState) const {
		return m_ImageSources.at(btState);
	}

private:
	void SetBtState(int state);
private:
	std::function<void()> m_OnClick;


	class JGUIImageRenderer*   m_Image = nullptr;
	std::map<int, std::string> m_ImageSources;
	std::map<int, JColor>      m_BtColors;

	int  m_BtState = Normal;
	bool m_IsClick = false;
};