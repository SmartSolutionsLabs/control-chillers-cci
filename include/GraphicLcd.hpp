//george lo sugirio
#ifndef _INC_GRAPHICLCD_
#define _INC_GRAPHICLCD_
#include <Module.hpp>
#include <U8g2lib.h>
#include "Screens.hpp"

class GraphicLCD : public Module {
	private:
        U8G2_ST7920_128X64_F_SW_SPI *u8g2; //(U8G2_R0, 27, 26, 25, 14);
	public:
        GraphicLCD(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

		void print(int x, int y,  char *text);

		void testprint();

		void splashScreen();

		void drawImage(int xPos, int yPos, const Bitmap &image);
};

#endif