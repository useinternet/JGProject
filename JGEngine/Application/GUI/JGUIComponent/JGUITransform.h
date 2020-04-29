#pragma once
#include "JGUIComponent.h"
#include <set>


class  JGUIWindow;
class  JGUIElement;
class JGENGINE_API JGUITransform : public JGUIComponent
{
	friend JGUI;
	friend JGUIWindow;
	friend JGUIElement;
protected:
	virtual void Awake() override;
	virtual void Destroy() override;
public:
	void AttachTransform(JGUITransform* transform);
	void DetachTransform(JGUITransform* transform);
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


	void SetPivot(const JVector2& pivot);
	void SetPivot(float x, float y);
	void OffsetPivot(const JVector2& offset);
	void OffsetPivot(float x, float y);

	bool IsDirty() const;
	void SendDirty();
	void SendPosDirty();
	void SendAngleDirty();
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
	void SendAttachedTransform_Size(const JVector2& size);
	void SendAttachedTransform_Scale(const JVector2& scale);
	void SendPosToWin();
	void SendSizeToWin();
	void SendSizeToElement();
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
	std::set<JGUITransform*> m_AttachedList; // 연결된 트랜스폼
	JGUITransform* m_Attacher = nullptr; // 연결한 트랜스폼
};