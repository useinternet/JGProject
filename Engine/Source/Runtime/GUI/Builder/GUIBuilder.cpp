#include "PCH/PCH.h"
#include "GUIBuilder.h"
#include "Widgets/Widget.h"


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

void HGUIBuilder::Text(const PString& inText)
{
}

void HGUIBuilder::PushWidget(PSharedPtr<WWidget> inWidget)
{
	HCommandData commandData;
	commandData.Command      = ECommand::PushWidget;
	commandData.CommandValue = Allocate<PWidgetCommandValue>(inWidget);

	_commandQueue.push(commandData);
}

void HGUIBuilder::Build()
{
	if (_guiBuild == nullptr)
	{
		return;
	}

	while (_commandQueue.empty() == false)
	{
		_guiBuild->OnBuild(_commandQueue.front());
		_commandQueue.pop();
	}
}