//george lo sugirio
#ifndef _INC_GRAPHICLCD_
#define _INC_GRAPHICLCD_

#include <U8g2lib.h>
#include <lvgl.h>

enum Screen {
	HOME,       // Pantalla principal
	CONFIG,   // Configuración
	MANUAL,     // Estado del sistema
	LOG,    // Control manual
	SCREEN_ABOUT       // Información sobre el sistema
};

/**
 * Via to draw and control display.
 */
class GraphicLCD {
	private:
        U8G2_ST7920_128X64_F_SW_SPI *u8g2; //(U8G2_R0, 27, 26, 25, 14);

		lv_display_t *display;

		uint16_t splashScreenTime = 2500;

		uint32_t screenTimer;

		Screen currentScreen = HOME; // memoria cache

		bool newScreen;

		uint32_t timerFPS ;

		bool initialized = false;

		// all widgets for manipulation in real time. Don't forget allocate and free.
		lv_obj_t * splashScreen;
		lv_obj_t * lastItemButton;

	public:
		lv_obj_t * list1;
		static uint8_t bitReverseTable[256]; // but must be precomputed to make it constant

		GraphicLCD();

		~GraphicLCD();

		static uint32_t getTickCount(void);

		void init();

		/**
		 * Create widgets of splash screen and show them.
		 */
		void initSplashScreen();

		/**
		 * Free all widgets of splash screen.
		 */
		void clearSplashScreen();

		void initMenu();
		void clearMenu();

		void initHomePage();
		void clearHomePage();

		void initConfigPage();
		void clearConfigPage();

		void initManualPage();
		void clearManualPage();

		void initLogPage();
		void clearLogPage();

		void update();

		void setScreen(Screen newScreen);

		void setNewScreen();

		bool isInitialized() const { return initialized; }
		void setInitialized(bool value) { initialized = value; }

		uint16_t inline getSplashScreenTime() {
			return splashScreenTime;
		}

		inline void clearSplashScreenTime() {
			splashScreenTime = 0;
		}

		uint32_t inline getScreenTimer() {
			return screenTimer;
		}

		inline void setScreenTimer(uint32_t newTimer) {
			screenTimer = newTimer;
		}

		static void displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
	};

#endif
