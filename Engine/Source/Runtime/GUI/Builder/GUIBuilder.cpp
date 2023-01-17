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

void HGUIBuilder::Text(const PString& inText)
{
	HCommandData commandData;
	commandData.Command = ECommand::Text;
	commandData.CommandValue = Allocate<PTextCommandValue>(inText);

	_commandQueue.push(commandData);
}

void HGUIBuilder::Button(const PString& inLabel, const HVector2& inSize)
{
	HCommandData commandData;
	commandData.Command = ECommand::Button;
	commandData.CommandValue = Allocate<PButtonCommandValue>(inLabel, inSize);

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

void HGUIBuilder::PushWidgetComponent(PSharedPtr<WWidgetComponent> inWidgetCom)
{
	HCommandData commandData;
	commandData.Command      = ECommand::PushWidgetComponent;
	commandData.CommandValue = Allocate<PWidgetComponentCommandValue>(inWidgetCom);

	_commandQueue.push(commandData);
}

void HGUIBuilder::Build()
{
	const HCoreSystemGlobalValues& globalValues = GCoreSystem::GetGlobalValues();
	IGUIBuild* guiBuild = globalValues.GUIBuild;
	if (guiBuild == nullptr)
	{
		return;
	}

	guiBuild->PushData(_commandQueue);
}