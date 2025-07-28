#include "GraphicLcd.hpp"
#include "images/Logo.h"

LV_IMAGE_DECLARE(imageLogo);

MenuOption GraphicLCD::menuOptions[ABOUT + 1] = {
	{"DASHBOARD", LV_SYMBOL_HOME},
	{"SETUP", LV_SYMBOL_SETTINGS},
	{"HAND-OPERATED", LV_SYMBOL_PLAY},
	{"LOGS", LV_SYMBOL_SD_CARD},
	{"ABOUT", LV_SYMBOL_EYE_OPEN}
};

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

void GraphicLCD::toggleNavigationMode() {
	this->isMenuActive = !this->isMenuActive;

	// ---- Animar desplazamiento X del menú ----
	lv_anim_t a_menu;
	lv_anim_init(&a_menu);
	lv_anim_set_var(&a_menu, this->menuBox);
	lv_anim_set_values(&a_menu,
					   this->isMenuActive ? -10 : 0,
					   this->isMenuActive ? 0 : -10);
	lv_anim_set_time(&a_menu, 200);
	lv_anim_set_exec_cb(&a_menu, [](void* var, int32_t value) {
		lv_obj_set_x(static_cast<lv_obj_t*>(var), value);
	});
	lv_anim_start(&a_menu);

	// ---- Animar escala del contenido ----
	lv_anim_t a_scale;
	lv_anim_init(&a_scale);
	lv_anim_set_var(&a_scale, this->contentBox);
	lv_anim_set_values(&a_scale,
					   this->isMenuActive ? 255 : 230,  // de
					   this->isMenuActive ? 230 : 255); // a
	lv_anim_set_time(&a_scale, 200);
	lv_anim_set_exec_cb(&a_scale, [](void* var, int32_t value) {
		lv_obj_set_style_transform_scale(static_cast<lv_obj_t*>(var), value, 0);
	});
	lv_anim_start(&a_scale);

	// ---- Ajustar posición del contenido ----
	lv_anim_t a_content_x;
	lv_anim_init(&a_content_x);
	lv_anim_set_var(&a_content_x, this->contentBox);
	lv_anim_set_values(&a_content_x,
					   this->isMenuActive ? 10 : 20,  // de
					   this->isMenuActive ? 20 : 10); // a
	lv_anim_set_time(&a_content_x, 200);
	lv_anim_set_exec_cb(&a_content_x, [](void* var, int32_t value) {
		lv_obj_set_x(static_cast<lv_obj_t*>(var), value);
	});
	lv_anim_start(&a_content_x);
}

void GraphicLCD::createMenuBox() {
	this->menuBox = lv_obj_create(lv_screen_active());
	lv_obj_set_style_radius(this->menuBox, 0, 0); // Sin esquinas redondeadas
	lv_obj_set_style_bg_opa(this->menuBox, LV_OPA_TRANSP, 0); // Full transparency
	lv_obj_set_style_shadow_width(this->menuBox, 0, 0); // Sin sombra
	lv_obj_set_style_border_width(this->menuBox, 0, 0); // Sin borde
	lv_obj_set_style_pad_all(this->menuBox, 0, 0);
	lv_obj_set_size(this->menuBox, 20, DSP_VER_RES);  // 20px de ancho, 64px de alto
	lv_obj_align(this->menuBox, LV_ALIGN_TOP_LEFT, 0, 0);  // pegado a la izquierda
	lv_obj_set_x(this->menuBox, -10); // retroceder 7 px
	lv_obj_set_scrollbar_mode(this->menuBox, LV_SCROLLBAR_MODE_OFF); // hide scroll bar
	lv_obj_set_scroll_snap_y(this->menuBox, LV_SCROLL_SNAP_CENTER);  // snap en Y
	lv_obj_add_flag(this->menuBox, LV_OBJ_FLAG_SCROLL_ONE);  // passing each one
	lv_obj_set_flex_flow(this->menuBox, LV_FLEX_FLOW_COLUMN);         // apilado vertical

	for (unsigned int i = 0; i < (ABOUT + 1); ++i) {
		// Crear contenedor individual por ítem del menú
		lv_obj_t* itemBox = lv_obj_create(this->menuBox);
		lv_obj_set_size(itemBox, lv_pct(100), 20); // ancho = 100% del panel (20px), alto = 20px
		lv_obj_set_style_bg_opa(itemBox, LV_OPA_TRANSP, 0);
		lv_obj_set_style_border_width(itemBox, 0, 0);
		lv_obj_set_style_pad_all(itemBox, 0, 0);
		lv_obj_clear_flag(itemBox, LV_OBJ_FLAG_SCROLLABLE); // no scroll interno

		// Crear la etiqueta del ícono dentro del contenedor
		lv_obj_t* label = lv_label_create(itemBox);
		lv_label_set_text(label, GraphicLCD::menuOptions[i].icon);
		lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
		lv_obj_set_style_text_color(label, lv_color_black(), 0);
		lv_obj_center(label);
	}

	lv_obj_update_snap(this->menuBox, LV_ANIM_ON);
	lv_obj_scroll_to_view(lv_obj_get_child(this->menuBox, this->currentScreen), LV_ANIM_OFF); // center first element vertically
}

void GraphicLCD::createTitleBox() {
	this->titleBox = lv_obj_create(lv_screen_active());

	// Ancho: lo que sobra después del menú lateral (20 px)
	lv_obj_set_size(this->titleBox, LV_HOR_RES - 18, 12);

	// Alinear al tope derecho
	lv_obj_align(this->titleBox, LV_ALIGN_TOP_RIGHT, 0, 0);

	// Eliminar bordes, sombra, redondeado
	lv_obj_set_style_radius(this->titleBox, 0, 0);
	lv_obj_set_style_shadow_width(this->titleBox, 0, 0);
	lv_obj_set_style_border_width(this->titleBox, 0, 0);
	lv_obj_set_style_pad_all(this->titleBox, 0, 0);

	// Fondo opaco
	lv_obj_set_style_bg_opa(this->titleBox, LV_OPA_COVER, 0);

	// Dibujar una línea inferior de 1 px (borde inferior)
	//~ lv_obj_set_style_border_side(this->titleBox, LV_BORDER_SIDE_BOTTOM, 0);
	//~ lv_obj_set_style_border_width(this->titleBox, 1, 0);
	//~ lv_obj_set_style_border_color(this->titleBox, lv_color_black(), 0);

	// No scroll
	lv_obj_clear_flag(this->titleBox, LV_OBJ_FLAG_SCROLLABLE);

	// Etiqueta centrada vertical y alineada izquierda
	this->titleLabel = lv_label_create(this->titleBox);
	lv_label_set_text(this->titleLabel, GraphicLCD::menuOptions[this->currentScreen].name);
	lv_obj_set_style_text_font(this->titleLabel, &lv_font_montserrat_8, 0);
	lv_obj_align(this->titleLabel, LV_ALIGN_CENTER, 0, 0);
}

void GraphicLCD::createMainScreen() {
	lv_obj_t* screen_active = lv_screen_active();
	// Disabling scroll because we will be creating elements out of borders
	lv_obj_clear_flag(screen_active, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_clear_flag(screen_active, LV_OBJ_FLAG_SCROLL_ELASTIC);
	lv_obj_clear_flag(screen_active, LV_OBJ_FLAG_SCROLL_MOMENTUM);
	lv_obj_clear_flag(screen_active, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
	lv_obj_clear_flag(screen_active, LV_OBJ_FLAG_SCROLL_CHAIN_VER);
	lv_obj_set_scrollbar_mode(screen_active, LV_SCROLLBAR_MODE_OFF);

	this->createMenuBox();

	this->createTitleBox();

	this->createContentBox(0); // craeting without offset, I mean centered

	this->logo = lv_img_create(screen_active);
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

void GraphicLCD::createContentBox(int yOffset) {
	// Crear el contenedor principal del contenido
	this->contentBox = lv_obj_create(lv_screen_active());

	// Tamaño: ocupar el resto del área (ancho - 10px, alto - 12px)
	lv_coord_t contentWidth = DSP_HOR_RES - 10; // -10 pixels because menu of 20 px is half hidden
	lv_coord_t contentHeight = DSP_VER_RES - 12;

	lv_obj_set_size(this->contentBox, contentWidth, contentHeight);

	// Posicionar considerando desplazamiento vertical
	// x 20 or 10, y always 12 + yOffset
	if (this->isMenuActive) {
		lv_obj_set_pos(this->contentBox,
						20,
						12 + yOffset);
		// Escalar contenido al 90%
		lv_obj_set_style_transform_scale(this->contentBox, 230 , 0); // escala 255 * 90%
	}
	else {
		lv_obj_set_pos(this->contentBox, 10, 12 + yOffset);
	}

	// Estilo: sin borde, sin sombra, sin radio
	lv_obj_set_style_radius(this->contentBox, 0, 0);
	lv_obj_set_style_border_width(this->contentBox, 0, 0);
	lv_obj_set_style_shadow_width(this->contentBox, 0, 0);
	lv_obj_set_style_pad_all(this->contentBox, 0, 0);

	// Crear contenido según pantalla actual
	switch (this->currentScreen) {
		case HOME:
			this->createHomePage();
			break;
		case CONFIG:
			this->createConfigPage();
			break;
		case MANUAL:
			this->createManualPage();
			break;
		case LOG:
			this->createLogPage();
			break;
		case ABOUT:
			this->createAboutPage();
			break;
	}
}

void GraphicLCD::createHomePage() {
	// Eliminar el scroll si no es necesario aún
	lv_obj_clear_flag(this->contentBox, LV_OBJ_FLAG_SCROLLABLE);

	// Crear una etiqueta centrada en el contenedor
	lv_obj_t * label = lv_label_create(this->contentBox);
	lv_label_set_text(label, "Home Content");
	lv_obj_center(label);
}

void GraphicLCD::createConfigPage() {
	// Crear una etiqueta centrada en el contenedor
	lv_obj_t * label = lv_label_create(this->contentBox);
	lv_label_set_text(label, "Config Content");
	lv_obj_center(label);
}

void GraphicLCD::createManualPage() {
	// Crear una etiqueta centrada en el contenedor
	lv_obj_t * label = lv_label_create(this->contentBox);
	lv_label_set_text(label, "Manual Content");
	lv_obj_center(label);
}

void GraphicLCD::createLogPage() {
	// Crear una etiqueta centrada en el contenedor
	lv_obj_t * label = lv_label_create(this->contentBox);
	lv_label_set_text(label, "Log Content");
	lv_obj_center(label);
}

void GraphicLCD::createAboutPage() {
	// Hacer que contentBox sea scrollable verticalmente
	lv_obj_set_scroll_dir(this->contentBox, LV_DIR_VER);
	lv_obj_set_scrollbar_mode(this->contentBox, LV_SCROLLBAR_MODE_AUTO);
	lv_obj_set_style_pad_all(this->contentBox, 1, 0);  // padding interno opcional

	// Crear etiqueta directamente en contentBox
	lv_obj_t* label = lv_label_create(this->contentBox);
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_label_set_text(label,
		"(c) Smart Solutions Labs.\n"
		"Creator/Designer:\n"
		"    Miguel Quispe\n"
		"Additional Engineering:\n"
		"    George Garro\n\n"
		"Sigue en contacto para recibir mejoras de este dispositivo.\n\n"
		"Este texto adicional asegura que el contenido supere la altura de la pantalla y active el scroll automático.\n"
		"Gracias por tu interés."
	);

	// Ajustes de estilo
	//~ lv_obj_set_width(label, lv_obj_get_width(this->contentBox) - 8);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_8, 0);
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_obj_set_style_pad_all(label, 0, 0);
	lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
}

void GraphicLCD::moveNextContent(bool next) {
	if (!this->isMenuActive) {
		// don't change content if we are not navigating through menu
		return;
	}

	// flag to decide creation of new content
	bool notMoved = true;

	if (next) {
		if (this->currentScreen < ABOUT) {  // Within range
			this->currentScreen = static_cast<Screen>(this->currentScreen + 1);

			notMoved = false;
		}
	}
	else {
		if (this->currentScreen > HOME) {
			this->currentScreen = static_cast<Screen>(this->currentScreen - 1);
			notMoved = false;
		}
	}

	if (notMoved) {
		// nothing else to make
		return;
	}

	// Update title and position of menu item
	lv_obj_scroll_to_view(lv_obj_get_child(this->menuBox, this->currentScreen), LV_ANIM_ON);
	lv_label_set_text(this->titleLabel, GraphicLCD::menuOptions[this->currentScreen].name);

	// Guardar contentBox anterior
	lv_obj_t* oldContent = this->contentBox;

	// Calcular desplazamiento
	int yOffset = lv_obj_get_height(oldContent);

	// Crear el nuevo contentBox ya desplazado fuera de vista
	this->createContentBox(next ? yOffset : -yOffset);

	// Asegurarse que la barra de título quede por encima
	lv_obj_move_foreground(this->titleBox);

	// Animar el nuevo contentBox para deslizarlo a su posición
	lv_anim_t a_new;
	lv_anim_init(&a_new);
	lv_anim_set_var(&a_new, this->contentBox);
	lv_anim_set_exec_cb(&a_new, [](void* obj, int32_t v) {
		lv_obj_set_y(static_cast<lv_obj_t*>(obj), v);
	});
	lv_anim_set_values(&a_new, next ? yOffset : -yOffset, 12);  // 12 es la posición final visible
	lv_anim_set_time(&a_new, 400); // same for old
	lv_anim_set_path_cb(&a_new, lv_anim_path_ease_out);
	lv_anim_start(&a_new);

	// Animar y eliminar el antiguo contentBox
	lv_anim_t a_old;
	lv_anim_init(&a_old);
	lv_anim_set_var(&a_old, oldContent);
	lv_anim_set_exec_cb(&a_old, [](void* obj, int32_t v) {
		lv_obj_set_y(static_cast<lv_obj_t*>(obj), v);
	});
	lv_anim_set_values(&a_old, 12, next ? -yOffset : yOffset);
	lv_anim_set_time(&a_old, 400);
	lv_anim_set_path_cb(&a_old, lv_anim_path_ease_in);
	lv_anim_set_deleted_cb(&a_old, lv_obj_delete_anim_completed_cb);
	lv_anim_start(&a_old);
}

#ifdef USING_EMULATOR
void GraphicLCD::keyboardEventHandler(lv_event_t *e) {
	uint32_t key = lv_event_get_key(e);
	GraphicLCD* lcd = static_cast<GraphicLCD*>(lv_event_get_user_data(e));

	switch (key) {
		case LV_KEY_UP:
			lcd->moveNextContent(false);
			break;
		case LV_KEY_DOWN:
			lcd->moveNextContent(true);
			break;
		case LV_KEY_ENTER:
			lcd->toggleNavigationMode();
			break;
		case LV_KEY_ESC:
			lcd->toggleNavigationMode();
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
