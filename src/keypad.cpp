#include "Keypad.hpp"

Keypad::Keypad(const char * name, int taskCore) : Module(name, taskCore) {
    Serial.println("keypad constructor");
}

void Keypad::connect(void * data) {
    pcf = new PCF8574(0x27,static_cast<TwoWire*>(data));
    pcf->begin();
    // Configurar pines del PCF8574
    for (uint8_t i = 0; i < 4; i++) {
        pcf->write(rowPins[i], OUTPUT);
        pcf->write(colPins[i], INPUT_PULLUP);
    }
}

void Keypad::run(void* data) {
	this->iterationDelay = 1 / portTICK_PERIOD_MS;

	while (1) {
		vTaskDelay(this->iterationDelay);
        unsigned long currentMillis = millis();  // Obtener el tiempo actual

        // Solo ejecuta el escaneo cada SCAN_INTERVAL milisegundos
        if (currentMillis - lastScanTime >= SCAN_INTERVAL) {
            lastScanTime = currentMillis;  // Actualizar tiempo

            char key = this->getKey();  // Escanear el teclado

            if (key != 0) {
                Serial.println("key pressed");
                control->handleKey(key);
            }
        }
	}
}


char Keypad::getKey() {
    for (uint8_t row = 0; row < 4; row++) {
        // Activar una fila en LOW
        for (uint8_t i = 0; i < 4; i++) {
            pcf->write(rowPins[i], HIGH);
        }
        pcf->write(rowPins[row], LOW);

        // Leer columnas
        for (uint8_t col = 0; col < 4; col++) {
            if (pcf->read(colPins[col]) == LOW) {
                while (pcf->read(colPins[col]) == LOW); // Esperar soltar tecla
                return keys[row][col]; // Retornar tecla presionada
            }
        }
    }
    return '\0'; // No hay tecla presionada
}