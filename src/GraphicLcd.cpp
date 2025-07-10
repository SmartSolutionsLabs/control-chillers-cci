#include "lvgl.h"
#include "GraphicLcd.hpp"
#include "images/Logo.h"

LV_IMAGE_DECLARE(imageLogo);

GraphicLCD::GraphicLCD(const char * name, int taskCore) : Module(name, taskCore) {
}

void GraphicLCD::connect(void *data) {
	Serial.print("GraphicLCD::connect\n");
	// Crear instancia de U8G2 (SPI software)
	this->u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, 47, 21, 14, 38); // SCLK=47, MOSI=21, CS=14, RESET=38

	// Inicializar hardware (pantalla)
	this->u8g2->begin();
	this->u8g2->clearBuffer();
	this->u8g2->setDrawColor(0);

	// Inicializar LVGL
	lv_init();

	lv_tick_set_cb(GraphicLCD::getTickCount);

	// Crear framebuffer de 1 bit para 128x64
	static lv_color_t buf[128 * 64 / 8];

	// Crear display LVGL y asignar callbacks
	this->display = lv_display_create(128, 64);
	lv_display_set_color_format(this->display, LV_COLOR_FORMAT_I1);
	lv_display_set_flush_cb(this->display, GraphicLCD::displayFlush);

	// Asignar framebuffer y backend (modo directo)
	lv_display_set_buffers(this->display, buf, nullptr, sizeof(buf), LV_DISPLAY_RENDER_MODE_FULL);

	// Guardar puntero u8g2 como user_data
	lv_display_set_user_data(this->display, this->u8g2);

	this->setInitialized(true); // Marca como inicializado
}

void GraphicLCD::run(void* data) {
	Serial.print("GraphicLCD::run\n");

	this->initSplashScreen();

	// Bucle de refresco (FreeRTOS task)
	while (1) {
		lv_timer_handler();                 // Procesa eventos de LVGL
		vTaskDelay(48 / portTICK_PERIOD_MS); // Pequeño retardo
	}
}

void GraphicLCD::displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
	U8G2 *u8g2 = static_cast<U8G2 *>(lv_display_get_user_data(disp));

	// Always clear whole screen
	u8g2->clearBuffer();
	u8g2->drawXBMP(0, 0, 128, 64, px_map);
	u8g2->sendBuffer();

	lv_display_flush_ready(disp);
}

uint32_t GraphicLCD::getTickCount() {
	return esp_timer_get_time() / 1000;
}

void GraphicLCD::initSplashScreen() {
	this->splashScreen = lv_img_create(lv_screen_active());
	lv_image_set_src(this->splashScreen, &imageLogo);
	lv_obj_set_pos(this->splashScreen, 0, 0);
}

void GraphicLCD::clearSplashScreen() {
	lv_obj_del(this->splashScreen);
}

void GraphicLCD::update(){
	if(millis() - this->timerFPS < 100){
		return;
	}

	this->timerFPS = millis();

	//~ static bool lastMotorState[2] = {false, false};  // Guardar el último estado de los motores
	//~ static bool lastChillerState[2] = {false, false};  // Guardar el último estado de los chillers
	//~ static uint16_t lastDelay1 = progressBar[0].getValue();  // Guardar el último valor de delay1
	//~ static uint16_t lastDelay2 = progressBar[1].getValue();  // Guardar el último valor de delay2

	//~ bool motorStateChanged = (this->motorIcon[0].getState() != lastMotorState[0]) ||
							 //~ (this->motorIcon[1].getState() != lastMotorState[1]);
	//~ bool chillerStateChanged = (this->chillerIcon[0].getState() != lastChillerState[0]) ||
							   //~ (this->chillerIcon[1].getState() != lastChillerState[1]);
	//~ bool delayChanged = (progressBar[0].getValue() != lastDelay1) ||
						//~ (progressBar[1].getValue() != lastDelay2);

	//if (this->newScreen || motorStateChanged || chillerStateChanged || delayChanged ) {
	if (true){
		switch (this->currentScreen) {
			case HOME:
				//~ this->drawHomePage();
				break;
			case CONFIG:
				//~ this->drawConfigPage();
				break;
			case MANUAL:
				//~ this->drawManualPage();
				break;
			case LOG:
				//~ this->drawLogPage();
				break;
		}

		// Actualizar los últimos estados
		//~ lastMotorState[0]   =   this->motorIcon[0].getState();
		//~ lastMotorState[1]   =   this->motorIcon[1].getState();
		//~ lastChillerState[0] = this->chillerIcon[0].getState();
		//~ lastChillerState[1] = this->chillerIcon[1].getState();
		//~ lastDelay1 = progressBar[0].getValue();
		//~ lastDelay2 = progressBar[1].getValue();
	}
}

void GraphicLCD::setScreen(Screen newScreen){
	this->currentScreen = newScreen;
	this->setNewScreen();
}

void GraphicLCD::setNewScreen(){
	this->newScreen = true;
}
