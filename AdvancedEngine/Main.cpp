#include <iostream>
#include "Engine.h"
#include <windows.h>
#include <string>

void fixNewLines(std::string& msg) {
	std::string::size_type pos = 0;
	while ((pos = msg.find('\n', pos)) != std::string::npos) {
		msg.replace(pos, 1, "\r\n");
		pos += 2; // move past the inserted "\r\n"
	}

	
}

int main() {

	try {
		std::cout << "Hello, Advanced Engine!" << std::endl;

		Engine* engine = new Engine();

		engine->run();


		return 0;
	}
	catch (const std::exception& e) {
		std::string bottomText = std::string("The game has crashed with the following error: \n\n") + e.what();
		fixNewLines(bottomText);

		MessageBoxA(NULL, bottomText.c_str(), "Unhandled exception: Game crashed!", MB_OK | MB_ICONERROR);

		return 1;
	}


}