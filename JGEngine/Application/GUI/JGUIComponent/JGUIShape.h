#pragma once


#include "GUI/JGUIObject/JGUIComponent.h"



namespace RE
{
	class Texture;
	class RenderItem;
	class InstanceRenderItem;
	class FixedGShaderModuleClone;
}
class JGUIPanel;
class JGUIShape : public JGUIComponent
{

protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void Destroy() override;
	virtual void ParentUpdateNotification() override;
public:
	virtual void SetActive(bool active) override;
public:
	virtual void SetParent(JGUIComponent* parent) override;


	void SetColor(const JColor& color);
	uint64_t GetPriority() const {
		return m_Priority;
	}
protected:
	void DestroyRI();
	void FindPanel();
	virtual void CreateRI();
protected:
	JGUIPanel*              m_OwnerPanel = nullptr;
	RE::RenderItem*         m_RenderItem = nullptr;
	RE::InstanceRenderItem* m_Instance   = nullptr;
	JColor                  m_Color = { 1.0f,1.0f,1.0f,1.0f };
	JVector2                m_Offset = { 0,0 };


	JVector2 m_PrevWindowSize;
	uint64_t m_Priority = 0;
	bool     m_ParentDirty = false;
	bool     m_IsWindowTexture = false;
};



class JGUIWindowTexture : public JGUIShape
{
public:
	void Bind(const std::string& moduleKey, uint64_t parnet_id);
	void UnBind();
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
	uint64_t m_ParentID = -1;
};



class JGUIRectangle : public JGUIShape
{
public:
	void SetImage(const std::string& texture_image);
	void FillOn();
	void EmptyOn();
	void  SetThickness(float thick) { m_Thick = thick; }
	float GetThickness() const      { return m_Thick; }
protected:
	virtual void CreateRI() override;
	virtual void Resize(const JGUIResizeEvent& e) override;

private:
	void CreateMesh();
private:
	std::string m_ImageName;
	bool m_IsFill = true;
	float m_Thick = 5.0f;
};


class JGUIText : public JGUIShape
{
public:
	void SetFont(const std::string& str);
	void SetText(const std::string& str);
	void AddText(const std::string& str);
	void Insert(int idx, const std::string& str);
	void Modify(int idx, const std::string& str);
	void RemoveFirstOf(const std::string& str);
	void RemoveLastOf(const std::string& str);
	void RemoveAll(const std::string& str);
	void RemoveRange(int start_pos, int end_pos);
	void RemoveBack();
	uint32_t Length() const;
	void SetFontSize(float size);
	void SetTextRect(float width, float height);
public:
	const JGUIRect& GetTextRect() const {
		return m_TextRect;
	}
public:
	JGUIText& operator=(const std::string& str);
	JGUIText& operator+=(const std::string& str);
protected:
	virtual void CreateRI() override;
	std::pair<uint32_t, JVector2> ConvertVertex(
		uint32_t ioffset, JVector2 start_pos, const std::string& str,
		std::vector<JGUIVertex>& v, std::vector<uint32_t>& i, std::vector<JVector2>& pos);
	void UpdateTextSize(const std::string& str);
private:
	std::string m_FontName = "";
	std::string m_Text = "";
	float       m_FontSize = 32;
	std::vector<JVector2> m_TextPosMap;

	JGUIRect m_TextRect;
	uint32_t m_IOffset = 0;
	JVector2 m_StartPos;
	int  m_VertexCount = 4;
	int  m_IndexCount = 6;
};
