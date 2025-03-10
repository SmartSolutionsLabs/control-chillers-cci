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
	char msg[32];  // Buffer donde sprintf almacenará la cadena
    sprintf(msg, "key pressed: %c", key);
	
	switch (key){
		case '*':
			this->lcd->splashScreen();
			break;
		case 'A':
			this->lcd->print(20,40,msg);
			break;
		}
}
