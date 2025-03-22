#include "GraphicLcd.hpp"

GraphicLCD::GraphicLCD(const char * name, int taskCore) : Module(name, taskCore) {
}

void GraphicLCD::connect(void *data) {
    this->u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, 47, 21, 14, 38); // smart board R8-AI8-DI8
    this->u8g2->setFont(u8g2_font_6x10_tf);  // Establecer la fuente una vez
    pinMode(48, OUTPUT);
    digitalWrite(48, LOW);
    this->motorIcon = new MotorGraphicLCD[2];
    this->progressBar = new progressBarLCD[2];
    this->chillerIcon = new ChillerGraphicLCD[2];
    this->screenTimer = millis();  
    this->timerFPS = millis();

    for (int i = 0; i < 2; i++) {
        this->motorIcon[i] = MotorGraphicLCD(u8g2);
        this->progressBar[i] = progressBarLCD(u8g2);
        this->chillerIcon [i] = ChillerGraphicLCD(u8g2);

        this->motorIcon[i].setID(i + 1);
        this->motorIcon[i].setRun(false);
        this->motorIcon[i].setState(false);
        this->motorIcon[i].setUpdateTimer(200);
        this->motorIcon[i].setTimer(this->screenTimer);

        this->chillerIcon[i].setID(i + 1);
        this->chillerIcon[i].setRun(false);
        this->chillerIcon[i].setState(false);
        this->chillerIcon[i].setUpdateTimer(200);
        this->chillerIcon[i].setTimer(this->screenTimer);

        this->progressBar[i].setID(i + 1);
        this->progressBar[i].setRun(false);
        this->progressBar[i].setState(false);
        this->progressBar[i].setUpdateTimer(200);
        this->progressBar[i].setTimer(this->screenTimer);
        this->progressBar[i].setValue(60); // valor de delay inicial
        this->progressBar[i].setPercentage(0);
    }

    if (!this->u8g2->begin()) {
        Serial.println("Display initialization failed!");
    } else {
        Serial.println("Display ok!");
        this->setInitialized(true); 
    }
    this->u8g2->setFont(u8g2_font_6x10_tf);
}

void GraphicLCD::run(void* data) {
	this->iterationDelay = 1 / portTICK_PERIOD_MS;
    this->screenTimer = millis();
    this->splashScreen();
    vTaskDelay(splashScreenTimer/portTICK_PERIOD_MS);
    this->u8g2->clearBuffer();
    this->update();
	while (1) {
        this->update();
		vTaskDelay(this->iterationDelay);
	}
}

void GraphicLCD::print(int x, int y, char *text){
    this->u8g2->setCursor(x,y); 
    this->u8g2->print(text);
}

void GraphicLCD::splashScreen(){
    this->u8g2->clearBuffer();
    this->drawImage(17,5, IMAGE_SSL_LOGO_DATA);
    this->u8g2->sendBuffer();
}

void GraphicLCD::drawMenu(){
    this->u8g2->drawLine(16, 0, 16, 64);
    this->u8g2->drawLine(0, 16, 16, 16);
    this->u8g2->drawLine(0, 32, 16, 32);
    this->u8g2->drawLine(0, 48, 16, 48);
    switch (this->currentScreen){
        case HOME:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_SELECT_DATA);
            this->drawImage(1,17,ICON_CONFIG_DATA);
            this->drawImage(1,33,ICON_MANUAL_DATA);
            this->drawImage(1,49,ICON_LOG_DATA);    
            break;
        case CONFIG:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_DATA);
            this->drawImage(1,17,ICON_CONFIG_SELECT_DATA);
            this->drawImage(1,33,ICON_MANUAL_DATA);
            this->drawImage(1,49,ICON_LOG_DATA); 
            break;
        case MANUAL:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_DATA);
            this->drawImage(1,17,ICON_CONFIG_DATA);
            this->drawImage(1,33,ICON_MANUAL_SELECT_DATA);
            this->drawImage(1,49,ICON_LOG_DATA); 
            break;
        case LOG:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_DATA);
            this->drawImage(1,17,ICON_CONFIG_DATA);
            this->drawImage(1,33,ICON_MANUAL_DATA);
            this->drawImage(1,49,ICON_LOG_SELECT_DATA); 
            break;
    }
    //this->drawImage(0,0, IMAGE_HOME_SCREEN_DATA);
}

void GraphicLCD::drawHomePage() {
    this->drawMenu();
    this->drawBoxes();

    // Dibujar motor 1
    motorIcon[0].setPosition(33, 5);
    motorIcon[0].showIcon();
    motorIcon[0].hideLabelState();
    motorIcon[0].setAnimation(true);
    motorIcon[0].animate();

    // Dibujar progressBar 1
    progressBar[0].setPosition(62, 5);
    progressBar[0].showIcon();
    progressBar[0].showLabelState();
    //progressBar[0].setAnimation(true);
    progressBar[0].animate();
    progressBar[0].hideTextInput();

    // Dibujar chiller 1
    chillerIcon[0].setPosition(90, 5);
    chillerIcon[0].showIcon();
    chillerIcon[0].hideLabelState();
    chillerIcon[0].setAnimation(true);
    chillerIcon[0].animate();
    //chillerIcon[0].animate();

    // Dibujar motor 2
    motorIcon[1].setPosition(33, 37);
    motorIcon[1].showIcon();
    motorIcon[1].hideLabelState();
    motorIcon[1].setAnimation(true);
    motorIcon[1].animate();

    // Dibujar progressBar 2
    progressBar[1].setPosition(62, 37);
    progressBar[1].showIcon();
    progressBar[1].showLabelState();
    //progressBar[1].setAnimation(true);
    progressBar[1].animate();
    progressBar[1].hideTextInput();

    // Dibujar chiller 2
    chillerIcon[1].setPosition(90, 37);
    chillerIcon[1].showIcon();
    chillerIcon[1].hideLabelState();
    chillerIcon[1].setAnimation(true);
    chillerIcon[1].animate();
    //chillerIcon[1].animate();

    this->u8g2->sendBuffer();
}

void GraphicLCD::drawConfigPage() {
    this->drawMenu();
    this->drawBoxes();
    
    this->u8g2->setFont(u8g2_font_6x10_tf);
    this->u8g2->drawStr(42, 12, "CHILLER 1");
    this->u8g2->drawStr(42, 44, "CHILLER 2");

    // Dibujar Progress bar 1
    progressBar[0].setPosition(70, 15);
    progressBar[0].hideIcon();
    progressBar[0].hideLabelState();
    progressBar[0].showTextInput();
    progressBar[0].showLabelInput();

    // Dibujar Progress bar 2
    progressBar[1].setPosition(70, 47);
    progressBar[1].hideIcon();
    progressBar[1].hideLabelState();
    progressBar[1].showTextInput();
    progressBar[1].showLabelInput();

    this->u8g2->sendBuffer();
}

void GraphicLCD::drawManualPage(){
    this->drawMenu();
    this->drawBoxes();

    motorIcon[0].setPosition(33,5);
    motorIcon[0].showIcon();
    motorIcon[0].showLabelState();
    motorIcon[0].animate();
    motorIcon[0].update();

    motorIcon[1].setPosition(33,37);
    motorIcon[1].showIcon(); 
    motorIcon[1].showLabelState();
    motorIcon[1].animate();
    motorIcon[1].update();

    // Dibujar CHILLER 1
    chillerIcon[0].setPosition(80, 5);
    chillerIcon[0].showIcon();
    chillerIcon[0].showLabelState();
    chillerIcon[0].setAnimation(true);
    chillerIcon[0].animate();
    chillerIcon[0].update();
    

    // Dibujar CHILLER 1
    chillerIcon[1].setPosition(80, 37);
    chillerIcon[1].showIcon();
    chillerIcon[1].showLabelState();
    chillerIcon[1].setAnimation(true);
    chillerIcon[1].animate();
    chillerIcon[1].update();
    
    this->u8g2->sendBuffer();
}

void GraphicLCD::drawLogPage(){
    this->drawMenu();
    this->u8g2->sendBuffer();
}

void GraphicLCD::setMotorState(int motorNumber , bool state){
    this->motorIcon[motorNumber].setState(state);
    this->motorIcon[motorNumber].setRun(state);
}

void GraphicLCD::setChillerState(int chillerNumber , bool state){
    this->chillerIcon[chillerNumber].setState(state);
    this->chillerIcon[chillerNumber].setRun(state);
}

void GraphicLCD::update(){
    if(millis() - this->timerFPS < 100){
        return;
    }
    //Serial.println("Actualizando LCD...");

    this->timerFPS = millis();

    static bool lastMotorState[2] = {false, false};  // Guardar el último estado de los motores
    static bool lastChillerState[2] = {false, false};  // Guardar el último estado de los chillers
    static uint16_t lastDelay1 = progressBar[0].getValue();  // Guardar el último valor de delay1
    static uint16_t lastDelay2 = progressBar[1].getValue();  // Guardar el último valor de delay2

    bool motorStateChanged = (this->motorIcon[0].getState() != lastMotorState[0]) || 
                             (this->motorIcon[1].getState() != lastMotorState[1]);
    bool chillerStateChanged = (this->chillerIcon[0].getState() != lastChillerState[0]) || 
                               (this->chillerIcon[1].getState() != lastChillerState[1]);
    bool delayChanged = (progressBar[0].getValue() != lastDelay1) || 
                        (progressBar[1].getValue() != lastDelay2);

    //if (this->newScreen || motorStateChanged || chillerStateChanged || delayChanged ) {
    if (true){
        this->u8g2->clearBuffer();

        switch (this->currentScreen) {
            case HOME:
                this->drawHomePage();
                break;
            case CONFIG:
                this->drawConfigPage();
                break;
            case MANUAL:
                this->drawManualPage();
                break;
            case LOG:
                this->drawLogPage();
                break;
        }

        this->u8g2->sendBuffer();

        // Actualizar los últimos estados
        lastMotorState[0]   =   this->motorIcon[0].getState();
        lastMotorState[1]   =   this->motorIcon[1].getState();
        lastChillerState[0] = this->chillerIcon[0].getState();
        lastChillerState[1] = this->chillerIcon[1].getState();
        lastDelay1 = progressBar[0].getValue();
        lastDelay2 = progressBar[1].getValue();
    }
}

void GraphicLCD::drawRotatedImage(int xPos, int yPos, const Bitmap &image, float angle) {
    float radians = angle * M_PI / 180.0;
    float cosA = cos(radians);
    float sinA = sin(radians);

    int oldW = image.width;
    int oldH = image.height;
    int centerX = oldW / 2;
    int centerY = oldH / 2;

    int newSize = ceil(sqrt(oldW * oldW + oldH * oldH)); // Tamaño del área que puede ocupar la imagen rotada
    int newCenterX = newSize / 2;
    int newCenterY = newSize / 2;

    uint8_t bytesPerRow = (oldW + 7) / 8;

    for (int y = 0; y < oldH; y++) {
        for (int x = 0; x < oldW; x++) {
            int byteIndex = y * bytesPerRow + (x / 8);
            uint8_t bitMask = 1 << (7 - (x % 8));

            if (image.data[byteIndex] & bitMask) { 
                // Coordenadas relativas al centro
                int xRel = x - centerX;
                int yRel = y - centerY;

                // Aplicar transformación de rotación
                int xRot = round(xRel * cosA - yRel * sinA) + newCenterX;
                int yRot = round(xRel * sinA + yRel * cosA) + newCenterY;

                // Dibujar el píxel en la nueva posición
                this->u8g2->drawPixel(xPos + xRot, yPos + yRot);
            }
        }
    }
}

void GraphicLCD::drawImage(int xPos, int yPos, const Bitmap &image) {
    uint8_t bytesPerRow = (image.width + 7) / 8; // Bytes por fila

    for (uint8_t y = 0; y < image.height; y++) {
        for (uint8_t x = 0; x < image.width; x++) {
            uint16_t byteIndex = y * bytesPerRow + (x / 8);
            uint8_t bitMask = 1 << (7 - (x % 8));

            if (image.data[byteIndex] & bitMask) {
                this->u8g2->drawPixel(xPos + x, yPos + y);
            }
        }
    }
}

void GraphicLCD::setScreen(Screen newScreen){
    this->currentScreen = newScreen;
    this->setNewScreen();
}

void GraphicLCD::setProgressBarValue(uint8_t index ,uint16_t newValue){
    this->progressBar[index-1].setValue(newValue);
}

void GraphicLCD::setProgressBarPercentage(uint8_t index ,uint8_t newPercentage){
    this->progressBar[index-1].setPercentage(newPercentage);
}

void GraphicLCD::drawBoxes() {
    this->u8g2->setFont(u8g2_font_ncenB12_tr);
    uint8_t dy = 32;

    for (int i = 0; i < 2; i++) {
        int yOffset = i * dy;
        this->u8g2->drawLine(18, 2 + yOffset, 126, 2 + yOffset);
        this->u8g2->drawLine(18, 30 + yOffset, 126, 30 + yOffset);
        this->u8g2->drawLine(18, 2 + yOffset, 18, 30 + yOffset);
        this->u8g2->drawLine(126, 2 + yOffset, 126, 30 + yOffset);
        this->u8g2->drawStr(19, 22 + yOffset, String(i + 1).c_str());
        this->u8g2->drawLine(29, 2 + yOffset, 29, 29 + yOffset);
    }
}

void GraphicLCD::drawCenteredText(int xCenter, int yCenter, const char *text) {
    int textWidth = this->u8g2->getStrWidth(text);
    int textHeight = this->u8g2->getMaxCharHeight();
    int xPos = xCenter - (textWidth  / 2);  // Centra el texto
    int yPos = yCenter + (textHeight / 2) -1;  // Centra el texto
    this->u8g2->drawStr(xPos, yPos, text);
}

void GraphicLCD::setNewScreen(){
    this->newScreen = true;
}

void GraphicLCD::setProgressBarDelay(uint8_t index , uint8_t newDelay){
    this->progressBar[index-1].setValue(newDelay);
}
		
uint8_t GraphicLCD::getProgressBarDelay(uint8_t index ){
    return this->progressBar[index].getValue();
}

textInputLCD* GraphicLCD::getTextInput(uint8_t index) {
    return this->progressBar[index].getTextInput();  // Sin '&'
}

void GraphicLCD::selectMotor(int index,bool mode){
    this->motorIcon[index].setSelected(mode);
}

void GraphicLCD::selectChiller(int index,bool mode){
    this->chillerIcon[index].setSelected(mode);
}

void GraphicLCD::navigateMotor(int index,bool mode){
    this->motorIcon[index].setNavigated(mode);
}

void GraphicLCD::navigateChiller(int index,bool mode){
    this->chillerIcon[index].setNavigated(mode);
}