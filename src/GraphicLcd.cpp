#include "GraphicLCD.hpp"

GraphicLCD::GraphicLCD(const char * name, int taskCore) : Module(name, taskCore) {
}

void GraphicLCD::connect(void * data) {
    this->u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, 27, 26, 25, 14);
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

