#include "Control.hpp"

Control::Control(const char *name, int taskCore) : Module(name, taskCore) {
    this->currentMode = AUTOMATIC_MODE;  // Por defecto, el modo es manual
    this->delay1 = 0;                 // Inicializar delay1
    this->delay2 = 0;                 // Inicializar delay2
}


void Control::connect(void *data) {
    this->ScreenSelected = false;
    this->optionSelected = false;
    this->option = 0;
    this->lcd = (GraphicLCD *)data;  // Asignar el puntero a la pantalla LCD
    this->chillers = new Chiller*[2];
    this->pumps = new Pump*[2];

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
            vTaskDelay(this->iterationDelay); // NO HACE NADA AUTOMATICO
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
    Serial.printf("| ScreenSelected : %d - optionSelected : %d - option : %d \n", this->ScreenSelected, this->optionSelected, this->option);
}

void Control::proccessEnterKey() {
    if (currentScreen == CONFIG || currentScreen == MANUAL) {
        if (!this->ScreenSelected) {
            this->ScreenSelected = true;
            this->optionSelected = false;
            this->option = 0;
        } else if (this->ScreenSelected && !this->optionSelected) {
            this->optionSelected = true;
        } else if (this->optionSelected) {
            if (currentScreen == CONFIG) {
                if (this->option == 1) {
                    // Cambiar entre modo manual y automático
                    this->currentMode = (this->currentMode == MANUAL_MODE) ? AUTOMATIC_MODE : MANUAL_MODE;
                    Serial.println(this->currentMode == MANUAL_MODE ? "Modo Manual" : "Modo Automático");
                } else if (this->option == 2) {
                    // Editar DELAY 1
                    Serial.println("Editando DELAY 1...");
                } else if (this->option == 3) {
                    // Editar DELAY 2
                    Serial.println("Editando DELAY 2...");
                }
            } else if (currentScreen == MANUAL && this->currentMode == MANUAL_MODE) {
                // Cambiar estado de MOTOR o CHILLER (solo en modo manual)
                switch (this->option) {
                    case 1:
                        Serial.println("Cambiando estado de MOTOR 1...");
                        break;
                    case 2:
                        Serial.println("Cambiando estado de CHILLER 1...");
                        break;
                    case 3:
                        Serial.println("Cambiando estado de MOTOR 2...");
                        break;
                    case 4:
                        Serial.println("Cambiando estado de CHILLER 2...");
                        break;
                }
            }
        }
    }
}

void Control::proccessBackKey() {
    if (currentScreen == CONFIG || currentScreen == MANUAL) {
        if (this->ScreenSelected && this->optionSelected) {
            this->optionSelected = false;
        } else if (this->ScreenSelected && !this->optionSelected) {
            this->ScreenSelected = false;
        }
    }
}

void Control::proccessUpKey() {
    if (this->ScreenSelected) {
        if (currentScreen == CONFIG) {
            uint8_t maxScreenOptions = 3;  // Ahora hay 3 opciones: Modo, DELAY 1, DELAY 2
            if (!this->optionSelected) {
                this->option = (this->option % maxScreenOptions) + 1;
            } else {
                // Aumentar valor de DELAY
                if (this->option == 2) {
                    this->delay1 += 1;  // Aumentar DELAY 1
                    Serial.printf("DELAY 1: %lu\n", this->delay1);
                } else if (this->option == 3) {
                    this->delay2 += 1;  // Aumentar DELAY 2
                    Serial.printf("DELAY 2: %lu\n", this->delay2);
                }
            }
        } else if (currentScreen == MANUAL && this->currentMode == MANUAL_MODE) {
            uint8_t maxScreenOptions = 4;
            if (!this->optionSelected) {
                this->option = (this->option % maxScreenOptions) + 1;
            } else {
                // Cambiar estado de MOTOR o CHILLER
                switch (this->option) {
                    case 1:
                        Serial.println("Encendiendo MOTOR 1...");
                        break;
                    case 2:
                        Serial.println("Encendiendo CHILLER 1...");
                        break;
                    case 3:
                        Serial.println("Encendiendo MOTOR 2...");
                        break;
                    case 4:
                        Serial.println("Encendiendo CHILLER 2...");
                        break;
                }
            }
        }
    } else {
        this->nextScreen();
        //this->lcd->nextScreen();
        this->option = 0;
    }
}

void Control::proccessDownKey() {
    if (this->ScreenSelected) {
        if (currentScreen == CONFIG) {
            uint8_t maxScreenOptions = 3;  // Ahora hay 3 opciones: Modo, DELAY 1, DELAY 2
            if (!this->optionSelected) {
                this->option = (this->option - 2 + maxScreenOptions) % maxScreenOptions + 1;
            } else {
                // Disminuir valor de DELAY
                if (this->option == 2) {
                    this->delay1 = (this->delay1 > 0) ? this->delay1 - 1 : 0;  // Disminuir DELAY 1
                    Serial.printf("DELAY 1: %lu\n", this->delay1);
                } else if (this->option == 3) {
                    this->delay2 = (this->delay2 > 0) ? this->delay2 - 1 : 0;  // Disminuir DELAY 2
                    Serial.printf("DELAY 2: %lu\n", this->delay2);
                }
            }
        } else if (currentScreen == MANUAL && this->currentMode == MANUAL_MODE) {
            uint8_t maxScreenOptions = 4;
            if (!this->optionSelected) {
                this->option = (this->option - 2 + maxScreenOptions) % maxScreenOptions + 1;
            } else {
                // Cambiar estado de MOTOR o CHILLER
                switch (this->option) {
                    case 1:
                        Serial.println("Apagando MOTOR 1...");
                        break;
                    case 2:
                        Serial.println("Apagando CHILLER 1...");
                        break;
                    case 3:
                        Serial.println("Apagando MOTOR 2...");
                        break;
                    case 4:
                        Serial.println("Apagando CHILLER 2...");
                        break;
                }
            }
        }
    } else {
        this->previousScreen();
        this->option = 0;
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