#include "Control.hpp"


Control::Control(const char * name, int taskCore) : Module(name, taskCore) {
}

void Control::connect(void * data) {

}

void Control::run(void* data) {
	this->iterationDelay = 1 / portTICK_PERIOD_MS;
	while (1) {
		vTaskDelay(this->iterationDelay);
	}
}

void Control::handleKey(char key) {
	Serial.printf("key pressed: %c \n",key);
}
