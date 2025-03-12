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
			if(this->ScreenSelected && this->option == 0){
				this->ScreenSelected = false;
			}
			else if(!this->ScreenSelected && this->option == 0){
				this->ScreenSelected = true;
			}

			else if(this->ScreenSelected && !this->optionSelected){
				this->optionSelected  = true;
			}
			else if(this->ScreenSelected && this->optionSelected){
				this->optionSelected  = false;
			}

			if(this->lcd->getScreen() == CONFIG){
				this->enter  = true;
				this->option ++;
				if(this->option > 2) this->option = 0;
			}
			if(this->lcd->getScreen() == MANUAL){
				this->enter  = true;
				this->option ++;
				if(this->option > 4) this->option = 0;
			}
			//this->lcd->splashScreen();
			break;
		case 'A':
			if(this->ScreenSelected){
				this->option++;
			}
			else{
				this->lcd->nextScreen();
				newScreen = this->lcd->getScreen();
				this->option = 0;
			}
			break;
		case 'B':
			if(this->ScreenSelected){
				this->option--;
			}
			else{
				this->lcd->previousScreen();
				newScreen = this->lcd->getScreen();
				this->option = 0;
			}
			break;
		case 'C':
			break;
	}
}
