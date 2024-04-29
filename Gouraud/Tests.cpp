#include "Tests.h"
#include "Application.h"
#include "thread"
#include <Windows.h>

std::vector<TestFunction> Tests::functions;

void Tests::runTests(HINSTANCE hInstance)
{
	for (std::vector<TestFunction>::iterator f = functions.begin(); f != functions.end() - 1; f++) {
		TestFunction function = *f;
		TestFunction nextFunction = *(std::next(f));

		Application a;
		a.waitRR(false);
		a.createWindow(hInstance, "Minimal DX window", 640, 640);

		std::thread t1([&] {
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			(*function.closeFunction)();		// close current
			(*nextFunction.initFunction)();		// init next
			a.setDrawFunction((*nextFunction.drawFunction));
			});

		(*function.initFunction)();
		a.messagesLoop(*function.drawFunction);
		t1.detach();
	}

	
}