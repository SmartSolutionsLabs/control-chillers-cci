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
        Serial.println(this->name);
        //this->testprint();
	}
}

void GraphicLCD::print(int x, int y, char *text){
    this->u8g2->clearBuffer();
    this->u8g2->setCursor(x,y); 
    this->u8g2->print(text);
    this->u8g2->sendBuffer();
}