#include "Chiller.hpp"

#define MCP23017_ADDR 0x27  // Dirección I2C del MCP23017 (ajústala si es necesario)
#define IODIRA 0x00         // Registro para configurar pines como entrada/salida (banco A)
#define GPIOA  0x12         // Registro de lectura/escritura de pines (banco A)

Chiller::Chiller() : Module("Chiller", 1) {
    // Constructor por defecto
}

Chiller::Chiller(const char * name, int taskCore) : Module(name, taskCore) {
    // Constructor con nombre y núcleo de tarea
}

void Chiller::connect(void *data) {
    this->wire = static_cast<TwoWire*>(data);  // Asignar el puntero a Wire desde los datos

    if (this->wire) {
        // Configurar todos los pines del banco A como salidas
        this->writeRegister(IODIRA, 0x00);  // 0x00 = todos los pines como salidas
    }

    pinMode(15, OUTPUT);  // Configurar el pin 15 como salida
    pinMode(41, OUTPUT);  // Configurar el pin 41 como salida
    digitalWrite(15, HIGH);  // Establecer el pin 15 en HIGH
    digitalWrite(41, HIGH);  // Establecer el pin 41 en HIGH

    this->state = false;  // Inicializar el estado del chiller como apagado
}

void Chiller::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;

    if (this->wire) {
        // Usar Wire para comunicación I2C (si es necesario)
    }

    while (1) {
        vTaskDelay(this->iterationDelay);
    }
}

// Función para escribir en un registro del MCP23017
void Chiller::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// Función para leer un registro del MCP23017
uint8_t Chiller::readRegister(uint8_t reg) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(MCP23017_ADDR, 1);
    return Wire.available() ? Wire.read() : 0;
}

// Función para escribir en un pin específico del banco A
void Chiller::writePin(uint8_t pin, bool state) {
    if (this->wire) {
        uint8_t estadoActual = this->readRegister(GPIOA);  // Leer el estado actual del banco A

        if (state) {
            estadoActual |= (1 << pin);  // Encender el pin (establecer el bit en 1)
        } else {
            estadoActual &= ~(1 << pin);  // Apagar el pin (establecer el bit en 0)
        }

        this->writeRegister(GPIOA, estadoActual);  // Escribir el nuevo estado en el banco A
    }
}

// Método para encender el chiller
void Chiller::turnOn() {
    this->state = true;
    writePin(this->pin, this->state);
}

// Método para apagar el chiller
void Chiller::turnOff() {
    this->state = false;
    writePin(this->pin, this->state);
}

// Método para alternar el estado del chiller
void Chiller::toggle() {
    this->state = !this->state;  // Alternar el estado
    writePin(this->pin, this->state);  // Actualizar el estado del pin
}

// Método para establecer el pin de control del chiller
void Chiller::setPin(uint8_t newPin) {
    this->pin = newPin;
}

// Método para obtener el estado actual del chiller
bool Chiller::getState() {
    return this->state;
}