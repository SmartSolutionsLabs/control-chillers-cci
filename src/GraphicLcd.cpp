#include "GraphicLCD.hpp"

GraphicLCD::GraphicLCD(const char * name, int taskCore) : Module(name, taskCore) {
}

void GraphicLCD::connect(void * data) {
    this->u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, 27, 26, 25, 14);
    this->motorIcon = new MotorGraphicLCD[2];
    this->progressBar = new progressBarLCD[2];

    for (int i = 0; i < 2; i++) {
        motorIcon[i]   = MotorGraphicLCD(u8g2);
        progressBar[i] = progressBarLCD(u8g2);
    }

    for(int i=0 ;i<2;i++){
        motorIcon[i].setID(i+1);
        motorIcon[i].setRun(true) ;
        motorIcon[i].setState(false);
        motorIcon[i].setUpdateTimer(250);
        motorIcon[i].setTimer(millis());

        progressBar[i].setID(i+1);
        progressBar[i].setRun(false) ;
        progressBar[i].setState(false);
        progressBar[i].setUpdateTimer(250);
        progressBar[i].setTimer(millis());
        progressBar[i].setValue(0);
        progressBar[i].setPercentage(0);
    }
    chillerIcon[0].number = 1;
    chillerIcon[0].run = true;
    chillerIcon[0].state = 1;
    chillerIcon[0].updateTimer = 1;
    chillerIcon[0].timer = millis();
    chillerIcon[1].number = 2;
    chillerIcon[1].run = true;
    chillerIcon[1].state = 1;
    chillerIcon[1].updateTimer = 1;
    chillerIcon[1].timer = millis();


    if (!this->u8g2->begin()) {
        Serial.println("Display initialization failed!");
    }
    else{
        Serial.println("Display ok!");
    }
    this->u8g2->setFont(u8g2_font_6x10_tf);
}

void GraphicLCD::run(void* data) {
	this->iterationDelay = 1 / portTICK_PERIOD_MS;
    this->screenTimer = millis();
    this->splashScreen();
    vTaskDelay(splashScreenTimer/portTICK_PERIOD_MS);
    this->u8g2->clearBuffer();
	while (1) {
		vTaskDelay(this->iterationDelay);
        if(newScreen){
            newScreen = false;
        }//this->testprint();
        else{
            this->update();
        }
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

void GraphicLCD::drawPage(uint8_t page){
    this->u8g2->drawLine(16, 0, 16, 64);
    this->u8g2->drawLine(0, 16, 16, 16);
    this->u8g2->drawLine(0, 32, 16, 32);
    this->u8g2->drawLine(0, 48, 16, 48);
    switch (page){
        case 0:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_SELECT_DATA);
            this->drawImage(1,17,ICON_CONFIG_DATA);
            this->drawImage(1,33,ICON_MANUAL_DATA);
            this->drawImage(1,49,ICON_LOG_DATA);    
            break;
        case 1:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_DATA);
            this->drawImage(1,17,ICON_CONFIG_SELECT_DATA);
            this->drawImage(1,33,ICON_MANUAL_DATA);
            this->drawImage(1,49,ICON_LOG_DATA); 
            break;
        case 2:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_DATA);
            this->drawImage(1,17,ICON_CONFIG_DATA);
            this->drawImage(1,33,ICON_MANUAL_SELECT_DATA);
            this->drawImage(1,49,ICON_LOG_DATA); 
            break;
        case 3:
            this->u8g2->setDrawColor(1);
            this->drawImage(1,1,ICON_HOME_DATA);
            this->drawImage(1,17,ICON_CONFIG_DATA);
            this->drawImage(1,33,ICON_MANUAL_DATA);
            this->drawImage(1,49,ICON_LOG_SELECT_DATA); 
            break;
    }
    //this->drawImage(0,0, IMAGE_HOME_SCREEN_DATA);
}

void GraphicLCD::drawHomePage(){
    this->drawPage(0);
    this->drawBoxes();
    motorIcon[0].setPosition(33,5);
    motorIcon[0].showIcon();
    motorIcon[0].hideLabelState();
    motorIcon[0].animate(millis());

    progressBar[0].setPosition(62,5);
    progressBar[0].showIcon();
    progressBar[0].showLabelState();
    progressBar[0].animate(millis());
    this->drawImage(90,5,ICON_CHILLER_DATA);
    
    
    motorIcon[1].setPosition(33,37);
    motorIcon[1].showIcon();
    motorIcon[1].hideLabelState();
    motorIcon[1].animate(millis());

    progressBar[1].setPosition(62,37);
    progressBar[1].showIcon();
    progressBar[1].showLabelState();
    progressBar[1].animate(millis());
    this->drawImage(90,37,ICON_CHILLER_DATA);
    // CHILLER 1
    this->u8g2->sendBuffer();
    //motorIcon 1
}

void GraphicLCD::drawConfigPage(){
    this->drawPage(1);
    char *text = new char[20];  // Asigna memoria dinámica
    strcpy(text, "12345"); 
    
    this->u8g2->setFont(u8g2_font_6x10_tf);
    this->u8g2->drawStr(42,12, "CHILLER 1");
    this->u8g2->drawStr(30,25, "DELAY=");
    this->textInput(70,15,35,text);

    this->u8g2->setFont(u8g2_font_6x10_tf);
    this->u8g2->drawStr(42,44, "CHILLER 2");
    this->u8g2->drawStr(30,57, "DELAY=");
    this->textInput(70,47,35,text);

    this->drawBoxes();
    this->u8g2->sendBuffer();
}

void GraphicLCD::drawManualPage(){
    this->drawPage(2);
    this->drawBoxes();

    motorIcon[0].setPosition(33,5);
    motorIcon[0].showIcon();
    motorIcon[0].showLabelState();
    motorIcon[0].animate(millis());

    motorIcon[1].setPosition(33,37);
    motorIcon[1].showIcon();
    motorIcon[1].showLabelState();
    motorIcon[1].animate(millis());

    this->u8g2->sendBuffer();
}
void GraphicLCD::drawLogPage(){
    this->drawPage(3);
    this->u8g2->sendBuffer();
}

void GraphicLCD::setMotorState(uint8_t motorNumber , bool state){
    this->motorIcon[motorNumber].setState(state);
}

void GraphicLCD::setChillerState(uint8_t chillerNumber , bool state){
    this->chillerIcon[chillerNumber].state = state;
}
void GraphicLCD::update(){
    this->screenTimer = millis();
    this->u8g2->clearBuffer();
    switch(currentScreen){
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
    this->progressBar[index].setValue(newValue);
}

void GraphicLCD::setProgressBarPercentage(uint8_t index ,uint8_t newPercentage){
    this->progressBar[index].setPercentage(newPercentage);
}

void GraphicLCD::nextScreen(){
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
}

void GraphicLCD::previousScreen(){
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
}

Screen GraphicLCD::getScreen(){
    return this->currentScreen;
}

void GraphicLCD::drawBoxes(){
    this->u8g2->setFont(u8g2_font_ncenB12_tr);
    uint8_t dy = 32;
    this->u8g2->drawLine(18, 2, 126, 2);
    this->u8g2->drawLine(18, 30, 126, 30);
    this->u8g2->drawLine(18, 2, 18, 30);
    this->u8g2->drawLine(126, 2, 126, 30);

    this->u8g2->drawLine(18, 2+dy , 126, 2 +dy);
    this->u8g2->drawLine(18, 30+dy, 126, 30+dy);
    this->u8g2->drawLine(18, 2+dy , 18 , 30+dy);
    this->u8g2->drawLine(126,2+dy , 126, 30+dy);

    this->u8g2->drawStr(19,22   , "1");
    this->u8g2->drawStr(19,22+dy, "2");

    this->u8g2->drawLine(29,  2    , 29 ,29);
    this->u8g2->drawLine(29,  2+dy , 29, 29 +dy);

}

void GraphicLCD::textInput(uint8_t posX , uint8_t posY, uint8_t size, char *text){
    char buffer[20];
    strncpy(buffer, text, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    this->u8g2->setDrawColor(1);
    this->u8g2->drawBox(posX  ,posY  ,size    , 13  );
    this->u8g2->setDrawColor(0);
    this->u8g2->drawBox(posX+1,posY+1,size - 2, 11  );
    this->u8g2->setDrawColor(1);
    this->u8g2->setFont(u8g2_font_6x10_tf);
    this->u8g2->drawStr(posX + 2 ,posY + 10 , buffer);   
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