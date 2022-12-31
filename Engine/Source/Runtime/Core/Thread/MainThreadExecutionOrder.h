#pragma once


enum class EMainThreadExecutionOrder
{
	GraphicsBegin,
	Begin,


	Update,


	End,
	GraphicsEnd,
	UpdateWindow,
};