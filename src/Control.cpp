#include "Control.hpp"

Control::Control(const char *name, int taskCore) : Module(name, taskCore) {
    this->currentMode = AUTOMATIC_MODE;  // Por defecto, el modo es automático
    this->delay[0] = 60;                   // Inicializar delay1
    this->delay[1] = 60;                   // Inicializar delay2
    this->delayCounter[0] = 0;
    this->delayCounter[1] = 0;
    this->flag_process[0] = false;
    this->flag_process[1] = false;
    this->iterationDelay = 1;
    Serial.println("Control initialized");
}

void Control::connect(void *data) {
    this->chillerMode = NONE_SELECTED;
    this->ScreenSelected = false;
    this->optionSelected = false;
    this->currentOption = 1;
    this->maxOptions = new uint8_t[4]; // delay 1, delay 2, 
    this->lcd = (GraphicLCD *)data;    // Asignar el puntero a la pantalla LCD

    // ✅ CREAR NUEVAS INSTANCIAS DE Pump[]
    this->pumps = new Pump*[2];  // Se asigna memoria para el array de punteros
    this->pumps[0] = new Pump("Pump0", 1);  // Nueva instancia de Pump
    this->pumps[1] = new Pump("Pump1", 1);  // Nueva instancia de Pump

    // ✅ CREAR NUEVAS INSTANCIAS DE Chiller[]
    this->chillers = new Chiller*[2];
    this->chillers[0] = new Chiller("Chiller0", 1);
    this->chillers[1] = new Chiller("Chiller1", 1);


    this->maxOptions[0] = 0;
    this->maxOptions[1] = 2;
    this->maxOptions[2] = 4;
    this->maxOptions[3] = 0;

    Serial.println("Control connected to LCD");
}

void Control::run(void *data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;

    // ✅ Asegurar que pumps[0] y pumps[1] son objetos diferentes
    if (this->pumps[0] && this->pumps[1]) {
        this->pumps[0]->setPin(1);
        this->pumps[1]->setPin(2);
    }

    if (this->chillers[0] && this->chillers[1]) {
        this->chillers[0]->setPin(3);
        this->chillers[1]->setPin(4);
    }

    this->lcd->setMotorState(0, false);
    this->lcd->setMotorState(1, false);
    this->lcd->setChillerState(0, false);
    this->lcd->setChillerState(1, false);

    
    this->lcd->setProgressBarValue(1,0);
    this->lcd->setProgressBarValue(2,0);

    this->lcd->getProgressBar(1).setTimer(this->delay[0]);
    this->lcd->getProgressBar(2).setTimer(this->delay[1]);

    this->pumps[0]->toggle(this->GPIOA);
    this->pumps[1]->toggle(this->GPIOA);
    //Serial.printf("Pump 0 Pin: %d, Pump 1 Pin: %d\n", 
    //    this->pumps[0]->getPin(), this->pumps[1]->getPin());

    bool flag = false;

    while (1) {
        vTaskDelay(this->iterationDelay);

        // Lógica para el modo automático
        if (this->currentMode == AUTOMATIC_MODE) {
            processChiller();
        }
    }
}

void Control::handleKey(char key) {
    Serial.printf("Key pressed: %c\n", key);

    switch (key) {
        case 'A':  // Enter
            proccessEnterKey();
            break;
        case 'B':  // Back
            proccessBackKey();
            break;
        case 'C':  // Navegar hacia arriba o aumentar valor
            proccessUpKey();
            break;
        case 'D':  // Navegar hacia abajo o disminuir valor
            proccessDownKey();
            break;
        case 'E':  // Navegar hacia abajo o disminuir valor
            setProcessChiller(1);
            break;
        case 'F':  // Navegar hacia abajo o disminuir valor
            setProcessChiller(2);
            break;
        case 'G':  // Navegar hacia abajo o disminuir valor
            setProcessChiller(0);
            break;
        case 'H':  // Navegar hacia abajo o disminuir valor
            setProcessChiller(0);
            break;
    }
    Serial.printf("ScreenSelected: %d, optionSelected: %d, currentOption: %d\n", 
                  this->ScreenSelected, this->optionSelected, this->currentOption);
}

void Control::proccessEnterKey() {
    if (this->ScreenSelected) {
        if (this->optionSelected) {
            switch (this->currentScreen) {
                case HOME:
                    this->currentMode = AUTOMATIC_MODE;
                    Serial.println("Automatic mode activated");
                    break;
                case CONFIG:
                    if (this->currentOption == 1) {
                        this->lcd->getProgressBar(0).setSelected(true);
                        this->lcd->getProgressBar(0).setNavigated(false);
                        this->lcd->getProgressBar(1).setSelected(false);
                        this->lcd->getProgressBar(1).setNavigated(false);
                    } else if (this->currentOption == 2) {
                        this->lcd->getProgressBar(0).setSelected(false);
                        this->lcd->getProgressBar(0).setNavigated(false);
                        this->lcd->getProgressBar(1).setSelected(true);
                        this->lcd->getProgressBar(1).setNavigated(false);
                    }
                    break;
                case MANUAL:
                    this->currentMode = MANUAL_MODE;
                    Serial.println("Manual mode activated");
                    this->manual();
                    // Activar/desactivar dispositivos según la opción seleccionada
                    break;
                case LOG:
                    Serial.println("Nothing to do in LOG");
                    break;
            }
            //this->lcd->update();  // Actualizar la pantalla después de confirmar la selección
        } else {
            // Si no hay una opción seleccionada, seleccionamos la opción actual
            this->optionSelected = true;
            Serial.printf("Option %d selected\n", this->currentOption);

            // Actualizar el estado a SELECTED
            //this->lcd->getTextInput(this->currentOption - 1)->setState(InputState::SELECTED);

            //this->lcd->update();  // Actualizar la pantalla después de seleccionar la opción
        }
    } else {
        // Si no hay una pantalla seleccionada, seleccionamos la pantalla actual
        this->ScreenSelected = true;
        Serial.printf("Screen %d selected\n", this->currentScreen);
        if (this->currentScreen == HOME) {
            this->currentMode = AUTOMATIC_MODE;
            Serial.println("Automatic mode activated");
        } else if (this->currentScreen == MANUAL) {
            this->currentMode = MANUAL_MODE;
            Serial.println("Manual mode activated");
        }
        //this->lcd->update();  // Actualizar la pantalla después de seleccionar la pantalla
    }
}

void Control::proccessBackKey() {
    if (this->ScreenSelected) {
        if (this->optionSelected) {
            // Si una opción está seleccionada, deseleccionamos la opción
            this->optionSelected = false;
            Serial.println("Option deselected");
        } else {
            // Si no hay una opción seleccionada, deseleccionamos la pantalla
            this->ScreenSelected = false;
            Serial.println("Screen deselected");
        }
        //this->lcd->update();  // Actualizar la pantalla después de deseleccionar
    } else {
        Serial.println("Nothing to do");
    }
}

void Control::proccessUpKey() {
    if (this->ScreenSelected) {
        if (this->optionSelected) {
            switch (this->currentScreen) {
                case HOME:
                    Serial.println("Nothing to do in home");
                    break;
                case CONFIG:
                    if (this->currentOption == 1) {
                        this->delay[0] += 1;
                        this->lcd->setProgressBarDelay(1, delay[0]);
                        Serial.printf("Delay 1: %d\n", delay[0]);
                    } else if (this->currentOption == 2) {
                        this->delay[1] += 1;
                        this->lcd->setProgressBarDelay(2, delay[1]);
                        Serial.printf("Delay 2: %d\n", delay[1]);
                    }
                    //this->lcd->update();
                    break;
            }
        } else {
            this->currentOption++;
            switch (this->currentScreen) {
                case CONFIG:
                    Serial.println("Navigating config");
                    if (this->currentOption == 1) {
                        this->lcd->getProgressBar(0).setNavigated(true);
                        this->lcd->getProgressBar(0).setSelected(false);
                        this->lcd->getProgressBar(1).setNavigated(false);
                        this->lcd->getProgressBar(1).setSelected(false);
                    } else if (this->currentOption == 2) {
                        this->lcd->getProgressBar(0).setNavigated(false);
                        this->lcd->getProgressBar(0).setSelected(false);
                        this->lcd->getProgressBar(1).setNavigated(true);
                        this->lcd->getProgressBar(1).setSelected(false);
                    }
                    //this->lcd->update();
                    break;
            }

            switch (this->currentScreen) {
                case HOME:
                    if (this->currentOption > this->maxOptions[0]) this->currentOption = 1;
                    break;
                case CONFIG:
                    if (this->currentOption > this->maxOptions[1]) this->currentOption = 1;
                    break;
                case MANUAL:
                    if (this->currentOption > this->maxOptions[2]) this->currentOption = 1;
                    break;
                case LOG:
                    if (this->currentOption > this->maxOptions[3]) this->currentOption = 1;
                    break;
            }
            //this->lcd->update();  // Actualizar la pantalla después de cambiar la opción
        }
    } else {
        this->nextScreen();
    }
}

void Control::proccessDownKey() {
    if (this->ScreenSelected) {
        if (this->optionSelected) {
            switch (this->currentScreen) {
                case HOME:
                    Serial.println("Nothing to do in home");
                    break;
                case CONFIG:
                    if (this->currentOption == 1 || this->currentOption == 2) {
                        uint8_t index = this->currentOption;
                        this->delay[index -1 ] -= 1;
                        if (delay[index -1 ] < 0) {
                            delay[index - 1] = 0;
                        }
                        this->lcd->setProgressBarDelay(index, delay[ index -1 ]);
                        Serial.printf("Delay 1: %d\n", delay[ index - 1 ]);
                    }
                    //this->lcd->update();
                    break;
            }    
        } 
        else {
            this->currentOption--;
            switch (this->currentScreen) {
                case CONFIG:
                    Serial.println("Down key navigating config");
                    if (this->currentOption == 1) {
                        this->lcd->getProgressBar(0).setNavigated(true);
                        this->lcd->getProgressBar(0).setSelected(false);
                        this->lcd->getProgressBar(1).setNavigated(false);
                        this->lcd->getProgressBar(1).setSelected(false);
                    } else if (this->currentOption == 2) {
                        this->lcd->getProgressBar(0).setNavigated(false);
                        this->lcd->getProgressBar(0).setSelected(false);
                        this->lcd->getProgressBar(1).setNavigated(true);
                        this->lcd->getProgressBar(1).setSelected(false);
                    }
                    //this->lcd->update();
                    break;
            }

            switch (this->currentScreen) {
                case HOME:
                    if (this->currentOption > this->maxOptions[0] || this->currentOption < 1) this->currentOption = this->maxOptions[0];
                    break;
                case CONFIG:
                    if (this->currentOption > this->maxOptions[1] || this->currentOption < 1) this->currentOption = this->maxOptions[1];
                    break;
                case MANUAL:
                    if (this->currentOption > this->maxOptions[2] || this->currentOption < 1) this->currentOption = this->maxOptions[2];
                    break;
                case LOG:
                    if (this->currentOption > this->maxOptions[3] || this->currentOption < 1) this->currentOption = this->maxOptions[3];
                    break;
            }
        }
            //this->lcd->update();  // Actualizar la pantalla después de cambiar la opción
    }

    else {
        this->previousScreen();
    }
}

void Control::nextScreen() {
    switch (this->currentScreen) {
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
    //this->lcd->update();
    Serial.printf("CurrentScreen: %d\n", this->currentScreen);
}

void Control::previousScreen() {
    switch (this->currentScreen) {
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
    Serial.printf("CurrentScreen: %d\n", this->currentScreen);
}

Screen Control::getScreen() {
    return this->currentScreen;
}

void Control::setChiller(uint8_t index, Chiller *newChiller) {
    this->chillers[index-1] = newChiller;
}

void Control::setPump(uint8_t index, Pump *newPump) {
    this->pumps[index-1] = newPump;
}

bool readBit(uint8_t byte, uint8_t pin) {
    return (byte & (1 << pin)) != 0;
}

void Control::manual() {
    uint8_t index = this->currentOption - 1;  // 🟢 Crear un índice sin modificar `currentOption`

    if (index < 2) {  // 🟢 Para `pumps[0]` y `pumps[1]`
        this->GPIOA ^= (1 << this->pumps[index]->getPin());
        this->pumps[index]->toggle(this->GPIOA);

        // 🟢 Actualizar pantalla
        bool isOn = readBit(this->GPIOA, this->pumps[index]->getPin());
        this->lcd->setMotorState(index, isOn);
        Serial.printf("Pump %d toggled: %d\n", this->currentOption, isOn);
    } 
    else if (index < 4) {  // 🟢 Para `chillers[0]` y `chillers[1]`
        uint8_t chillerIndex = index - 2;  // 🟢 Ajustar índice para `chillers[]`
        this->GPIOA ^= (1 << this->chillers[chillerIndex]->getPin());
        this->chillers[chillerIndex]->toggle(this->GPIOA);

        // 🟢 Actualizar pantalla
        bool isOn = readBit(this->GPIOA, this->chillers[chillerIndex]->getPin());
        this->lcd->setChillerState(chillerIndex, isOn);
        Serial.printf("Chiller %d toggled: %d\n", this->currentOption - 2, isOn);
    }

    // 📝 Mostrar el estado actual de GPIOA
    Serial.print("\t\t ********* DATA: 0b");
    for (int j = 7; j >= 0; j--) {
        Serial.print(bitRead(this->GPIOA, j));  
    }
    Serial.println();
}

void Control::processChiller(){
    
    switch(this->chillerMode){
        case NONE_SELECTED:
            timerDelayCounter[0] = millis();
            timerDelayCounter[1] = millis();
            this->automaticSecuence(2);
            break;
        
        case CHILLER_1_SELECTED:
            this->automaticSecuence(0);
            break;

        case CHILLER_2_SELECTED:
            this->automaticSecuence(1);
            break;
    }
}

void Control::setProcessChiller(uint8_t index){
    if(index == 1){
        this->chillerMode = CHILLER_1_SELECTED;
    }
    else if(index == 2){
        this->chillerMode = CHILLER_2_SELECTED;
    }
    else{
        this->chillerMode = NONE_SELECTED;
    }
    
}


void Control::automaticSecuence(int index){
    //Serial.printf("processing chiller %d in automatic mode  \n",index + 1);   
    if(index == 2){
        this->GPIOA &= ~(1 << this->pumps[0]->getPin());
        this->GPIOA &= ~(1 << this->pumps[1]->getPin());
        this->GPIOA &= ~(1 << this->chillers[0]->getPin());
        this->GPIOA &= ~(1 << this->chillers[1]->getPin());

        this->pumps[0]->toggle(this->GPIOA);
        this->chillers[1]->toggle(this->GPIOA);
        this->pumps[0]->toggle(this->GPIOA);
        this->chillers[1]->toggle(this->GPIOA);

        this->lcd->setChillerState(0, false);
        this->lcd->setMotorState(0,false);
        this->lcd->setChillerState(1, false);
        this->lcd->setMotorState(1,false);

        this->lcd->setProgressBarValue(1,0);
        this->lcd->setProgressBarValue(2,0);

        this->lcd->setProgressBarPercentage(1,0);
        this->lcd->setProgressBarPercentage(2,0);
        
        this->flag_process[0] = false;
        this->flag_process[1] = false;
        return;
    }
    
}



void Control::turnOnAutomaticSecuence(int index){
    if(this->flag_process[index] == false){ // incia el contador 
        this->flag_process[index] = true;
        this->timerDelayCounter[index] = millis();
    }
    else{ // ya incio el contador
        this->delayCounter[index] = millis() - timerDelayCounter[index];
        this->timerDelayCounter[index] = millis();
        this->updateProgressBar(index);
        this->turnOnPump(index);         // se enciende bomba ;
        if(this->delayCounter[index] > this->delay[index] * 1000 ){  // se espera el tiempo para encender el chiller
            this->turnOnChiller(index);  // se enciende el chiller;
            this->flag_process[index] = false;
        }
    }    
    
        if(this->delayCounter[index] > this->delay[index] * 1000 ){ // cuando pase el tiempo
            turnOnChiller(index);
            uint8_t percentage = (100 * this->delayCounter[index]) / (1000 * this->delay[index]);
            this->lcd->getProgressBar(index).setPercentage(percentage);
            this->lcd->getProgressBar(index).setCounter(this->delayCounter[index]/1000);
            this->flag_process[index] = true;
        }

}

void Control::updateProgressBar(int index){
    static unsigned long lastUpdateTime[8] = {0};  // Suponiendo hasta 8 chillers
            
    if (millis() - lastUpdateTime[index] >= 1000) {  // Se actualiza cada 1 segundo
        lastUpdateTime[index] = millis();

        uint8_t percentage = (100 * this->delayCounter[index]) / (1000 * this->delay[index]);
        this->lcd->getProgressBar(index).setPercentage(percentage);
        this->lcd->getProgressBar(index).setCounter(this->delayCounter[index]/1000);
        
        Serial.printf("\rProgress Bar : |");  // Retorno de carro para sobrescribir
        for (int i = 0; i < 20; i++) {
            if (i < percentage / 5) {
                Serial.printf("█");  // Bloques llenos
            } else {
                Serial.printf(" ");  // Espacios vacíos
            }
        }
        Serial.printf("| %d% %", percentage);  // Espacios extras para borrar residuos
        Serial.flush();  // Asegurar que se imprima en la terminal en tiempo real
    }
}

void Control::turnOffAutomaticSecuence(int index){

}

void Control::turnOnChiller(int index){
    this->GPIOA |= (1 << this->chillers[index]->getPin());
    this->chillers[index]->toggle(this->GPIOA);
    this->lcd->setChillerState(index, true);
}

void Control::turnOffChiller(int index){
    this->GPIOA &= ~(1 << this->chillers[index]->getPin());
    this->chillers[index]->toggle(this->GPIOA);
    this->lcd->setMotorState(index, false);
}

void Control::turnOnPump(int index){
    this->GPIOA |= (1 << this->pumps[index]->getPin());
    this->pumps[index]->toggle(this->GPIOA);
    this->lcd->setMotorState(index, true);
}

void Control::turnOffPump(int index){
    this->GPIOA &= ~(1 << this->pumps[index]->getPin());
    this->pumps[index]->toggle(this->GPIOA);
    this->lcd->setMotorState(index, false);
}