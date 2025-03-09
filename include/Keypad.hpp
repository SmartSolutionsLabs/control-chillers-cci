#ifndef _INC_KEYPAD_
#define _INC_KEYPAD_
#include <Module.hpp>
#include <Wire.h>
#include <Adafruit_PCF8574.h>
#include "Control.hpp"

#define PCF8574_ADDR 0x27   // Dirección I2C del PCF8574
#define SCAN_INTERVAL 50    // Intervalo de escaneo en milisegundos

class Keypad : public Module {
	private:
		Control * control;
        // Mapeo de teclas del teclado 4x4
        char keymap[4][4] = {
            {'1', '2', '3', 'A'},
            {'4', '5', '6', 'B'},
            {'7', '8', '9', 'C'},
            {'*', '0', '#', 'D'}
        };
        // Definir pines del teclado (Filas = Salidas, Columnas = Entradas)
        const uint8_t rowPins[4] = {7, 6, 5, 4};  // P0-P3
        const uint8_t colPins[4] = {3, 2, 1, 0};  // P4-P7

        Adafruit_PCF8574 pcf8574;
        unsigned long lastScanTime = 0;  // Variable para el control de tiempo
	public:
		Keypad(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

        char scanKeypad();

        void setControl(Control * newControl){
            control = newControl;
        };
};

#endif