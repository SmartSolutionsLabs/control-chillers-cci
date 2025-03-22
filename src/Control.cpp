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

    while(this->lcd == nullptr || !this->lcd->isInitialized()) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
    } 

    this->turnOffPump(1);
    this->turnOffPump(2);
    this->turnOffChiller(1);
    this->turnOffChiller(2);

    this->lcd->setProgressBarValue(1,0);
    this->lcd->setProgressBarValue(2,0);

    this->lcd->setProgressBarDelay(1,this->delay[0]);
    this->lcd->setProgressBarDelay(2,this->delay[1]);

    this->lcd->getProgressBar(0).setCounter(this->delayCounter[0]/1000);
    this->lcd->getProgressBar(1).setCounter(this->delayCounter[1]/1000);

    bool flag = false;
    this->flag_process[0] = true;
    this->flag_process[1] = true;
    this->lcd->getProgressBar(0).setCounter(0);
    this->lcd->getProgressBar(1).setCounter(0);
    this->lcd->setScreen(MANUAL);
    while (1) {
        vTaskDelay(this->iterationDelay);

        // Lógica para el modo automático
        if (this->currentMode == AUTOMATIC_MODE) {
            processChiller();
        }
    }
}

void Control::handleKey(char key) {
    //Serial.printf("Key pressed: %c\n", key);

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
    Serial.printf("currentScreen : %d, ScreenSelected: %d, currentOption: %d, optionSelected: %d\n", 
                  this->currentScreen, this->ScreenSelected, this->currentOption, this->optionSelected);
}

void Control::proccessEnterKey() {
    if(!this->ScreenSelected){
        this->ScreenSelected = true;
    }
    else if(this->ScreenSelected && !this->optionSelected){
        this->optionSelected = true;
        /////////////////
    }
    else if(this->ScreenSelected && this->optionSelected){
        this->manualControlDevice();
    }
}

void Control::proccessBackKey() {
    if (this->ScreenSelected && this->optionSelected) {
        this->optionSelected = false;
    }
    else if(this->ScreenSelected && !this->optionSelected){
        this->ScreenSelected = false;
    }   
}


void Control::proccessUpKey() {
    if(!ScreenSelected){   // navegando entre pantallas
        this->nextScreen();
        this->currentOption = 1;
    }
    else if(ScreenSelected && !optionSelected ){ // navegando entre opciones
        this->nextOption();
    }
    else if(ScreenSelected && optionSelected ){ // cambiando valor de una opcion
        this->upValueOption();
    }
}

void Control::proccessDownKey() {
    if(!ScreenSelected){
        this->previousScreen();
    }
    else if(ScreenSelected && !optionSelected ){ // navegando entre opciones
        this->previousOption();
    }
    else if(ScreenSelected && optionSelected ){ // navegando entre opciones
        this->downValueOption();
    }
}

void Control::upValueOption(){

}

void Control::downValueOption(){
    
}

void Control::processOption(){
    switch(this->currentScreen){
        case 0: //home
            this->currentOption = 0;
            break;
        case 1: // config
            if(this->currentOption == 1 ){
                this->lcd->getProgressBar(0).setNavigated(true);
                this->lcd->getProgressBar(1).setNavigated(false);
            }
            else if(this->currentOption == 2 ){
                this->lcd->getProgressBar(0).setNavigated(false);
                this->lcd->getProgressBar(1).setNavigated(true);
            }
            break;
        case 2: // manual
            if(this->currentOption == 1){
                this->lcd->navigateMotor(0,true);
                this->lcd->navigateMotor(1,false);
                this->lcd->navigateChiller(0,false);
                this->lcd->navigateChiller(1,false);
            }
            else if(this->currentOption == 2){
                this->lcd->navigateMotor(0,false);
                this->lcd->navigateMotor(1,false);
                this->lcd->navigateChiller(0,true);
                this->lcd->navigateChiller(1,false);
            }
            else if(this->currentOption == 3){
                this->lcd->navigateMotor(0,false);
                this->lcd->navigateMotor(1,true);
                this->lcd->navigateChiller(0,false);
                this->lcd->navigateChiller(1,false);
            }
            else if(this->currentOption == 4){
                this->lcd->navigateMotor(0,false);
                this->lcd->navigateMotor(1,false);
                this->lcd->navigateChiller(0,false);
                this->lcd->navigateChiller(1,true);
            }
            break;
        case 3: // log
            this->currentOption = 0;
            break;
    }
   
}

void Control::nextOption(){
    this->currentOption ++ ;
    if(this->currentOption > maxOptions[this->currentScreen]){
        this->currentOption = 1;
    }
    this->processOption();
}

void Control::previousOption(){
    this->currentOption -- ;
    if(this->currentOption <= 0 || this->currentOption > maxOptions[this->currentScreen]){
        this->currentOption = maxOptions[this->currentScreen];
    }
    this->processOption();
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
    this->chillers[index] = newChiller;
}

void Control::setPump(uint8_t index, Pump *newPump) {
    this->pumps[index] = newPump;
}

bool readBit(uint8_t byte, uint8_t pin) {
    return (byte & (1 << pin)) != 0;
}

void Control::manualControlDevice() {
    uint8_t index = this->currentOption;  // 🟢 Crear un índice sin modificar `currentOption`
    if(index == 1){
        if(this->chillers[index-1]->getState()){ // si el chiller esta encendido ; apagar en secuencia
            this->turnOffAutomaticSecuence(1);
        }
        else{
            if(this->pumps[index-1]->getState()){ // si no ;  y si la bomba esta encendida ; se puede apagar directamente la bomba
                this->turnOffPump(1);
            }
            else{
                this->turnOnPump(1);  // si la bomba esta apagada ; se enciende.
            }
        }
    }
    if(index == 2){    
        if(this->chillers[0]->getState()){// si el chiller esta encendido
            this->turnOffChiller(1);
        }
        else{ // si el chiller esta apagado
            this->turnOnChiller(1);
        }
    }
    
    if(index == 3){
        if(this->chillers[1]->getState()){ // si el chiller esta encendido ; apagar en secuencia
            this->turnOffAutomaticSecuence(2);
        }
        else{
            if(this->pumps[1]->getState()){ // si no ;  y si la bomba esta encendida ; se puede apagar directamente la bomba
                this->turnOffPump(2);
            }
            else{
                this->turnOnPump(2);  // si la bomba esta apagada ; se enciende.
            }
        }
    }
    if(index == 4){
        if(this->chillers[1]->getState()){// si el chiller esta encendido
            this->turnOffChiller(2);
        }
        else{ // si el chiller esta apagado
            this->turnOnChiller(2);
        }
    }    
}

void Control::processChiller(){
    if(this->automaticSecuenceOn[0]){
        this->turnOnAutomaticSecuence(1);
    }
    else if(this->automaticSecuenceOn[1]){
        this->turnOnAutomaticSecuence(2);
    }
    else if(this->automaticSecuenceOff[0]){
        this->turnOffAutomaticSecuence(1);
    }
    else if(this->automaticSecuenceOff[1]){
        this->turnOffAutomaticSecuence(2);
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
    
    switch(this->chillerMode){
        case NONE_SELECTED:
            if(this->lastChillerMode == CHILLER_1_SELECTED){
                this->automaticSecuenceOn[0] = false;
                this->automaticSecuenceOff[0] = true;
                this->flag_process[0] = false;
            }
            if(this->lastChillerMode == CHILLER_2_SELECTED){
                this->automaticSecuenceOn[1] = false;
                this->automaticSecuenceOff[1] = true;
                this->flag_process[1] = false;
            }
            break;
        
        case CHILLER_1_SELECTED:
            if(this->lastChillerMode == NONE_SELECTED){
                this->automaticSecuenceOn[0] = true;
                this->flag_process[0] = false;
            }
            break;

        case CHILLER_2_SELECTED:
            if(this->lastChillerMode == NONE_SELECTED){
                this->automaticSecuenceOn[1] = true;
                this->flag_process[1] = false;
            }
            break;
    }
    Serial.printf("lastChillerMode: %d | ChillerMode: %d",this->lastChillerMode,this->chillerMode);
    this->lastChillerMode = this->chillerMode;
}

void Control::turnOnAutomaticSecuence(int index){
    if(this->flag_process[index-1] == true){  // incia el contador 
        return;
    }
    
    if(!this->pumps[index-1]->getState()){ //  verifica el estado del motor
        this->turnOnPump(index);           /// enciende el motor por primera vez
        this->timerDelayCounter[index-1] = millis(); //incia el timer contador del delay entre motor y chiller
    }

    this->delayCounter[index-1] = millis() - timerDelayCounter[index-1];
    this->updateProgressBar(index);

    if(this->delayCounter[index-1] > this->delay[index-1] * 1000 ){  // se espera el tiempo para encender el chiller
        this->turnOnChiller(index);  // se enciende el chiller;
        this->flag_process[index-1] = true;
        Serial.printf("Process %d already Done ;  this->flag_process%d = %d \n",index-1,index-1,this->flag_process[index-1]);
    }
}

void Control::turnOffAutomaticSecuence(int index){
    if(this->flag_process[index-1] == true){  // incia el contador 
        return;
    }
    
    if(this->chillers[index-1]->getState()){ //  verifica el estado del motor
        this->turnOffChiller(index);           /// enciende el motor por primera vez
        this->timerDelayCounter[index-1] = millis(); //incia el timer contador del delay entre motor y chiller
    }

    this->delayCounter[index-1] = millis() - timerDelayCounter[index-1];
    this->updateProgressBar(index);

    if(this->delayCounter[index-1] > this->delay[index-1] * 1000 ){  // se espera el tiempo para encender el chiller
        this->turnOffPump(index);  // se enciende el chiller;
        this->flag_process[index-1] = true;
    }
}

void Control::updateProgressBar(int index){
    static unsigned long lastUpdateTime[8] = {0};  // Suponiendo hasta 8 chillers
            
    if (millis() - lastUpdateTime[index-1] >= 1000) {  // Se actualiza cada 1 segundo
        lastUpdateTime[index-1] = millis();

        uint8_t percentage = (100 * this->delayCounter[index-1]) / (1000 * this->delay[index-1]);
        if(percentage > 100) {
            percentage = 100;
        }

        this->lcd->getProgressBar(index-1).setPercentage(percentage);
        uint32_t secondsElapsed = this->delayCounter[index-1] / 1000;
        if (secondsElapsed > this->delay[index-1]) {
            secondsElapsed = this->delay[index-1]; // Evita overflow
        }
        this->lcd->getProgressBar(index-1).setCounter(secondsElapsed);
        
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

void Control::turnOnChiller(int index){
    this->GPIOA |= (1 << this->chillers[index-1]->getPin());
    if (this->chillers[index-1] != nullptr) {
        this->chillers[index-1]->toggle(this->GPIOA);
    } else {
        Serial.printf("Error: chiller %d es nullptr\n", index-1);
    }

    if (index < 1 || index > 2) {
        Serial.printf("Error: Índice de chiller fuera de rango: %d\n", index);
        return;
    }
    this->lcd->setChillerState(index-1, true);
}

void Control::turnOffChiller(int index){
    this->GPIOA &= ~(1 << this->chillers[index-1]->getPin());

    if (this->chillers[index-1] != nullptr) {
        this->chillers[index-1]->toggle(this->GPIOA);
    } 
    else {
        Serial.printf("Error: chiller %d es nullptr\n", index-1);
    }

    if (index < 1 || index > 2) {
        Serial.printf("Error: Índice de chiller fuera de rango: %d\n", index);
        return;
    }
    
    this->lcd->setChillerState(index-1, false);
}

void Control::turnOnPump(int index){
    this->GPIOA |= (1 << this->pumps[index-1]->getPin());
    this->pumps[index-1]->toggle(this->GPIOA);
    this->lcd->setMotorState(index-1, true);
}

void Control::turnOffPump(int index){
    this->GPIOA &= ~(1 << this->pumps[index-1]->getPin());
    this->pumps[index-1]->toggle(this->GPIOA);
    this->lcd->setMotorState(index-1, false);
}