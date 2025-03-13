#include "progressBarLCD.hpp"

progressBarLCD::progressBarLCD(){

}

progressBarLCD::progressBarLCD(U8G2_ST7920_128X64_F_SW_SPI *newu8g2)  {
    this->u8g2 = newu8g2;
    this->timer = 0 ;
    this->textInputDelay = new textInputLCD[2];
    for (int i = 0; i < 2; i++) {
        textInputDelay[i]   = textInputLCD(u8g2);
    }

    for(int i=0 ;i<2;i++){
        textInputDelay[i].setID(i+1);
        textInputDelay[i].setRun(true) ;
        textInputDelay[i].setState(false);
        textInputDelay[i].setUpdateTimer(200);
        textInputDelay[i].setTimer(millis());
        textInputDelay[i].setWidth(30);
        textInputDelay[i].setHeight(11);
        textInputDelay[i].setLabelInput("Delay");
    }
}

void progressBarLCD::setID(uint8_t newID){
    this->ID = newID;
}

uint8_t progressBarLCD::getID(){
    return this->ID;
}

void progressBarLCD::setState(bool newState){
    this->state = newState;
}

bool progressBarLCD::getState(){
    return this->state;
}

void progressBarLCD::setRun(bool newRun){
    this->run = newRun;
}

bool progressBarLCD::getRun(){
    return this->run;
}

void progressBarLCD::setUpdateTimer(uint32_t newUpdateTimer){
    this->updateTimer = newUpdateTimer;
}

uint32_t progressBarLCD::getUpdateTimer(){
    return this->updateTimer;
}

void progressBarLCD::setTimer(uint32_t newTimer){
    this->timer = newTimer;
}

uint32_t progressBarLCD::getTimer(){
    return this->timer;
}

void progressBarLCD::setCounter(uint8_t newCounter){
    this->counter = newCounter;
}

void progressBarLCD::setValue(uint8_t newValue){
    this->value = newValue;
    this->textInputDelay[0].setInteger(newValue);
}

uint8_t progressBarLCD::getValue(){
    return this->value;
}

void progressBarLCD::setPercentage(uint8_t newPercentage){
    this->percentage = newPercentage;
}

uint8_t progressBarLCD::getPercentage(){
    return this->percentage;
}

void progressBarLCD::setPosition(uint8_t xpos , uint8_t ypos){
    this->xPosition = xpos;
    this->yPosition = ypos;
    this->setLabelPosition(xpos, ypos + 10);
    this->textInputDelay[0].setPosition(xPosition,yPosition);
    this->textInputDelay[1].setPosition(xPosition,yPosition + 32);
}

void progressBarLCD::drawIcon(bool show){
    if(show){
        this->drawImage(this->xPosition,this->yPosition,ICON_PROGRESS_BAR_DATA);
    }
}
void progressBarLCD::showIcon(){
    this->drawIcon(1);
}
void progressBarLCD::hideIcon(){
    this->drawIcon(0);
}
/////////////////////
void progressBarLCD::setLabelPosition(uint8_t xCenter , uint8_t yCenter){
    this->xCenterLabel = xCenter;
    this->yCenterLabel = yCenter;
}

void progressBarLCD::drawLabelState(bool show){
    int textWidth;
    this->u8g2->setFont(u8g2_font_6x10_tf);
    if(show){
        this->u8g2->setFont(u8g2_font_ncenB08_tr);  // Selecciona una fuente
        char buffer[10];  
        itoa(this->counter, buffer, 10);  // Convierte el número a cadena en base 10
        this->drawCenteredText(this->xCenterLabel + 11 , this->yCenterLabel + 4 , buffer );  // Dibuja texto en (10,20)
    }
}

void progressBarLCD::showLabelState(){  
    drawLabelState(1);
}
void progressBarLCD::hideLabelState(){
    drawLabelState(0);
}

void progressBarLCD::animate(uint32_t externalTimer){
    if(externalTimer - this->timer > this->updateTimer ) {
        uint16_t cicles = this->percentage / 5 ;
        for(int i= 0; i< cicles ;i++){
            this->u8g2->drawLine(this->xPosition + i, this->yPosition +1 , this->xPosition + i ,  this->yPosition + 3);
        }
    }
}
///////////////
//void MotorGraphicLCD::animate(uint32_t externalTimer){

///////////////////////

void progressBarLCD::drawCenteredText(int xCenter, int yCenter, const char *text) {
    int textWidth = this->u8g2->getStrWidth(text);
    int textHeight = this->u8g2->getMaxCharHeight();
    int xPos = xCenter - (textWidth  / 2);  // Centra el texto
    int yPos = yCenter + (textHeight / 2) -1;  // Centra el texto
    this->u8g2->drawStr(xPos, yPos, text);
}

void progressBarLCD::drawRotatedImage(int xPos, int yPos, const Bitmap &image, float angle) {
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

void progressBarLCD::drawImage(int xPos, int yPos, const Bitmap &image) {
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

////////////////////////

void progressBarLCD::drawTextInput(bool show){
    if(show){
        this->textInputDelay[0].show();
    }
}
void progressBarLCD::showTextInput(){
    this->drawTextInput(1);
}
void progressBarLCD::hideTextInput(){
    this->drawTextInput(0);
}

void progressBarLCD::showLabelInput(){
    this->textInputDelay[0].showLabelInput();
}
void progressBarLCD::hideLabelInput(){
    this->textInputDelay[0].hideLabelInput();
}