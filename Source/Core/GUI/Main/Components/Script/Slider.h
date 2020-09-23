#pragma once
#include "Components/NativeScriptComponent.h"




namespace JGUI
{
	class ImageComponent;
	class GUI_API Slider : public NativeScriptComponent
	{

	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		void SetMaxValue(float value) {
			m_MaxValue = value;
		}
		void SetMinValue(float value) {
			m_MinValue = value;
		}
		void SetValue(float value);

		float GetMaxValue() const {
			return m_MaxValue;
		}
		float GetMinValue() const {
			return m_MinValue;
		}
		float GetValue() const {
			return m_Value;
		}

		void SetHandleColor(const JColor& color);
		void SetPressedHandleColor(const JColor& color);
		void SetBackGroundColor(const JColor& color);
		void SetFillAreaColor(const JColor& color);


		const JColor& GetHandleColor() const;
		const JColor& GetPressedHandleColor() const;
		const JColor& GetBackGroundColor() const;
		const JColor& GetFillAreaColor()   const;

	private:
		void Adjust(float ratio);
	private:
		float m_MaxValue = 1.0f;
		float m_MinValue = 0.0f;
		float m_Value    = 0.0f;


		// 배경 
		Element*        m_BackGround = nullptr;
		ImageComponent*        m_BackGroundImage = nullptr;
		NativeScriptComponent* m_BackGroundScript = nullptr;
		// 
		Element* m_FillArea   = nullptr;
		ImageComponent* m_FillImage = nullptr;


		Element* m_Handle     = nullptr;
		ImageComponent*        m_HandleImage = nullptr;
		NativeScriptComponent* m_HandleScript = nullptr;

		//
		bool  m_HandlePressed = false;
		float m_PrevMouseX = 0.0f;


		JColor m_PressedHandleColor = JColor(0.8f, 0.8f, 0.8f, 1.0f);
		JColor m_NormalHandleColor;

		/* 필요한 요소들
		* 버튼
		* 배경
		* 채움 배경
		
		*/

	};

}