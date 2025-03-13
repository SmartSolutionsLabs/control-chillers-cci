#include "Keypad.hpp"
#include <Wire.h>

#define MCP23017_ADDR 0x27  // Dirección I2C del MCP23017 (ajústala si es necesario)
#define IODIRA 0x00         // Registro para configurar pines como entrada/salida (banco A)
#define IODIRB 0x01         // Registro para configurar pines como entrada/salida (banco B)
#define GPIOA  0x12         // Registro de lectura de pines (banco A)
#define GPIOB  0x13         // Registro de lectura de pines (banco B)

Keypad::Keypad(const char * name, int taskCore) : Module(name, taskCore) {
    Serial.println("keypad constructor");
}

// Función para escribir en un registro del MCP23017
void Keypad::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// Función para leer un registro del MCP23017
uint8_t Keypad::readRegister(uint8_t reg) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(MCP23017_ADDR, 1);
    return Wire.available() ? Wire.read() : 0;
}

void Keypad::connect(void * data) {
    pinMode(15,OUTPUT);
    pinMode(41,OUTPUT);
    digitalWrite(15,HIGH);
    digitalWrite(41,HIGH);
    this->writeRegister(IODIRA, 0xFF);  // Todos los pines del banco A como entrada
    this->writeRegister(IODIRB, 0xFF);  // Todos los pines del banco B como entrada
    // Configurar pines del PCF8574
    
}
void Keypad::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;
    bool lastState[16] = {1}; // Array para guardar el estado anterior de cada pin (asumimos que inician en ALTO)

    while (1) {
        vTaskDelay(this->iterationDelay);
        unsigned long currentMillis = millis();  

        if (currentMillis - lastScanTime >= SCAN_INTERVAL) {
            lastScanTime = currentMillis;  

            for (int i = 0; i < 16; i++) {
                bool currentState = leerPin(i); // Leer el estado actual del pin

                if (lastState[i] == HIGH && currentState == LOW) { // Detectar cambio de ALTO a BAJO
                    if (i == 8) {
                        this->control->handleKey('A');
                    } 
                    else if (i == 9) {
                        this->control->handleKey('B');
                    }
                    else if (i == 10) {
                        this->control->handleKey('*');
                    }
                    else if (i == 11) {
                        this->control->handleKey('#');
                    }
                    Serial.printf("| IO%d FALLING EDGE DETECTED!\t", i);
                }

                lastState[i] = currentState; // Actualizar estado anterior
            }
        }
    }
}


bool Keypad::leerPin(uint8_t pin) {
    uint8_t registro = (pin < 8) ? GPIOA : GPIOB; // Determinar si está en GPIOA o GPIOB
    uint8_t estado = this->readRegister(registro);      // Leer el registro del banco
    return estado & (1 << (pin % 8));             // Extraer el estado del pin específico
}
