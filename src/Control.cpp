#include "Control.hpp"

Control::Control(const char *name, int taskCore) : Module(name, taskCore) {
    this->currentMode = AUTOMATIC_MODE;  // Por defecto, el modo es automático
    this->delay1 = 0;                   // Inicializar delay1
    this->delay2 = 0;                   // Inicializar delay2
    this->iterationDelay = 1;
    Serial.println("Control initialized");
}

void Control::connect(void *data) {
    this->ScreenSelected = false;
    this->optionSelected = false;
    this->currentOption = 0;
    this->maxOptions = new uint8_t[4]; // delay 1, delay 2, 
    this->lcd = (GraphicLCD *)data;    // Asignar el puntero a la pantalla LCD
    this->chillers = new Chiller*[2];
    this->pumps = new Pump*[2];
    this->maxOptions[0] = 0;
    this->maxOptions[1] = 2;
    this->maxOptions[2] = 4;
    this->maxOptions[3] = 0;

    Serial.println("Control connected to LCD");
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
            if (flag) {
                this->pumps[0]->turnOn();
                this->chillers[0]->turnOn();
            } else {
                this->pumps[1]->turnOn();
                this->chillers[1]->turnOn();
            }
            flag = !flag;  // Alternar el estado de flag
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

                    // Activar/desactivar dispositivos según la opción seleccionada
                    if (this->currentOption == 1) {
                        this->pumps[0]->toggle();
                        Serial.println("Pump 1 toggled");
                    } else if (this->currentOption == 2) {
                        this->pumps[1]->toggle();
                        Serial.println("Pump 2 toggled");
                    } else if (this->currentOption == 3) {
                        this->chillers[0]->toggle();
                        Serial.println("Chiller 1 toggled");
                    } else if (this->currentOption == 4) {
                        this->chillers[1]->toggle();
                        Serial.println("Chiller 2 toggled");
                    }
                    break;
                case LOG:
                    Serial.println("Nothing to do in LOG");
                    break;
            }
            this->lcd->update();  // Actualizar la pantalla después de confirmar la selección
        } else {
            // Si no hay una opción seleccionada, seleccionamos la opción actual
            this->optionSelected = true;
            Serial.printf("Option %d selected\n", this->currentOption);

            // Actualizar el estado a SELECTED
            this->lcd->getTextInput(this->currentOption - 1)->setState(InputState::SELECTED);

            this->lcd->update();  // Actualizar la pantalla después de seleccionar la opción
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
        this->lcd->update();  // Actualizar la pantalla después de seleccionar la pantalla
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
        this->lcd->update();  // Actualizar la pantalla después de deseleccionar
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
                        this->delay1 += 1;
                        this->lcd->setProgressBarDelay(0, delay1);
                        Serial.printf("Delay 1: %d\n", delay1);
                    } else if (this->currentOption == 2) {
                        this->delay2 += 1;
                        this->lcd->setProgressBarDelay(1, delay2);
                        Serial.printf("Delay 2: %d\n", delay2);
                    }
                    this->lcd->update();
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
                    this->lcd->update();
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
            this->lcd->update();  // Actualizar la pantalla después de cambiar la opción
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
                    if (this->currentOption == 1) {
                        this->delay1 -= 1;
                        if (delay1 < 0) delay1 = 0;
                        this->lcd->setProgressBarDelay(0, delay1);
                        Serial.printf("Delay 1: %d\n", delay1);
                    } else if (this->currentOption == 2) {
                        this->delay2 -= 1;
                        if (delay2 < 0) delay2 = 0;
                        this->lcd->setProgressBarDelay(1, delay2);
                        Serial.printf("Delay 2: %d\n", delay2);
                    }
                    this->lcd->update();
                    break;
            }
        } else {
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
                    this->lcd->update();
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
            this->lcd->update();  // Actualizar la pantalla después de cambiar la opción
        }
    } else {
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
    this->lcd->update();
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
    this->lcd->update();
    Serial.printf("CurrentScreen: %d\n", this->currentScreen);
}

Screen Control::getScreen() {
    return this->currentScreen;
}

void Control::setChiller(uint8_t index, Chiller *newChiller) {
    this->chillers[index] = newChiller;
}

void Control::setPump(uint8_t index, Pump *newPump) {
    this->pumps[index] = newPump;
}