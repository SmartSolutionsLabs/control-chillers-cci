#include "Keypad.hpp"
#include <Wire.h>

// Variable estática para manejar la instancia de Keypad en las interrupciones
static Keypad* keypadInstance = nullptr;

// Variable para almacenar el estado del pulsador
volatile bool interruptFlag = false;  // Bandera de interrupción

Keypad::Keypad(const char * name, int taskCore) : Module(name, taskCore) {
    Serial.println("Keypad constructor");
    keypadInstance = this;  // Asignar la instancia actual a la variable estática
    this->control = nullptr;  // Inicializar el controlador
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

void Keypad::connect(void * data) {
    this->timerAutomatic = millis();
    
    // Habilitar el MCP23017 (pines 15 y 41 como salida)
    pinMode(15, OUTPUT);
    pinMode(41, OUTPUT);
    digitalWrite(15, HIGH);  // Activar el MCP23017
    digitalWrite(41, HIGH);

    writeRegister(IODIRB, 0xFF);   // Todos como entradas
    writeRegister(GPPUB, 0xFF);    // Habilitar resistencias pull-up
}

void Keypad::run(void* data) {
    this->iterationDelay = 10 / portTICK_PERIOD_MS;  // Polling cada 10ms
    
    Serial.println("Keypad task started (Polling mode)...");
    uint8_t lastGPIOB = readRegister(GPIOB);  // Estado inicial
    
    while (1) {
        uint8_t currentGPIOB = readRegister(GPIOB);  // Leer estado actual
        
        // Debug: Mostrar estado binario de los pines (opcional)
        // Serial.print("GPIOB: 0b");
        // Serial.println(currentGPIOB, BIN);  // Muestra 6 bits (0-5)

        // Solo procesar si hay cambios
        if (currentGPIOB != lastGPIOB) {
            for (int i = 0; i < 8; i++) {  // Procesar 6 botones (bits 0-5)
                bool lastState = (lastGPIOB >> i) & 1;  // Estado anterior
                bool currentState = (currentGPIOB >> i) & 1;  // Estado actual
                
                // Determinar qué tecla corresponde a este bit
                char key;
                key = 'A' + i;  // Bits 0-3: A, B, C, D
                
                // Detectar FALLING (botón presionado)
                if (lastState && !currentState) {
                    Serial.print("PRESSED: ");
                    Serial.println(key);
                    if (control) control->handleKey(key);
                }
                
                // Detectar RISING (botón liberado)
                else if (!lastState && currentState) {
                    char releasedKey = key + ('J' - 'A');  // A→G, B→H, etc.
                    Serial.print("RELEASED: ");
                    Serial.println(releasedKey);
                    if (control) control->handleKey(releasedKey);
                }
            }
            lastGPIOB = currentGPIOB;  // Actualizar estado anterior
        }
        vTaskDelay(this->iterationDelay);
    }
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