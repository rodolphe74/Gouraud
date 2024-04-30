#include "Tests.h"
#include "Application.h"
#include "thread"
#include <Windows.h>

std::vector<TestFunction> Tests::functions;

void Tests::runTests(HINSTANCE hInstance)
{
	std::vector<TestFunction>::iterator f = functions.begin();
	TestFunction function = *f;

	Application a;
	a.waitRR(false);
	a.createWindow(hInstance, "Minimal DX window", 640, 640);

	(*function.initFunction)();


	std::thread t([&] {
		for (int i = 0; i < functions.size() - 1; i++) {

			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			(*function.closeFunction)();	// close current

			f++;
			function = *f;

			(*function.initFunction)();		// init next
			a.setDrawFunction((*function.drawFunction));
		}
		
		});

	a.messagesLoop(*function.drawFunction);
	t.detach();

}