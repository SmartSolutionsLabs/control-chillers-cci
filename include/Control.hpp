#ifndef _INC_CONTROL_
#define _INC_CONTROL_

#include <Module.hpp>

#include "GraphicLcd.hpp"
#include "Chiller.hpp"
#include "Pump.hpp"


class Control : public Module {
	private:
		Chiller * chiller;

		Pump * pump;

		GraphicLCD *lcd;

		Screen newScreen = HOME;
	public:
		Control(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

		void handleKey(char key);

		void setGraphicLCD(GraphicLCD * newlcd){
			lcd = newlcd;
		};

};

#endif
