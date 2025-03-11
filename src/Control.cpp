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
	this->lcd->setProgressBarPercentage(0,0);
	this->lcd->setProgressBarPercentage(1,0);
	switch (key){
		case '*':
			//this->lcd->splashScreen();
			break;
		case 'A':
			this->lcd->nextScreen();
			newScreen = this->lcd->getScreen();
			break;
		case 'B':
			this->lcd->previousScreen();
			newScreen = this->lcd->getScreen();
			break;
		case 'C':
			break;
		case 'D':
			if(newScreen == CONFIG){

			}
			else if(newScreen == MANUAL){

			}
			break;
		case '1':
			this->lcd->setProgressBarValue(0,1);
			break;
		case '2':
			this->lcd->setProgressBarValue(0,2);
			break;
		case '3':
			this->lcd->setProgressBarValue(0,3);
			break;
		case '4':
			this->lcd->setProgressBarValue(0,4);
			break;
		case '5':
			this->lcd->setProgressBarValue(0,5);
			break;
		case '6':
			this->lcd->setProgressBarValue(0,6);
			break;
		case '7':
			this->lcd->setProgressBarValue(0,7);
			break;
		case '8':
			this->lcd->setProgressBarValue(0,8);
			break;
		case '9':
			this->lcd->setProgressBarValue(0,9);
			break;
		case '0':
			this->lcd->setProgressBarValue(0,0);
			break;
		}
}
