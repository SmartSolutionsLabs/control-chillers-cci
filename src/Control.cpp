#include "Control.hpp"

Control::Control(const char *name, int taskCore) : Module(name, taskCore) {
    this->currentMode = AUTOMATIC_MODE;  // Por defecto, el modo es manual
    this->delay1 = 0;                 // Inicializar delay1
    this->delay2 = 0;                 // Inicializar delay2
    this->iterationDelay = 1;
}


void Control::connect(void *data) {
    this->ScreenSelected = false;
    this->optionSelected = false;
    this->currentOption = 0;
    this->maxOptions = new uint8_t[4]; // delay 1, delay 2, 
    this->lcd = (GraphicLCD *)data;  // Asignar el puntero a la pantalla LCD
    this->chillers = new Chiller*[2];
    this->pumps = new Pump*[2];
    this->maxOptions[0] = 0;
    this->maxOptions[1] = 2;
    this->maxOptions[2] = 4;
    this->maxOptions[3] = 0;
}

void Control::run(void *data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;
    this->pumps[0]->setPin(0);
    this->pumps[1]->setPin(1);
    this->chillers[0]->setPin(2);
    this->chillers[1]->setPin(3);

    this->pumps[0]->turnOff();
    this->pumps[1]->turnOff();
    this->chillers[0]->turnOff();
    this->chillers[1]->turnOff();

    bool flag = false;

    while (1) {
        vTaskDelay(this->iterationDelay);

        // Lógica para el modo automático
        if (this->currentMode == AUTOMATIC_MODE) {
    
            if( flag == true){
                this->pumps[0]->turnOn();
                // delay medido por millis ()
                this->chillers[0]->turnOn();
                
            }
            else{
                this->pumps[1]->turnOn();
                // delay medido por millis()
                this->chillers[1]->turnOn();
            }
        }
        else if(this->currentMode == MANUAL_MODE){
           
        }
    }
}

void Control::handleKey(char key) {
    char msg[32];  // Buffer donde sprintf almacenará la cadena
    sprintf(msg, "key pressed: %c", key);

    switch (key) {
        case '*':  // Enter
            proccessEnterKey();
            break;
        case '#':  // Back
            proccessBackKey();
            break;

        case 'A':  // Navegar hacia arriba o aumentar valor
            proccessUpKey();
            break;

        case 'B':  // Navegar hacia abajo o disminuir valor
            proccessDownKey();
            break;
    }
    Serial.printf("|ScreenSelected : %d - optionSelected : %d - option : %d \n", this->ScreenSelected, this->optionSelected, this->currentOption);
}

void Control::proccessEnterKey() {
    if (this->ScreenSelected) {
        // Si estamos en una pantalla y una opción está seleccionada
        if (this->optionSelected) {
            switch (this->currentScreen) {
                case HOME:
                    // En la pantalla HOME, al presionar ENTER, activamos el modo automático
                    this->currentMode = AUTOMATIC_MODE;
                    Serial.printf("Automatic mode activated\n");
                    break;
                case CONFIG:
                    // En la pantalla CONFIG, al presionar ENTER, confirmamos la selección de la opción
                    Serial.printf("Option %d selected in CONFIG\n", this->currentOption);
                    break;
                case MANUAL:
                    // En la pantalla MANUAL, al presionar ENTER, activamos el modo manual
                    this->currentMode = MANUAL_MODE;
                    Serial.printf("Manual mode activated\n");
                    break;
                case LOG:
                    // En la pantalla LOG, al presionar ENTER, no hay acciones específicas
                    Serial.printf("Nothing to do in LOG\n");
                    break;
            }
        } else {
            // Si no hay una opción seleccionada, seleccionamos la opción actual
            this->optionSelected = true;
            Serial.printf("Option %d selected\n", this->currentOption);
        }
    } 
    else {
        // Si no hay una pantalla seleccionada, seleccionamos la pantalla actual
        this->ScreenSelected = true;
        Serial.printf("Screen %d selected\n", this->currentScreen);
        if (this->currentScreen == HOME) {
            this->currentMode = AUTOMATIC_MODE;
            Serial.printf("Automatic mode activated\n");
        } else if (this->currentScreen == MANUAL) {
            this->currentMode = MANUAL_MODE;
            Serial.printf("Manual mode activated\n");
        }
    }

}

void Control::proccessBackKey() {
    if (this->ScreenSelected) {
        if (this->optionSelected) {
            // Si una opción está seleccionada, deseleccionamos la opción
            this->optionSelected = false;
            Serial.printf("Option deselected\n");
        } else {
            // Si no hay una opción seleccionada, deseleccionamos la pantalla
            this->ScreenSelected = false;
            Serial.printf("Screen deselected\n");
        }
    } else {
        // Si no hay una pantalla seleccionada, no hacemos nada
        Serial.printf("Nothing to do\n");
    }
}

void Control::proccessUpKey() {
    if (this->ScreenSelected) {
        if(this->optionSelected){
            switch(this->currentScreen){
                case HOME:
                    Serial.printf("nothing to do in home \n");
                    break;
                case CONFIG:
                    if(this->currentOption == 1){
                        this->delay1 += 1;
                        this->lcd->setProgressBarDelay(0,delay1);
                        this->lcd->update();
                        Serial.printf("delay 1 : %d\n",delay1);
                    }
                    else if(this->currentOption == 2){
                        this->delay1 += 1;
                        this->lcd->setProgressBarDelay(1,delay1);
                        this->lcd->update();
                        Serial.printf("delay 2 : %d\n",delay1);
                    }
            }
        }
        else{
            this->currentOption ++;
            switch(this->currentScreen){  
                case HOME:
                    if(this->currentOption > this->maxOptions[0]) this->currentOption = 1;
                    break;
                case CONFIG:
                    if(this->currentOption > this->maxOptions[1]) this->currentOption = 1;
                    break;
                case MANUAL:
                    if(this->currentOption > this->maxOptions[2]) this->currentOption = 1;
                    break;
                case LOG:
                    if(this->currentOption > this->maxOptions[3]) this->currentOption = 1;
                    break;
            }
        }
    } 
    else {
        this->nextScreen();
    }
}

void Control::proccessDownKey() {
    if (this->ScreenSelected) {
        if(this->optionSelected){
            switch(this->currentScreen){
                case HOME:
                    Serial.printf("nothing to do in home \n");
                    break;
                case CONFIG:
                    if(this->currentOption == 1){
                        this->delay1 -= 1;
                        if(delay1<0)delay1=0;
                        Serial.printf("delay 1 : %d\n",delay1);
                    }
                    else if(this->currentOption == 2){
                        this->delay2 -= 1;
                        if(delay2<0)delay2=0;
                        Serial.printf("delay 2 : %d\n",delay1);
                    }
            }
        }
        else{
            this->currentOption --;
            switch(this->currentScreen){
                case HOME:
                    if(this->currentOption > this->maxOptions[0] || this->currentOption <1) this->currentOption = this->maxOptions[0];
                    break;
                case CONFIG:
                    if(this->currentOption > this->maxOptions[1] || this->currentOption <1) this->currentOption = this->maxOptions[1];
                    break;
                case MANUAL:
                    if(this->currentOption > this->maxOptions[2] || this->currentOption <1) this->currentOption = this->maxOptions[2];
                    break;
                case LOG:
                    if(this->currentOption > this->maxOptions[3] || this->currentOption <1) this->currentOption = this->maxOptions[3];
                    break;
            }
        }
    } 
    else {
        this->previousScreen();
    }
}


void Control::nextScreen(){
    switch(this->currentScreen){
        case HOME:
            this->currentScreen = CONFIG;
            break;
        case CONFIG:
            this->currentScreen = MANUAL;
            break;
        case MANUAL:
            this->currentScreen = LOG;
            break;
        case LOG:
            this->currentScreen = HOME;
            break;
    }
    this->lcd->setScreen(this->currentScreen);    
    this->lcd->update();
    Serial.printf("CurrentScreen: %d  ",this->currentScreen);
}

void Control::previousScreen(){
    switch(this->currentScreen){
        case HOME:
            this->currentScreen = LOG;
            break;
        case CONFIG:
            this->currentScreen = HOME;
            break;
        case MANUAL:
            this->currentScreen = CONFIG;
            break;
        case LOG:
            this->currentScreen = MANUAL;
            break;
    }  
    this->lcd->setScreen(this->currentScreen);  
    this->lcd->update();
    Serial.printf("CurrentScreen: %d  ",this->currentScreen);
}

Screen Control::getScreen(){
    return this->currentScreen;
}

void Control::setChiller(uint8_t index, Chiller *newChiller){
    //Chiller *chillers;
    //chillers = new Chiller[2];
    this->chillers[index] = newChiller;
}
void Control::setPump(uint8_t index, Pump *newPump){
    //Chiller *chillers;
    //chillers = new Chiller[2];
    this->pumps[index] = newPump;
}