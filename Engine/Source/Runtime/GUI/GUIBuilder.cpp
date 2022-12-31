#include "PCH/PCH.h"
#include "GUIBuilder.h"
#include "Widget.h"

void HGUIBuilder::BeginHorizontal(int32 fixedWidth)
{
	HCommandData commandData;
	commandData.Command = ECommand::BeginHorizontal;

	_commandQueue.push(commandData);
}

void HGUIBuilder::EndHorizontal()
{
	HCommandData commandData;
	commandData.Command = ECommand::EndHorizontal;

	_commandQueue.push(commandData);
}

void HGUIBuilder::BeginVertical(int32 fixedHeight)
{
	HCommandData commandData;
	commandData.Command = ECommand::BeginVertical;

	_commandQueue.push(commandData);
}

void HGUIBuilder::EndVertical()
{
	HCommandData commandData;
	commandData.Command = ECommand::EndVertical;

	_commandQueue.push(commandData);
}