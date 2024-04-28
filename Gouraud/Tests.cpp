#include "Tests.h"

std::vector<TestFunction> Tests::functions;

void Tests::runTests()
{
	for (std::vector<TestFunction>::iterator f = functions.begin(); f != functions.end(); f++) {
		TestFunction function = *f;
		(*function.initFunction)();
		(*function.drawFunction)(function.pixels, function.w, function.h, function.pitch, function.tictac);
	}
}
