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
    // Habilitar el MCP23017 (pin 15 como salida)
    pinMode(15, OUTPUT);
    pinMode(41, OUTPUT);
    digitalWrite(15, HIGH);  // Activar el pin ENABLE del MCP23017
    digitalWrite(41, HIGH);  // Activar el pin ENABLE del MCP23017

    // Configurar los pines de los botones como entradas (banco B)
    writeRegister(IODIRB, 0xFF);  // Todos los pines del banco B como entradas

    // Habilitar resistencias pull-up para los pines de los botones (banco B)
    writeRegister(GPPUB, 0xFF);  // Habilitar pull-up en todos los pines del banco B

    // Habilitar interrupciones solo en los pines 8, 9, 10 y 11 (banco B)
    writeRegister(GPINTENB, 0x0F);  // Habilitar interrupciones en los pines 8, 9, 10 y 11 (bits 0-3)

    // Configurar interrupciones por cambio de estado (FALLING)
    writeRegister(INTCONB, 0x00);   // Interrupciones por cambio de estado
    writeRegister(DEFVALB, 0xFF);   // Valor por defecto HIGH (para detectar FALLING)

    // Configurar los pines de interrupción del MCP23017
    pinMode(6, INPUT_PULLUP);  // Pin de interrupción del bloque A (INTA)
    pinMode(7, INPUT_PULLUP);  // Pin de interrupción del bloque B (INTB)

    // Asignar manejadores de interrupción
    attachInterrupt(digitalPinToInterrupt(6), []() { keypadInstance->handleInterruptA(); }, FALLING);
    attachInterrupt(digitalPinToInterrupt(7), []() { keypadInstance->handleInterruptB(); }, FALLING);

    // Depuración: Leer y mostrar los registros del MCP23017
    Serial.println("Registros del MCP23017:");
    Serial.printf("IODIRB: 0x%02X\n", readRegister(IODIRB));
    Serial.printf("GPPUB: 0x%02X\n", readRegister(GPPUB));
    Serial.printf("GPINTENB: 0x%02X\n", readRegister(GPINTENB));
    Serial.printf("INTCONB: 0x%02X\n", readRegister(INTCONB));
    Serial.printf("DEFVALB: 0x%02X\n", readRegister(DEFVALB));
    Serial.printf("GPIOB: 0x%02X\n", readRegister(GPIOB));  // Estado actual de los pines del banco B

    Serial.println("Keypad configurado correctamente.");
}

void Keypad::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;

    while (1) {
        if (interruptFlag) {
            // Limpiar la bandera de interrupción
            interruptFlag = false;

            // Leer el registro de captura de interrupción del bloque B
            uint8_t intCap = readRegister(INTCAPB);

            // Verificar qué botón se presionó (pines 8, 9, 10 y 11)
            for (uint8_t i = 0; i < 4; i++) {
                if (!(intCap & (1 << i))) {  // Si el botón está presionado (LOW)
                    lastKeyPressed = 'A' + i;  // Asignar una tecla a cada botón (A, B, C, D)
                    keyPressed = true;
                    lastKeyPressTime = millis();  // Registrar el tiempo de la primera pulsación
                    if (control != nullptr) {
                        control->handleKey(lastKeyPressed);  // Enviar la primera pulsación
                    }
                    break;  // Solo procesar un pulsador a la vez
                }
            }
        }

        if (keyPressed) {
            // Leer el estado actual de los pines del banco B
            uint8_t gpioB = readRegister(GPIOB);

            // Verificar si el pulsador sigue presionado
            bool keyStillPressed = false;
            for (uint8_t i = 0; i < 4; i++) {
                if (!(gpioB & (1 << i))) {  // Si el botón está presionado (LOW)
                    keyStillPressed = true;
                    break;
                }
            }

            if (keyStillPressed) {
                // Si el pulsador sigue presionado, verificar si ha pasado el retardo inicial
                if (millis() - lastKeyPressTime >= initialDelay) {
                    // Enviar pulsaciones repetidas cada 100 ms
                    if (millis() - lastKeyPressTime >= repeatInterval) {
                        lastKeyPressTime = millis();
                        if (control != nullptr) {
                            control->handleKey(lastKeyPressed);  // Enviar una pulsación repetida
                            Serial.printf("Pulsación repetida: %c, Tiempo: %lu ms\n", lastKeyPressed, millis());
                        }
                    }
                }
            } else {
                // Si el pulsador se ha soltado, liberar la tecla
                keyPressed = false;
            }
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