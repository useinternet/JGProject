#pragma once



#include "GUI/JGUIComponent/JGUIElementComponent.h"
namespace RE
{
	class Texture;
	class RenderItem;
	class InstanceRenderItem;
	class FixedGShaderModuleClone;
}
class JGUICanvas;
class JGUICamera;
class JGUIElement;
// Image Renderer
class JGENGINE_API JGUIRenderer : public JGUIElementComponent
{
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
	virtual void ChangeParent(JGUIObject* prev_parent) override;
	virtual void LayerProcess();
public:
	virtual void SetActive(bool active) override;
	RE::RenderItem* GetRI() {
		return m_RenderItem;
	}
	void SetOffset(JVector2 offset) {
		m_Offset = offset;
	}
	const JVector2& GetOffset() const {
		return m_Offset;
	}
	void  SetClipRect(const JGUIRect& rect) { m_ClipRect = rect; }
	const JGUIRect& GetClipRect() const { return m_ClipRect; }
protected:
	void DestroyRI();
	void CreateRI();
	void FindCanvas();
	virtual void FindCamera();
protected:
	RE::RenderItem*         m_RenderItem = nullptr;
	RE::InstanceRenderItem* m_Instance = nullptr;
	JVector2                m_Offset = { 0,0 };
	JVector2 m_PrevCameraSize;
	JGUIRect m_ClipRect;
	uint64_t m_RegisterID = 0;



	JGUICanvas* m_OwnerCanvas = nullptr;
	JGUICamera* m_OwnerCamera = nullptr;
};

class JGENGINE_API JGUIShapeRenderer : public JGUIRenderer
{
	enum
	{
		Rectangle,
		EmptyRectangle,
		Circle
	};
protected:
	virtual void Awake() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
	virtual void ChangeParent(JGUIObject* prev_parent) override;
	virtual void Init();
public:
	void SetColor(const JColor& color);
	void SetColor(float r, float g, float b, float a);
	void CreateRectangle();
	void CreateEmptyRectangle(float thick);
	const JColor& GetColor() const;
	int  ShapeType() const {
		return m_ShapeType;
	}
	float GetThickness() const {
		return m_Thick;
	}
protected:
	JColor   m_Color = JColor::White();
	JVector2 m_PrevSize;
	int      m_ShapeType = Rectangle;
	float    m_Thick = 1.0f;
};


class JGENGINE_API JGUIImageRenderer : public JGUIRenderer
{
protected:
	virtual void Awake() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
	virtual void ChangeParent(JGUIObject* prev_parent) override;
	virtual void Init();
public:
	void SetColor(const JColor& color);
	void SetColor(float r, float g, float b, float a);
	void SetImage(const std::string& name);
	
	const JColor& GetColor() const;

protected:
	JColor   m_Color = JColor::White();
	JVector2 m_PrevSize;
	std::string m_Image = "none";
};


class JGENGINE_API JGUIWindowRenderer : public JGUIImageRenderer
{
protected:
	virtual void Init() override;
	virtual void Tick(float tick) override;
	virtual void LayerProcess() override;
	virtual void FindCamera() override;
private:
	JGUIWindow* m_PrevParent = nullptr;
};



