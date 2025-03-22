#include "ChillerGraphicLCD.hpp"

ChillerGraphicLCD::ChillerGraphicLCD()  {
}

ChillerGraphicLCD::ChillerGraphicLCD(U8G2_ST7920_128X64_F_SW_SPI *newu8g2)  {
    this->u8g2 = newu8g2;
    this->timer = 0 ;
    this->state = false;
}

void ChillerGraphicLCD::setID(uint8_t newID){
    this->ID = newID;
}

uint8_t ChillerGraphicLCD::getID(){
    return this->ID;
}

void ChillerGraphicLCD::setState(bool newState){
    this->state = newState;
}

bool ChillerGraphicLCD::getState(){
    return this->state;
}

void ChillerGraphicLCD::setRun(bool newRun){
    this->run = newRun;
}

bool ChillerGraphicLCD::getRun(){
    return this->run;
}

void ChillerGraphicLCD::setUpdateTimer(uint32_t newUpdateTimer){
    this->updateTimer = newUpdateTimer;
}

uint32_t ChillerGraphicLCD::getUpdateTimer(){
    return this->updateTimer;
}

void ChillerGraphicLCD::setTimer(uint32_t newTimer){
    this->timer = newTimer;
}

uint32_t ChillerGraphicLCD::getTimer(){
    return this->timer;
}

void ChillerGraphicLCD::drawCenteredText(int xCenter, int yCenter, const char *text) {
    int textWidth = this->u8g2->getStrWidth(text);
    int textHeight = this->u8g2->getMaxCharHeight();
    int xPos = xCenter - (textWidth  / 2);  // Centra el texto
    int yPos = yCenter + (textHeight / 2) -1;  // Centra el texto
    this->u8g2->drawStr(xPos, yPos, text);
}

void ChillerGraphicLCD::drawRotatedImage(int xPos, int yPos, const Bitmap &image, float angle) {
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

void ChillerGraphicLCD::drawImage(int xPos, int yPos, const Bitmap &image) {
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

void ChillerGraphicLCD::drawIcon(){
    if(this->iconState){
        this->drawImage(this->xPosition,this->yPosition,ICON_CHILLER_DATA);
    }
}

void ChillerGraphicLCD::showIcon(){
    this->iconState = true;
}
void ChillerGraphicLCD::hideIcon(){
    this->iconState = false;
}
/////////////////////
void ChillerGraphicLCD::setLabelPosition(uint8_t xCenter , uint8_t yCenter){
    this->xCenterLabel = xCenter;
    this->yCenterLabel = yCenter;
}

void ChillerGraphicLCD::drawLabelState(){
    int textWidth;
    this->u8g2->setFont(u8g2_font_6x10_tf);
    if(this->labelState){
        

        if(this->run){
            this->drawCenteredText(this->xCenterLabel,   this->yCenterLabel, "ON");
            textWidth = this->u8g2->getStrWidth("ON");
        }
        else{
            this->drawCenteredText(  this->xCenterLabel  ,  this->yCenterLabel, "OFF");
            textWidth= this->u8g2->getStrWidth("OFF");
        }

        if(this->selected){
            this->u8g2->setDrawColor(this->labelState); 
            this->drawImage(this->xCenterLabel - (ICON_BOX_22_11_DATA.width/2), this->yCenterLabel -(ICON_BOX_22_11_DATA.height/2) ,ICON_BOX_22_11_DATA); 
        }
    }
}

void ChillerGraphicLCD::showLabelState(){  
    this->labelState = true;
    //drawLabelState(1);
}
void ChillerGraphicLCD::hideLabelState(){
    this->labelState = false;
    //drawLabelState(0);
}

void ChillerGraphicLCD::setAnimation( bool newAnimation){
    this->animated = newAnimation;
}

///////////////
void ChillerGraphicLCD::animate(){
    if(!this->animated){
        return;
    }
    
    uint32_t externalTimer = millis();
    if(externalTimer - this->timer > this->updateTimer ) {
        this->setTimer(externalTimer);
        if(this->run){
            this->state = !this->state;
        }
        else{
            this->state = false;
        }
        //MOTOR 2
        if(this->state == false){
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(this->xPosition,this->yPosition,ICON_CHILLER_0_DATA.width,ICON_CHILLER_0_DATA.height);
            this->u8g2->setDrawColor(1);
            this->drawImage(this->xPosition,this->yPosition,ICON_CHILLER_0_DATA);
        }
        else{
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(this->xPosition,this->yPosition,ICON_CHILLER_1_DATA.width,ICON_CHILLER_1_DATA.height);
            this->u8g2->setDrawColor(1);
            this->drawImage(this->xPosition,this->yPosition,ICON_CHILLER_1_DATA);
        }
    }
}

void ChillerGraphicLCD::deanimate(){

}

void ChillerGraphicLCD::setPosition(uint8_t xpos , uint8_t ypos){
    this->xPosition = xpos;
    this->yPosition = ypos;
    this->setLabelPosition(xpos+32, ypos + 11);
}

void ChillerGraphicLCD::setSelected(){
    this->selected = true;
}


void ChillerGraphicLCD::setUnselected(){
    this->selected = false;
}

void ChillerGraphicLCD::update(){
    this->drawLabelState();
    this->drawIcon();
}
