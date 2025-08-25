#include <iostream>
#include "Engine.h"

int main() {
	std::cout << "Hello, Advanced Engine!" << std::endl;

	Engine* engine = new Engine();

	engine->run();


	return 0;
}