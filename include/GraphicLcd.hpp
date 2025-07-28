//george lo sugirio
#ifndef _INC_GRAPHICLCD_
#define _INC_GRAPHICLCD_

#include <lvgl.h>

#ifdef USING_EMULATOR
	#include <SDL2/SDL.h>
	#include "drivers/sdl/lv_sdl_mouse.h"
	#include "drivers/sdl/lv_sdl_mousewheel.h"
	#include "drivers/sdl/lv_sdl_keyboard.h"
#else // real hardware
	#include <U8g2lib.h>
#endif

enum Screen {
	HOME,       // Pantalla principal
	CONFIG,   // Configuración
	MANUAL,     // Estado del sistema
	LOG,    // Control manual
	ABOUT   // Información sobre el sistema
};

struct MenuOption {
	const char* name;
	const char* icon;
};

/**
 * Via to draw and control display.
 */
class GraphicLCD {
	private:
		lv_display_t *display;

		#ifdef USING_EMULATOR
			lv_indev_t* lvTouch;
			lv_indev_t* lvMouse;
			lv_indev_t* lvMouseWheel;
			lv_indev_t* lvKeyboard;
		#else
			U8G2_ST7920_128X64_F_SW_SPI* u8g2; //(U8G2_R0, 27, 26, 25, 14);
		#endif

		Screen currentScreen = HOME;

		bool isMenuActive = false;

		// all widgets for manipulation in real time. Don't forget allocate and free.
		lv_obj_t * logo;
		lv_obj_t * menuBox;

		lv_obj_t* titleBox;
		lv_obj_t* titleLabel;

		lv_obj_t* contentBox;

		lv_anim_t* menuItemPulse;

		static MenuOption menuOptions[];

		// Creation of contents
		void createHomePage();
		void createConfigPage();
		void createManualPage();
		void createLogPage();
		void createAboutPage();

		/**
		 * Starting or deleting animation checking if menu is active.
		 */
		void inline animateMenuItem(bool alreadyAnimated = false);

	public:
		static uint8_t bitReverseTable[256]; // but must be precomputed to make it constant

		GraphicLCD();

		~GraphicLCD();

		void init();

		void toggleNavigationMode();

		/**
		 * Create widgets for everything including splash.
		 */
		void createMainScreen();

		void createMenuBox();

		void createTitleBox();

		void createContentBox(int yOffset);

		/**
		 * Performs sequence to go up or down.
		 * @param next bool true for down.
		 */
		void moveNextContent(bool next);

		#ifdef USING_EMULATOR
			static void keyboardEventHandler(lv_event_t *e);
		#else
			static uint32_t getTickCount(void);
			static void displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
		#endif
	};

#endif
