//george lo sugirio
#ifndef _INC_GRAPHICLCD_
#define _INC_GRAPHICLCD_
#include <Module.hpp>
#include <U8g2lib.h>
#include "Screens.hpp"
#include "MotorGraphicLcd.hpp"
#include "progressBarLCD.hpp"

struct ChillerIcon {
    uint8_t number;
    bool state;
	bool run;
	uint32_t updateTimer;
	uint32_t timer;
};

struct ProgressBarIcon{
	uint16_t value;
	uint8_t percentage;
};

enum Screen {
	HOME,       // Pantalla principal
	CONFIG,   // Configuración
	MANUAL,     // Estado del sistema
	LOG,    // Control manual
	SCREEN_ABOUT       // Información sobre el sistema
};

class GraphicLCD : public Module {
	private:
        U8G2_ST7920_128X64_F_SW_SPI *u8g2; //(U8G2_R0, 27, 26, 25, 14);

		MotorGraphicLCD *motorIcon;

		progressBarLCD *progressBar;

		ChillerIcon chillerIcon[2];

		uint16_t splashScreenTimer = 2500;

		uint32_t screenTimer;
		
		Screen currentScreen = HOME;

		bool newScreen;
	public:
        GraphicLCD(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

		void print(int x, int y,  char *text);

		void testprint();

		void splashScreen();

		void drawImage(int xPos, int yPos, const Bitmap &image);

		void drawPage(uint8_t page);

		void setMotorState(uint8_t motorNumber , bool state);

		void setChillerState(uint8_t chillerNumber , bool state);

		void drawHomePage();

		void drawConfigPage();

		void drawManualPage();

		void drawLogPage();

		void update();

		void drawRotatedImage(int xPos, int yPos, const Bitmap &image, float angle);

		void setScreen(Screen newScreen);

		void setProgressBarValue(uint8_t index ,uint16_t newValue);

		void setProgressBarPercentage(uint8_t index ,uint8_t newPercentage);

		void drawProgressBar(uint8_t number , uint8_t posX , uint8_t posY, const Bitmap &image);

		void nextScreen();

		void previousScreen();

		Screen getScreen();

		void drawBoxes();

		void textInput(uint8_t posX , uint8_t posY, uint8_t size, char *text);

		void drawCenteredText(int xCenter, int y, const char *text);

		void setNewScreen();
	};

#endif