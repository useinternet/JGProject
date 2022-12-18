#pragma once


enum class EMainThreadExecutionOrder
{
	UpdateWindow,
	GraphicsBegin,
	Begin,


	Update,


	End,
	GraphicsEnd,
};