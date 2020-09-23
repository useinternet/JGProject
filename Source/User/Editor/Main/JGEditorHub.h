#pragma once
#include "Core.h"

class Viewport;
class Detail;

// Test
class DevelopTest;
class UnitTest;
class JGEDITOR_API JGEditorHub
{
private:
	friend class JGEditor;

	static Viewport* viewport;
	static Detail*   detail;

	// Test
	static DevelopTest* developTest;
	static UnitTest*    unitTest;
public:


};
