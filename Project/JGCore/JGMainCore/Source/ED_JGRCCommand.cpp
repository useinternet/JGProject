#include"ED_JGRCCommand.h"
using namespace JGRC;

ED_JGRCCommand::ED_JGRCCommand(EditCommand* edCommand, JGRenderCore* core)
{
	switch (edCommand->Command)
	{
	case ed_AddDxWindow:
		DxWinConfig* config = (DxWinConfig*)edCommand->Data;
		core->AddDxWindow(*config);
		break;
	}
}