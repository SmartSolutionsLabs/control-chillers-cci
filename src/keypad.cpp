#include "Keypad.hpp"

Keypad::Keypad(const char * name, int taskCore) : Module(name, taskCore) {
    Serial.println("keypad constructor");
}

void Keypad::connect(void * data) {
    Serial.println("try Begin Keypad.");
    Wire.begin();
    Serial.println("tried Begin Keypad.");
    if (!pcf8574.begin(PCF8574_ADDR, &Wire)) {
        Serial.println("Error: No se encontró el PCF8574.");
    }

    // Configurar filas como salidas y ponerlas en HIGH
    for (uint8_t i = 0; i < 4; i++) {
        pcf8574.pinMode(rowPins[i], OUTPUT);
        pcf8574.digitalWrite(rowPins[i], HIGH);
    }

    // Configurar columnas como entradas con pull-up
    for (uint8_t i = 0; i < 4; i++) {
        pcf8574.pinMode(colPins[i], INPUT_PULLUP);
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

            char key = this->scanKeypad();  // Escanear el teclado

            if (key != 0) {
                Serial.println("key pressed");
                control->handleKey(key);
            }
        }
	}
}


// Función para escanear el teclado 4x4
char Keypad::scanKeypad() {
    for (uint8_t row = 0; row < 4; row++) {
        this->pcf8574.digitalWrite(this->rowPins[row], LOW);  // Activar fila

        for (uint8_t col = 0; col < 4; col++) {
            if (this->pcf8574.digitalRead(this->colPins[col]) == LOW) {  // Si se detecta una tecla presionada
                while (pcf8574.digitalRead(this->colPins[col]) == LOW);  // Esperar a que se suelte la tecla
                pcf8574.digitalWrite(this->rowPins[row], HIGH);  // Restaurar la fila a HIGH
                return keymap[row][col];  // Retornar la tecla detectada
            }
        }

        this->pcf8574.digitalWrite(this->rowPins[row], HIGH);  // Restaurar la fila
    }
    return 0;  // No se presionó ninguna tecla
}
