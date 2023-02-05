#include "PCH/PCH.h"
#include "GUIBuilder.h"
#include "GUI.h"
#include "Widget.h"

void HGUIBuilder::BeginHorizontal(int32 fixedWidth)
{
	HCommandData commandData;
	commandData.Command = ECommand::BeginHorizontal;
	commandData.CommandValue = Allocate<PHorizontalCommandValue>(fixedWidth);
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
	commandData.CommandValue = Allocate<PVerticalCommandValue>(fixedHeight);
	_commandQueue.push(commandData);
}

void HGUIBuilder::EndVertical()
{
	HCommandData commandData;
	commandData.Command = ECommand::EndVertical;

	_commandQueue.push(commandData);
}

void HGUIBuilder::BeginWidget(PSharedPtr<WWidget> inWidget)
{
	HCommandData commandData;
	commandData.Command = ECommand::BeginWidget;
	commandData.CommandValue = Allocate<PWidgetCommandValue>(inWidget);

	_commandQueue.push(commandData);
}

void HGUIBuilder::EndWidget()
{
	HCommandData commandData;
	commandData.Command = ECommand::EndWidget;

	_commandQueue.push(commandData);
}

const HQueue<HGUIBuilder::HCommandData>& HGUIBuilder::GetCommandQueue() const
{
	return _commandQueue;
}

void HGUIBuilder::PushWidgetComponent(PSharedPtr<WWidgetComponent> inWidgetCom)
{
	HCommandData commandData;
	commandData.Command      = ECommand::PushWidgetComponent;
	commandData.CommandValue = Allocate<PWidgetComponentCommandValue>(inWidgetCom);

	_commandQueue.push(commandData);
}

void HGUIBuilder::PushGenerateNativeGUI(PSharedPtr<WWidgetComponent> inWidgetCom, const POnGenerateNativeGUI& OnGenerateGUI)
{
	HCommandData commandData;
	commandData.Command = ECommand::PushGenerateNatvieGUI;
	commandData.CommandValue = Allocate<PGenerateNativeGUICommandValue>(inWidgetCom, OnGenerateGUI);

	_commandQueue.push(commandData);
}
