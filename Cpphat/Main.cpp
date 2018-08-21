#include <thread>
#include "ChatWindow.hpp"

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef _DEBUG
//#define new new(_CLIENT_BLOCK,__FILE__,__LINE)
//#endif

std::chrono::milliseconds GetCurrentMillis() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);
}

int main() {
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ChatWindow window;

	window.Init();

	SocketStartup();

	std::chrono::milliseconds startTime = GetCurrentMillis();
	unsigned long long tickCount = 0;

	while (true) {
		std::chrono::milliseconds startTick = GetCurrentMillis();

		window.Tick();
		Server::Instance.Tick();
		Client::Instance.Tick();

		tickCount++;

		if (startTick + std::chrono::milliseconds(10) > GetCurrentMillis())
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	SocketCleanup();
}