//george lo sugirio
#ifndef _INC_GRAPHICLCD_
#define _INC_GRAPHICLCD_
#include <Module.hpp>
#include <U8g2lib.h>
#include "Screens.hpp"
#include "MotorGraphicLcd.hpp"
#include "ChillerGraphicLcd.hpp"
#include "progressBarLCD.hpp"
#include "textInputLCD.hpp"

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

		ChillerGraphicLCD *chillerIcon;

		uint16_t splashScreenTimer = 2500;

		uint32_t screenTimer;
		
		Screen currentScreen = HOME; // memoria cache

		bool newScreen;

		uint32_t timerFPS ;

		bool initialized = false;
	public:
        GraphicLCD(const char * name, int taskCore = 1);
		
		~GraphicLCD() {
            delete[] progressBar;  // Liberar la memoria del arreglo
        }
    
        // Método para obtener una referencia a un progressBarLCD
        progressBarLCD& getProgressBar(uint8_t index) {
            if (index < 2) {  // Asegurarse de que el índice esté dentro del rango
                return progressBar[index];
            }
            // Manejar el caso de índice inválido (opcional)
            static progressBarLCD dummy;  // Objeto dummy para evitar errores
            return dummy;
        }
		
		textInputLCD* getTextInput(uint8_t index) ;

		void connect(void * data) override;

		void run(void* data) override;

		void print(int x, int y,  char *text);

		void splashScreen();

		void drawImage(int xPos, int yPos, const Bitmap &image);

		void drawMenu();

		void setMotorState(int motorNumber , bool state);

		void setChillerState(int chillerNumber , bool state);

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

		void drawBoxes();

		void textInput(uint8_t posX , uint8_t posY, uint8_t size, char *text);

		void drawCenteredText(int xCenter, int y, const char *text);

		void setNewScreen();

		void setProgressBarDelay(uint8_t index , uint8_t newDelay);
		
		uint8_t getProgressBarDelay(uint8_t index);
		

		bool isInitialized() const { return initialized; }
		void setInitialized(bool value) { initialized = value; }

		void selectMotor(int index,bool mode);
		void selectChiller(int index,bool mode);
	};

#endif