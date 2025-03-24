#include "WifiGraphicLCD.hpp"

WifiGraphicLCD::WifiGraphicLCD()  {
}

WifiGraphicLCD::WifiGraphicLCD(U8G2_ST7920_128X64_F_SW_SPI *newu8g2)  {
    this->u8g2 = newu8g2;
    this->timer = 0 ;
    this->state = false;
    this->iconState = false;
}

void WifiGraphicLCD::setID(uint8_t newID){
    this->ID = newID;
}

uint8_t WifiGraphicLCD::getID(){
    return this->ID;
}

void WifiGraphicLCD::setState(bool newState){
    this->state = newState;
}

bool WifiGraphicLCD::getState(){
    return this->state;
}

void WifiGraphicLCD::setRun(bool newRun){
    this->run = newRun;
}

bool WifiGraphicLCD::getRun(){
    return this->run;
}

void WifiGraphicLCD::setUpdateTimer(uint32_t newUpdateTimer){
    this->updateTimer = newUpdateTimer;
}

uint32_t WifiGraphicLCD::getUpdateTimer(){
    return this->updateTimer;
}

void WifiGraphicLCD::setTimer(uint32_t newTimer){
    this->timer = newTimer;
}

uint32_t WifiGraphicLCD::getTimer(){
    return this->timer;
}

void WifiGraphicLCD::drawCenteredText(int xCenter, int yCenter, const char *text) {
    int textWidth = this->u8g2->getStrWidth(text);
    int textHeight = this->u8g2->getMaxCharHeight();
    int xPos = xCenter - (textWidth  / 2);  // Centra el texto
    int yPos = yCenter + (textHeight / 2) -1;  // Centra el texto
    this->u8g2->drawStr(xPos, yPos, text);
}

void WifiGraphicLCD::drawRotatedImage(int xPos, int yPos, const Bitmap &image, float angle) {
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

void WifiGraphicLCD::drawImage(int xPos, int yPos, const Bitmap &image) {
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

void WifiGraphicLCD::drawIcon(){
    if(this->iconState){
        this->drawImage(this->xPosition,this->yPosition,ICON_WIFI_CONECTED_FULL_DATA);
    }
}
void WifiGraphicLCD::showIcon(){
    this->iconState = true;
}
void WifiGraphicLCD::hideIcon(){
    this->iconState = false;
}
/////////////////////
void WifiGraphicLCD::setLabelPosition(uint8_t xCenter , uint8_t yCenter){
    this->xCenterLabel = xCenter;
    this->yCenterLabel = yCenter;
}

void WifiGraphicLCD::drawLabelState(){
    int textWidth;
    this->u8g2->setFont(u8g2_font_6x10_tf);
    if(this->labelState){
        if(this->selected){
            this->u8g2->setDrawColor(1); 
            this->u8g2->drawBox(this->xCenterLabel - 11, this->yCenterLabel - 5, 22, 11);
            this->u8g2->setDrawColor(0); // Texto en negro
            //this->drawImage(this->xCenterLabel - (ICON_BOX_22_11_DATA.width/2), this->yCenterLabel -(ICON_BOX_22_11_DATA.height/2) ,ICON_BOX_22_11_DATA);
        } 
        else if(this->navigated){
            // Estado "navigated": dibujar solo el contorno del rectángulo en blanco,
            // y luego el texto en blanco
            this->u8g2->setDrawColor(1);
            this->u8g2->drawFrame(this->xCenterLabel - 11, this->yCenterLabel - 5, 22, 11);
            this->u8g2->setDrawColor(1); // Texto en blanco
            //this->drawImage(this->xCenterLabel - (ICON_BOX_22_11_DATA.width/2), this->yCenterLabel -(ICON_BOX_22_11_DATA.height/2) ,ICON_BOX_22_11_DATA);
        }

        else{
            // Caso por defecto: simplemente dibuja el texto (podrías ajustar según tu necesidad)
            // Aquí se podría elegir un color predeterminado
            this->u8g2->setDrawColor(1);
        }
        
        if(this->run){
            this->drawCenteredText(this->xCenterLabel,   this->yCenterLabel, "ON");
        }
        else{
            this->drawCenteredText(  this->xCenterLabel  ,  this->yCenterLabel, "OFF");
        }
    }
}

void WifiGraphicLCD::showLabelState(){  
    this->labelState = true;
}
void WifiGraphicLCD::hideLabelState(){
    this->labelState = false;
}
///////////////
void WifiGraphicLCD::setAnimation( bool newAnimation){
    this->animated = newAnimation;
}
void WifiGraphicLCD::animate(){
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
        if(this->state == false){
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(this->xPosition,this->yPosition,ICON_MOTOR_DATA.width,ICON_MOTOR_DATA.height);
            this->u8g2->setDrawColor(1);
            this->drawImage(this->xPosition,this->yPosition,ICON_MOTOR_DATA);
            this->u8g2->setDrawColor(0);
            this->u8g2->drawDisc(this->xPosition + 8 ,this->yPosition + 9 , 6);
            this->u8g2->setDrawColor(1);
            this->drawRotatedImage(this->xPosition,this->yPosition + 1 ,ICON_FAN_11_11_DATA , 0);
        }
        else{
            this->u8g2->setDrawColor(0);
            this->u8g2->drawBox(this->xPosition,this->yPosition,ICON_MOTOR_DATA.width,ICON_MOTOR_DATA.height);
            this->u8g2->setDrawColor(1);
            this->drawImage(this->xPosition,this->yPosition,ICON_MOTOR_DATA);
            this->u8g2->setDrawColor(0);
            this->u8g2->drawDisc(this->xPosition + 8 ,this->yPosition + 9 , 6);
            this->u8g2->setDrawColor(1);
            this->drawRotatedImage(this->xPosition,this->yPosition + 1 ,ICON_FAN_11_11_DATA , 45);
        }
    }
}

void WifiGraphicLCD::deanimate(){
    this->animated = false;
}

void WifiGraphicLCD::setIpPosition(uint8_t xpos , uint8_t ypos){
    this->xCenterLabelIp = xpos;
    this->yCenterLabelIp = ypos;
}

void WifiGraphicLCD::setPosition(uint8_t xpos , uint8_t ypos){
    this->xPosition = xpos;
    this->yPosition = ypos;
    this->setLabelPosition(xpos+32, ypos + 11);
    this->setIpPosition(xpos, ypos + 21);
}

void WifiGraphicLCD::drawIp(){
    String ipString = this->ip.toString();
    char buffer[20];  // Asegúrate de que el tamaño sea suficiente para la dirección IP y el caracter nulo
    ipString.toCharArray(buffer, sizeof(buffer));

    this->u8g2->drawStr(this->xCenterLabelIp-15, this->yCenterLabelIp, buffer);
}

void WifiGraphicLCD::update(){
    //this->drawLabelState();
    this->drawIcon();
    this->drawIp();
}

void WifiGraphicLCD::setLabelText(IPAddress newIp){
    this->ip = newIp;
    Serial.printf("WifiGraphicLCD::setLabelText(char *newIp) : %c \n",this->ip);
}