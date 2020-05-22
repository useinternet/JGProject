#pragma once



#include "GUI/JGUIComponent/JGUIElementComponent.h"


class JGUIImageRenderer;
class JGUIShapeRenderer;
class Button;

class JGENGINE_API CheckBox : public JGUIScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;



public:
	void SetBool(bool* bool_ptr);
	void SetCheckImage(const std::string& imagePath);
	void SetCheckBoxImage(const std::string& imagePath);


	const std::string& GetCheckImage() const;
	const std::string& GetCheckBoxImage() const;
private:
	bool* m_BoolPtr = nullptr;
	Button*            m_CheckBt = nullptr;
	JGUIImageRenderer* m_CheckBox = nullptr;
	JGUIImageRenderer* m_Check    = nullptr;

	JGUIShapeRenderer* m_Border = nullptr;

};