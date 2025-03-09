#include "GraphicLCD.hpp"

GraphicLCD::GraphicLCD(const char * name, int taskCore) : Module(name, taskCore) {
}

void GraphicLCD::connect(void * data) {
    
}

void GraphicLCD::run(void* data) {
	this->iterationDelay = 1 / portTICK_PERIOD_MS;

	while (1) {
		vTaskDelay(this->iterationDelay);
	}
}


