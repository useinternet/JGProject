#pragma once

typedef struct EditCommand
{
	unsigned int Command;
	void* Data;
	unsigned int Size;
}EditCommand;

enum ED2EGCommand
{
	ed_AddDxWindow
};