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

	// Listener of keyboard

	// Crear un grupo y asignar la pantalla activa
	lv_group_t* group = lv_group_create();
	lv_group_add_obj(group, lv_scr_act());

	// Asignar grupo al input device (teclado)
	lv_indev_set_group(this->lvKeyboard, group);

	// Enfocar pantalla principal
	lv_group_focus_obj(lv_scr_act());

	// Agregar callback de evento de teclado
	lv_obj_add_event_cb(lv_scr_act(), GraphicLCD::keyboardEventHandler, LV_EVENT_KEY, this);
#endif
}

void GraphicLCD::initMenu() {
	this->menuBox = lv_obj_create(lv_screen_active());
	lv_obj_set_style_pad_all(this->menuBox, 0, 0);
	lv_obj_set_size(this->menuBox, 20, DSP_VER_RES);  // 20px de ancho, 64px de alto
	lv_obj_align(this->menuBox, LV_ALIGN_TOP_LEFT, 0, 0);  // pegado a la izquierda
	lv_obj_set_scrollbar_mode(this->menuBox, LV_SCROLLBAR_MODE_OFF); // hide scroll bar
	lv_obj_set_scroll_snap_y(this->menuBox, LV_SCROLL_SNAP_CENTER);  // snap en Y
	lv_obj_add_flag(this->menuBox, LV_OBJ_FLAG_SCROLL_ONE);  // passing each one
	lv_obj_set_flex_flow(this->menuBox, LV_FLEX_FLOW_COLUMN);         // apilado vertical

	for (uint32_t i = 0; i < 8; i++) {  // puedes variar la cantidad según el tamaño del botón
		lv_obj_t * btn = lv_button_create(this->menuBox);
		lv_obj_set_size(btn, lv_pct(100), 20);  // ancho = 100% del panel (20px), alto = 20px

		// Opcionalmente hacer los botones más compactos visualmente
		lv_obj_set_style_pad_all(btn, 0, 0);  // sin relleno

		lv_obj_t * label = lv_label_create(btn);
		lv_label_set_text_fmt(label, "%" LV_PRIu32, i + 1);
		lv_obj_center(label);
	}

	lv_obj_update_snap(this->menuBox, LV_ANIM_ON);
	lv_obj_scroll_to_view(lv_obj_get_child(this->menuBox, 0), LV_ANIM_OFF); // center first element vertically
}

void GraphicLCD::createMainScreen() {
	this->initMenu();

	this->logo = lv_img_create(lv_screen_active());
	lv_image_set_src(this->logo, &imageLogo);
	lv_obj_set_pos(this->logo, 0, 0);

	/*Animate in the content after the intro time*/
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
	lv_anim_set_var(&a, this->logo);
	lv_anim_set_duration(&a, 2000);
	lv_anim_set_delay(&a, 1000);
	lv_anim_set_values(&a, LV_SCALE_NONE, 10);
	lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);
	lv_anim_set_exec_cb(&a, [](void *obj, int32_t v) {
		lv_image_set_scale((lv_obj_t *)obj, v);
	});
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

#ifdef USING_EMULATOR
void GraphicLCD::keyboardEventHandler(lv_event_t *e) {
	uint32_t key = lv_event_get_key(e);
	GraphicLCD* lcd = static_cast<GraphicLCD*>(lv_event_get_user_data(e));

	switch (key) {
		case LV_KEY_UP:
			lv_obj_scroll_to_view(lv_obj_get_child(lcd->menuBox, 2), LV_ANIM_ON);
			break;
		case LV_KEY_DOWN:
			lv_obj_scroll_to_view(lv_obj_get_child(lcd->menuBox, 4), LV_ANIM_ON);
			break;
		case LV_KEY_ENTER:
			break;
		default:
			break;
	}
}

#else
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
