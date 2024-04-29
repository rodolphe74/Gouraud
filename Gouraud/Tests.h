#pragma once
#include "Application.h"
#include <vector>

typedef void (*INIT_FUNC_PTR)();
typedef void (*CLOSE_FUNC_PTR)();

typedef struct _testFunction
{
	INIT_FUNC_PTR initFunction;
	DRAW_CALL_BACK_PTR drawFunction;
	CLOSE_FUNC_PTR closeFunction;
} TestFunction;

class Tests
{
private:
	static std::vector<TestFunction> functions;
public:
	static void addTests(TestFunction testFunction) {
		functions.push_back(testFunction);
	}
	static void runTests(HINSTANCE hInstance);
};

