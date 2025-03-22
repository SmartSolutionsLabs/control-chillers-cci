#ifndef _INC_CHILLER_
#define _INC_CHILLER_

#include <Module.hpp>
#include <Wire.h>

class Chiller : public Module {
	private:
		TwoWire *wire;  // Referencia a la instancia de Wire
		uint8_t pin;
		bool state;
		uint8_t estadoGPIOA = 0x00;  

	public:
		Chiller();
		
		Chiller(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

		void writeRegister(uint8_t reg, uint8_t value);

		uint8_t readRegister(uint8_t reg) ;

		bool readPin(uint8_t pin);

		void writePin(uint8_t pin, bool state);

		void turnOn();

		void turnOff();

		void setPin(uint8_t newPin);
		uint8_t getPin(){
			return this->pin;
		}
		bool getState();
		void setState(bool newState);

		void toggle(uint8_t newRegister);

		void setGPIOA(uint8_t newState);

		void scanI2C();
};

#endif
