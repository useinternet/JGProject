#pragma once

#include "GUI/JGUIObject/JGUIComponent.h"
#include <set>
class JGUIRectTransform : public JGUIComponent
{
	friend JGUIObject;
public:
	JGUIRectTransform(
		const JVector2& pos = { 0.0f,0.0f },
		float angle = 0.0f, 
		const JVector2 & scale = { 1.0f,1.0f });
protected:
	virtual void Destroy() override;
public:
	void AttachTransform(JGUIRectTransform* transform);
	void DetachTransform(JGUIRectTransform* transform);
	void ClearAttachedTransform();
	virtual void SetLocalPosition(const JVector2& pos);
	virtual void SetLocalPosition(float x, float y);
	virtual void OffsetLocalPosition(const JVector2& offset);
	virtual void OffsetLocalPosition(float x, float y);



	void SetLocalAngle(float angle);
	void OffsetLocalAngle(float angle);


	void SetLocalScale(const JVector2& scale);
	void SetLocalScale(float x, float y);
	void OffsetLocalScale(const JVector2& scale);
	void OffsetLocalScale(float x, float y);


	virtual void SetSize(const JVector2& size);
	virtual void SetSize(float x, float y);
	virtual void OffsetSize(const JVector2& offset);
	virtual void OffsetSize(float x, float y);

	//void SetLocalPivot(const JVector2& pivot);
	//void SetLocalPivot(float x, float y);
	//void OffsetLocalPivot(const JVector2& pivot);
	//void OffsetLocalPivot(float x, float y);

	bool IsDirty() const;
	void SendDirty();
	void Flush();
public:
	const JVector2& GetLocalPosition() const;
	const JVector2& GetPosition() const;
	float    GetLocalAngle() const;
	float    GetAngle() const;
	const JVector2& GetScale() const;
	const JVector2& GetSize()  const;
	const JVector2& GetPivot() const;
	const JVector2& GetLocalPivot() const;
	JGUIRect GetRect()  const;
	JGUIRect GetLocalRect() const;
private:
	void SendDirty(int n);
	void SendAttachedTransform_Size(const JVector2& pivot);
	void SendAttachedTransform_Scale(const JVector2& scale);
	void BindComponent(JGUIComponent* com);
	void BindWindow(JGUIWindow* win);
protected:
	JVector2    m_LocalPosition;
	float       m_LocalAngle;
	JVector2    m_LocalPivot;
	JVector2    m_Scale;
	JVector2    m_Size;
	mutable JVector2 m_Pivot;
	mutable JVector2 m_Position;
	mutable float    m_Angle = 0.0f;

	mutable bool m_IsDirty[4];
	enum
	{
		PosDirty = 0,
		AngleDirty = 1,
		PivotDirty = 2,
		UpdateDirty = 3,
		DirtyCount = 4
	};
	std::set<JGUIRectTransform*> m_AttachedList; // 연결된 트랜스폼
	JGUIRectTransform* m_Attacher = nullptr; // 연결한 트랜스폼

	//
	JGUIComponent* m_BindedComponent = nullptr;
	JGUIWindow*    m_BindedWindow    = nullptr;
};

class JGUIWinRectTransform : public JGUIRectTransform
{
	friend class JGUIWindow;
protected:


public:
	virtual void SetLocalPosition(const JVector2& pos) override;
	virtual void SetLocalPosition(float x, float y) override;
	virtual void OffsetLocalPosition(const JVector2& offset) override;
	virtual void OffsetLocalPosition(float x, float y) override;




	virtual void SetSize(const JVector2& size) override;
	virtual void SetSize(float x, float y) override;
	virtual void OffsetSize(const JVector2& offset) override;
	virtual void OffsetSize(float x, float y) override;
private:
	void SendPosToWin();
	void SendSizeToWin();
};