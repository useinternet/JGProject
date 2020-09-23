#pragma once
#include "Components/NativeScriptComponent.h"




namespace JGUI
{
	class InputField : public NativeScriptComponent
	{

	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;



	};



}