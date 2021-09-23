#pragma once
#include "JGCore.h"
#include "Application.h"



namespace JG
{
	class JGEditor : public Application
	{
	
	protected:
		virtual void OpenImpl() override;
		virtual void CloseImpl() override;
		virtual void LoadImpl() override;

		
	};

	
}