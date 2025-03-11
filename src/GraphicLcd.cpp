#include "GraphicLCD.hpp"

GraphicLCD::GraphicLCD(const char * name, int taskCore) : Module(name, taskCore) {
}

void GraphicLCD::connect(void * data) {
    this->u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, 27, 26, 25, 14);
    motorIcon[0].number = 1;
    motorIcon[0].run = true;
    motorIcon[0].state = false;
    motorIcon[0].updateTimer = 250;
    motorIcon[0].timer = millis();
    motorIcon[1].number = 2;
    motorIcon[1].run = true;
    motorIcon[1].state = true;
    motorIcon[1].updateTimer = 250;
    motorIcon[1].timer = millis();
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

    progressBar[0].value = 0;
    progressBar[0].percentage = 0;
    progressBar[1].value = 0;
    progressBar[1].percentage = 0;

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
	while (1) {
		vTaskDelay(this->iterationDelay);
        this->updateIcons();
        //this->testprint();
	}
}

void GraphicLCD::print(int x, int y, char *text){
    this->u8g2->clearBuffer();
    this->u8g2->setCursor(x,y); 
    this->u8g2->print(text);
    this->u8g2->sendBuffer();
}

void GraphicLCD::splashScreen(){
    this->u8g2->clearBuffer();
    this->drawImage(17,5, IMAGE_SSL_LOGO_DATA);
    this->u8g2->sendBuffer();
}

void GraphicLCD::drawPage(uint8_t page){
    this->u8g2->clearBuffer();
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
    if(this->screenTimer - this->motorIcon[0].timer > this->motorIcon[0].updateTimer) {
        this->motorIcon[0].timer = this->screenTimer;
        if(this->motorIcon[0].run == true){
            motorIcon[0].state = !motorIcon[0].state;
            Serial.printf("Motor running state : %d \n" ,motorIcon[0].state);
        }
        //MOTOR 1
        if(motorIcon[0].state == false){
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(36,4,21,21);
            this->u8g2->setDrawColor(1);
            this->drawImage(36,4,ICON_MOTOR_DATA);
            this->u8g2->setDrawColor(0);
            this->u8g2->drawDisc(44, 13, 6);
            this->u8g2->setDrawColor(1);
            this->drawRotatedImage(36,5,ICON_FAN_11_11_DATA,0);
        }
        else{
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(36,4,21,21);
            this->u8g2->setDrawColor(1);
            this->drawImage(36,4,ICON_MOTOR_DATA);
            this->u8g2->setDrawColor(0);
            this->u8g2->drawDisc(44, 13, 6);
            this->u8g2->setDrawColor(1);
            this->drawRotatedImage(36,5,ICON_FAN_11_11_DATA,45);
        }
        this->drawProgressBar(0,64,5,ICON_PROGRESS_BAR_DATA);
        this->drawImage(90,5,ICON_CHILLER_DATA);
    }
    
    if(this->screenTimer - this->motorIcon[1].timer > this->motorIcon[1].updateTimer) {
        this->motorIcon[1].timer = this->screenTimer;
        if(this->motorIcon[1].run == true){
            motorIcon[1].state = !motorIcon[1].state;
            Serial.printf("Motor running state : %d \n" ,motorIcon[1].state);
        }

        else{
            motorIcon[1].state = false;
        }
        //MOTOR 2
        if(motorIcon[1].state == false){
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(36,38,21,21);
            this->u8g2->setDrawColor(1);
            this->drawImage(36,38,ICON_MOTOR_DATA);
            this->u8g2->setDrawColor(0);
            this->u8g2->drawDisc(44, 47, 6);
            this->u8g2->setDrawColor(1);
            this->drawRotatedImage(36,39,ICON_FAN_11_11_DATA,0);
        }
        else{
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(36,38,21,21);
            this->u8g2->setDrawColor(1);
            this->drawImage(36,38,ICON_MOTOR_DATA);
            this->u8g2->setDrawColor(0);
            this->u8g2->drawDisc(44, 47, 6);
            this->u8g2->setDrawColor(1);
            this->drawRotatedImage(36,39,ICON_FAN_11_11_DATA,45);
        }
        this->drawProgressBar(1,64,38,ICON_PROGRESS_BAR_DATA);
        this->drawImage(90,38,ICON_CHILLER_DATA);
        this->u8g2->sendBuffer();
    }
    // CHILLER 1
    this->u8g2->sendBuffer();
    //motorIcon 1
}

void GraphicLCD::drawConfigPage(){
    this->drawPage(1);
    char *text = new char[20];  // Asigna memoria dinámica
    strcpy(text, "12345"); 
    
    this->u8g2->setFont(u8g2_font_6x10_tf);
    this->u8g2->drawStr(40,12, "CHILLER 1");
    this->u8g2->drawStr(30,25, "DELAY=");
    this->textInput(70,15,35,text);

    this->u8g2->setFont(u8g2_font_6x10_tf);
    this->u8g2->drawStr(40,44, "CHILLER 2");
    this->u8g2->drawStr(30,57, "DELAY=");
    this->textInput(70,47,35,text);

    uint8_t dy = 32;
    this->u8g2->drawLine(18, 2, 126, 2);
    this->u8g2->drawLine(18, 30, 126, 30);
    this->u8g2->drawLine(18, 2, 18, 30);
    this->u8g2->drawLine(126, 2, 126, 30);

    this->u8g2->drawLine(18, 2+dy , 126, 2 +dy);
    this->u8g2->drawLine(18, 30+dy, 126, 30+dy);
    this->u8g2->drawLine(18, 2+dy , 18 , 30+dy);
    this->u8g2->drawLine(126,2+dy , 126, 30+dy);

    this->u8g2->sendBuffer();
}

void GraphicLCD::drawManualPage(){
    this->drawPage(2);
    uint8_t dy = 32;
    this->u8g2->drawLine(18, 2, 126, 2);
    this->u8g2->drawLine(18, 30, 126, 30);
    this->u8g2->drawLine(18, 2, 18, 30);
    this->u8g2->drawLine(126, 2, 126, 30);

    this->u8g2->drawLine(18, 2+dy , 126, 2 +dy);
    this->u8g2->drawLine(18, 30+dy, 126, 30+dy);
    this->u8g2->drawLine(18, 2+dy , 18 , 30+dy);
    this->u8g2->drawLine(126,2+dy , 126, 30+dy);
    
    this->u8g2->sendBuffer();
}
void GraphicLCD::drawLogPage(){
    this->drawPage(3);
    this->u8g2->sendBuffer();
}

void GraphicLCD::setMotorState(uint8_t motorNumber , bool state){
    this->motorIcon[motorNumber].state = state;
}

void GraphicLCD::setChillerState(uint8_t chillerNumber , bool state){
    this->chillerIcon[chillerNumber].state = state;
}
void GraphicLCD::updateIcons(){
    this->screenTimer = millis();
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
}

void GraphicLCD::drawProgressBar(uint8_t number , uint8_t posX , uint8_t posY, const Bitmap &image){
    this->drawImage(posX,posY,image);
    uint16_t cicles = this->progressBar[number].percentage / 5 ;
    for(int i= 0; i< cicles ;i++){
        this->u8g2->drawLine(posX + i, posY +1 , posX + i , posY + 3);
    }
    this->u8g2->setFont(u8g2_font_ncenB08_tr);  // Selecciona una fuente

    char buffer[10];  
    itoa(this->progressBar[number].value, buffer, 10);  // Convierte el número a cadena en base 10
    this->u8g2->drawStr(posX + 8 , posY + 17 , buffer );  // Dibuja texto en (10,20)

    //this->u8g2->drawSTR;
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
}

void GraphicLCD::setProgressBarValue(uint8_t index ,uint16_t newValue){
    this->progressBar[index].value = newValue;
}

void GraphicLCD::setProgressBarPercentage(uint8_t index ,uint8_t newPercentage){
    this->progressBar[index].percentage = newPercentage;
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

    this->u8g2->drawStr(20,21,"1");
    this->u8g2->drawStr(20,53,"2");
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