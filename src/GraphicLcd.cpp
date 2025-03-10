#include "GraphicLCD.hpp"

GraphicLCD::GraphicLCD(const char * name, int taskCore) : Module(name, taskCore) {
}

void GraphicLCD::connect(void * data) {
    this->u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, 27, 26, 25, 14);
    motor[0].number = 1;
    motor[0].state = false;
    motor[1].number = 1;
    motor[1].state = true;
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
    this->drawPage(0);
	while (1) {
		vTaskDelay(this->iterationDelay);
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
            //MOTOR 1
            if(motor[0].state == false){
                this->drawImage(32,1,ICON_MOTOR_STOP_DATA);
                }
            else{
                this->drawImage(32,1,ICON_MOTOR_RUN_DATA);
                }
            //MOTOR 2
            if(motor[1].state == false){
                this->drawImage(64,1,ICON_MOTOR_STOP_DATA);
                }
            else{
                this->drawImage(64,1,ICON_MOTOR_RUN_DATA);
                }
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
    this->u8g2->sendBuffer();
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

void GraphicLCD::setMotorState(uint8_t motorNumber , bool state){
    this->motor[motorNumber].state = state;
}
