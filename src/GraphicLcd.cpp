#include "GraphicLcd.hpp"
#include "images/Logo.h"

LV_IMAGE_DECLARE(imageLogo);

uint8_t GraphicLCD::bitReverseTable[256] = {};

GraphicLCD::~GraphicLCD() {
	// Cleanup resources if necessary
}

GraphicLCD::GraphicLCD() {
	for (unsigned int i = 255; i > 0; --i) {
		uint8_t b = static_cast<uint8_t>(i);
		b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; // intercambia mitades
		b = (b & 0xCC) >> 2 | (b & 0x33) << 2; // intercambia pares
		b = (b & 0xAA) >> 1 | (b & 0x55) << 1; // intercambia bits individuales

		GraphicLCD::bitReverseTable[i] = b;
	}

	GraphicLCD::bitReverseTable[0] = 0; // Zero = Zero outside loop
}

void GraphicLCD::init() {
	// Inicializar LVGL
	lv_init();

#ifndef USING_EMULATOR
	// Crear instancia de U8G2 (via SPI)
	this->u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, 47, 21, 14, 38); // SCLK=47, MOSI=21, CS=14, RESET=38
	// Inicializar hardware (pantalla)
	this->u8g2->begin();
	this->u8g2->clearBuffer();
	this->u8g2->setDrawColor(0);

	lv_tick_set_cb(GraphicLCD::getTickCount);

	// Crear framebuffer de 1 bit para 128x64
	static lv_color_t buf[DSP_HOR_RES * DSP_VER_RES >> 3];

	// Crear display LVGL y asignar callbacks
	this->display = lv_display_create(DSP_HOR_RES, DSP_VER_RES);
	lv_display_set_color_format(this->display, LV_COLOR_FORMAT_I1);
	lv_display_set_flush_cb(this->display, GraphicLCD::displayFlush);

	// Asignar framebuffer y backend (modo directo)
	lv_display_set_buffers(this->display, buf, nullptr, sizeof(buf), LV_DISPLAY_RENDER_MODE_FULL);

	// Guardar puntero u8g2 como user_data
	lv_display_set_user_data(this->display, this->u8g2);

	this->setInitialized(true); // Marca como inicializado
#else
	#ifndef WIN32
		setenv("DBUS_FATAL_WARNINGS", "0", 1);
	#endif

	this->display = lv_sdl_window_create(DSP_HOR_RES, DSP_VER_RES);
	this->lvMouse = lv_sdl_mouse_create();
	this->lvMouseWheel = lv_sdl_mousewheel_create();
	this->lvKeyboard = lv_sdl_keyboard_create();

	lv_sdl_window_set_title(this->display, "SSL chiller emulator");
#endif
}

void GraphicLCD::initMenu() {
	/*Create a list*/
	list1 = lv_list_create(lv_screen_active());
	lv_obj_set_pos(list1, 0, 0);
	lv_obj_set_size(list1, 24, lv_pct(100));
	lv_obj_set_style_pad_row(list1, 5, 0);
	lv_obj_set_style_pad_left(list1, 0, 0);  // Quitar padding izquierdo
	lv_obj_set_style_pad_column(list1, 0, 0);
	lv_obj_set_style_margin_left(list1, 0, 0);

	/*Add buttons to the list*/
	lv_obj_t * btn;
	int i;
	for(i = 0; i < 15; i++) {
		btn = lv_button_create(list1);
		lv_obj_set_size(btn, 20, 20);

		//~ lv_obj_t * lab = lv_label_create(btn);
		//~ lv_label_set_text_fmt(lab, "%d", i);
	}

	lastItemButton = lv_button_create(list1);
	lv_obj_set_size(lastItemButton, 18, 18);
}

void GraphicLCD::createMainScreen() {
	this->logo = lv_img_create(lv_screen_active());
	lv_image_set_src(this->logo, &imageLogo);
	lv_obj_set_pos(this->logo, 0, 0);

	/*Animate in the content after the intro time*/
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
	lv_anim_set_var(&a, this->logo);
	lv_anim_set_duration(&a, 500);
	lv_anim_set_delay(&a, 4000);
	lv_anim_set_values(&a, LV_SCALE_NONE, 10);
	lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);
	lv_anim_start(&a);
}

void GraphicLCD::clearSplashScreen() {
	lv_obj_del(this->logo);
}

void GraphicLCD::setScreen(Screen newScreen){
	this->currentScreen = newScreen;
	this->setNewScreen();
}

void GraphicLCD::setNewScreen(){
	this->newScreen = true;
}

#ifndef USING_EMULATOR
void GraphicLCD::displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
	U8G2 *u8g2 = static_cast<U8G2 *>(lv_display_get_user_data(disp));

	// cleaning offseting with garbage
	px_map += 8;

	// Cantidad total de bytes (128 x 64 / 8 bits)
	constexpr int bufferSize = DSP_HOR_RES * DSP_VER_RES >> 3;

	// Crear buffer temporal con bits invertidos
	static uint8_t flippedBuffer[bufferSize];
	for (int i = 0; i < bufferSize; ++i) {
		flippedBuffer[i] = bitReverseTable[px_map[i]];
	}

	// Draw over display
	u8g2->clearBuffer(); // clear whole screen
	u8g2->drawXBMP(0, 0, DSP_HOR_RES, DSP_VER_RES, flippedBuffer);
	u8g2->sendBuffer();

	lv_display_flush_ready(disp);
}

uint32_t GraphicLCD::getTickCount() {
	return esp_timer_get_time() / 1000;
}
#endif // Using real hardware
