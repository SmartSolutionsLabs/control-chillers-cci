#ifndef USING_EMULATOR

#include "Keypad.hpp"
#include <Wire.h>

// Variable estática para manejar la instancia de Keypad en las interrupciones
static Keypad* keypadInstance = nullptr;

// Variable para almacenar el estado del pulsador
volatile bool interruptFlag = false;  // Bandera de interrupción

Keypad::Keypad() : lastState(0xFF), currentState(0xFF) {
	keypadInstance = this;  // Asignar la instancia actual a la variable estática
}

void Keypad::writeRegister(uint8_t reg, uint8_t value) {
	Wire.beginTransmission(MCP23017_ADDR);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

uint8_t Keypad::readRegister(uint8_t reg) {
	Wire.beginTransmission(MCP23017_ADDR);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(MCP23017_ADDR, 1);
	return Wire.available() ? Wire.read() : 0;
}

void Keypad::init() {
	// Habilitar el MCP23017 (pines 15 y 41 como salida)
	pinMode(15, OUTPUT);
	pinMode(41, OUTPUT);
	digitalWrite(15, HIGH);  // Activar el MCP23017
	digitalWrite(41, HIGH);

	writeRegister(IODIRB, 0xFF);   // Todos como entradas
	writeRegister(GPPUB, 0xFF);    // Habilitar resistencias pull-up
}

bool Keypad::isPressed(uint8_t i) {
	if (i < 8) {
		return this->debouncedStates[i];
	}

	return false;
}

void Keypad::update() {
	this->currentState = this->readRegister(GPIOB);

	unsigned long now = millis();

	for (int i = 0; i < 8; ++i) {
		bool pinNow = (this->currentState >> i) & 1;
		bool pinLast = (this->lastState >> i) & 1;

		if (pinNow != pinLast) {
			this->lastDebounceTimes[i] = now;
		}

		if ((now - this->lastDebounceTimes[i]) > this->debounceDelay) {
			this->debouncedStates[i] = (pinNow == 0);  // Pulled LOW cuando se presiona
		}
	}

	this->lastState = this->currentState;
}

// ISR para el bloque A
void IRAM_ATTR Keypad::handleInterruptA() {
	// Este método maneja las interrupciones del bloque A (pines 0-7)
	// No se usa en este caso, pero se deja implementado por si es necesario en el futuro.
}

// ISR para el bloque B
void IRAM_ATTR Keypad::handleInterruptB() {
	// Establecer la bandera de interrupción
	this->interruptFlag = true;
}

#endif // ifndef USING_EMULATOR
