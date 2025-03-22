#include "progressBarLCD.hpp"

progressBarLCD::progressBarLCD(){

}

progressBarLCD::progressBarLCD(U8G2_ST7920_128X64_F_SW_SPI *newu8g2)  {
    this->u8g2 = newu8g2;
    this->timer = 0 ;
    this->selected = false;
    this->textInputDelay = new textInputLCD(u8g2);
    this->counter = 0;

    this->textInputDelay->setID(1);
    this->textInputDelay->setRun(true) ;
    this->textInputDelay->setState(InputState::NONE);
    this->textInputDelay->setUpdateTimer(200);
    this->textInputDelay->setTimer(millis());
    this->textInputDelay->setWidth(30);
    this->textInputDelay->setHeight(11);
    this->textInputDelay->setLabelInput("Delay");
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

void progressBarLCD::incrementCounter() {
    if (this->counter < 100) {  // Asegurarse de no exceder el límite
        this->counter++;
    }
}

void progressBarLCD::resetCounter() {
    this->counter = 0;
}

uint32_t progressBarLCD::getCounter() const {
    return this->counter;
}

void progressBarLCD::setCounter(uint32_t newCounter){
    this->counter = newCounter;
}

void progressBarLCD::setValue(uint16_t newValue){
    this->value = newValue;
    this->textInputDelay->setInteger(newValue);
}

uint16_t progressBarLCD::getValue(){
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
    this->setLabelPosition(xpos, ypos + 8);
    this->textInputDelay->setPosition(xPosition,yPosition);
}

void progressBarLCD::drawIcon(){
    if(this->iconState){
        this->drawImage(this->xPosition,this->yPosition,ICON_PROGRESS_BAR_DATA);
    }
}

void progressBarLCD::showIcon(){
    this->iconState = true;
}

void progressBarLCD::hideIcon(){
    this->iconState = false;
}
/////////////////////
void progressBarLCD::setLabelPosition(uint8_t xCenter , uint8_t yCenter){
    this->xCenterLabel = xCenter;
    this->yCenterLabel = yCenter;
}

void progressBarLCD::drawLabelState(){
    int textWidth;
    this-> width  = 10;
    this->u8g2->setFont(u8g2_font_6x10_tf);
    if (this->showLblState) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", this->counter);  // Convertir el delay a cadena

        // Calcular el ancho y alto del texto
        int textWidth = this->u8g2->getStrWidth(buffer);
        int textHeight = this->u8g2->getMaxCharHeight();
        
        // Dibujar el número de delay dentro del cuadro
        this->drawCenteredText(this->xCenterLabel+ this-> width  ,this->yCenterLabel + textHeight/2 ,buffer);
    }
}

void progressBarLCD::showLabelState(){  
    this->showLblState = true ;
}

void progressBarLCD::hideLabelState(){
    this->showLblState = false ;
}

void progressBarLCD::animate(){
    uint32_t externalTimer = millis();
    if(externalTimer - this->timer > this->updateTimer ) {
        uint16_t cicles = this->percentage / 5 ;
        for(int i= 0; i< cicles ;i++){
            this->u8g2->drawLine(this->xPosition + i, this->yPosition +1 , this->xPosition + i ,  this->yPosition + 3);
        }
    }
}

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

void progressBarLCD::drawTextInput(){
    if(this->showTxtInput){
        this->textInputDelay->show();
    }
}

void progressBarLCD::showTextInput(){
    this->showTxtInput = true;
}

void progressBarLCD::hideTextInput(){
    this->showTxtInput = false;
}

void progressBarLCD::showLabelInput(){
    this->textInputDelay->showLabelInput();
}

void progressBarLCD::hideLabelInput(){
    this->textInputDelay->hideLabelInput();
}

void progressBarLCD::setSelected(bool isSelected) {
    this->selected = isSelected;
    
    if( this->selected && !this->navigated){
        // Cambiar el estado a SELECTED
        this->textInputDelay->setState(InputState::SELECTED);
        this->textInputDelay->show();
    }
    else{
        // Cambiar el estado a NONE
        this->textInputDelay->setState(InputState::NONE);
        this->textInputDelay->show();
    }
}

bool progressBarLCD::isSelected() const {
    return this->selected;
}

void progressBarLCD::setNavigated(bool isNavigated) {
    this->navigated = isNavigated;
    if(isNavigated && !this->selected){
        // Cambiar el estado a NAVIGATED
        this->textInputDelay->setState(InputState::NAVIGATED);
        this->textInputDelay->show();
    }
    else{
        // Cambiar el estado a NONE
        this->textInputDelay->setState(InputState::NONE);
        this->textInputDelay->show();
    }
}

bool progressBarLCD::isNavigated() const {
    return this->navigated;
}