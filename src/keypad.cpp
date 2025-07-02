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

    // Configurar los pines del banco B como entradas (botones)
    writeRegister(IODIRB, 0xFF);  // Todos los pines del banco B como entradas

    // Habilitar resistencias pull-up en todos los pines del banco B
    writeRegister(GPPUB, 0xFF);  

    // Habilitar interrupciones solo en los pines 8-11 (mantener 12-15 sin interrupción)
    writeRegister(GPINTENB, 0x0F);  // 0x0F = 00001111 (interrupciones solo en pines 8, 9, 10, 11)

    // Configurar interrupciones solo para pines 8-11 como FALLING
    writeRegister(INTCONB, 0x00);   // 0 = comparar con el estado anterior, cambio libre
    writeRegister(DEFVALB, 0x00);   // No aplica para estos pines en cambio libre

    // Configurar pines de interrupción del MCP23017
    pinMode(6, INPUT_PULLUP);  // INT A
    pinMode(7, INPUT_PULLUP);  // INT B

    // Asignar manejadores de interrupción
    attachInterrupt(digitalPinToInterrupt(6), []() { keypadInstance->handleInterruptA(); }, FALLING);
    attachInterrupt(digitalPinToInterrupt(7), []() { keypadInstance->handleInterruptB(); }, FALLING);

}


void Keypad::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;

    Serial.println("Keypad task started...");
    vTaskDelay(2000* this->iterationDelay);
    while (1) {
        if (millis() - this->timerAutomatic > 100) {
            uint8_t gpioB = readRegister(GPIOB);  // Leer el estado actual del banco B
        
            for (uint8_t i = 4; i < 8; i++) {  // Pines 12 y 13 corresponden a bits 4 y 5
                bool wasHigh = lastKeyStates[i];      // Guardar estado anterior
                bool isHigh = gpioB & (1 << i);       // Leer estado actual
                lastKeyStates[i] = isHigh;            // Actualizar estado
        
                if (wasHigh && !isHigh) {  // FALLING EDGE
                    lastKeyPressed = 'E' + (i - 4);  // Asignar tecla (E para 12, F para 13)
        
                    Serial.print("FALLING detectado en pin ");
                    Serial.print(12 + (i - 4));
                    Serial.print(" - Tecla: ");
                    Serial.println(lastKeyPressed);
        
                    if (control != nullptr) {
                        control->handleKey(lastKeyPressed);  // Enviar evento
                    }
                } 
                
                else if (!wasHigh && isHigh) {  // RISING EDGE
                    char keyRising = 'G' + (i - 4);  // Nueva tecla para RISING
        
                    Serial.print("RISING detectado en pin ");
                    Serial.print(12 + (i - 4));
                    Serial.print(" - Tecla: ");
                    Serial.println(keyRising);
        
                    if (control != nullptr) {
                        control->handleKey(keyRising);  // Enviar evento
                    }
                }
            }
        }
        

        if (interruptFlag) {
            interruptFlag = false;

            // Leer qué pines activaron la interrupción
            uint8_t intCap = readRegister(INTCAPB);
            uint8_t gpioB = readRegister(GPIOB);
            
            Serial.print("INTCAPB: ");
            Serial.println(intCap, BIN);
            Serial.print("GPIOB: ");
            Serial.println(gpioB, BIN);

            // Detectar pines 8-11 (solo en FALLING)
            for (uint8_t i = 0; i < 4; i++) {
                //bool isFalling = (intCap & (1 << i)) && !(gpioB & (1 << i));
                bool isFalling = lastKeyStates[i] && !(gpioB & (1 << i));  // Antes era HIGH, ahora es LOW
                lastKeyStates[i] = gpioB & (1 << i);  // Actualizar estado

                /*
                Serial.print("Pin ");
                Serial.print(8 + i);
                Serial.print(" - Estado: ");
                Serial.print((gpioB & (1 << i)) ? "HIGH" : "LOW");
                Serial.print(" - Interrupción: ");
                Serial.print((intCap & (1 << i)) ? "1" : "0");
                Serial.print(" - Falling detectado: ");
                Serial.println(isFalling ? "Sí" : "No");
                */
                if (isFalling) {
                    lastKeyPressed = 'A' + i;  // Asignar tecla (A, B, C, D)
                    keyPressed = true;
                    lastKeyPressTime = millis();
                    if (control != nullptr) {
                        Serial.print("Evento enviado: ");
                        Serial.println(lastKeyPressed);
                        control->handleKey(lastKeyPressed);  // Enviar evento
                    }
                    break;  // Solo procesar un botón a la vez
                }
            }

            // Detectar pines 12 y 13 (en cualquier cambio de estado)
            
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