#ifndef _INC_PUMP_
#define _INC_PUMP_

#include <Module.hpp>
#include <Wire.h>

class Pump : public Module {
	private:
		TwoWire *wire;  // Referencia a la instancia de Wire
		uint8_t pin;
		bool state;

	public:
		Pump();

        Pump(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

		void writeRegister(uint8_t reg, uint8_t value);

		uint8_t readRegister(uint8_t reg) ;

		bool readPin(uint8_t pin);

		void writePin(uint8_t pin, bool state);

		void turnOn();

		void turnOff();

		void setPin(uint8_t newPin);

		bool getState();
};

#endif
