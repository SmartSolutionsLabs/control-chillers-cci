#ifndef USING_EMULATOR

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
    pinMode(15, OUTPUT);  // Configurar el pin 15 como salida
    pinMode(41, OUTPUT);  // Configurar el pin 41 como salida
    digitalWrite(15, HIGH);  // Establecer el pin 15 en HIGH
    digitalWrite(41, HIGH);  // Establecer el pin 41 en HIGH

    // Configurar todos los pines del banco A como salida
    this->writeRegister(IODIRA, 0x00);
    vTaskDelay(this->iterationDelay );  // Espera breve para asegurar la configuración

    // Verificar que se configuró correctamente
    uint8_t iodir = this->readRegister(IODIRA);
    Serial.print("IODIRA configurado como: 0x");
    Serial.println(iodir, HEX);

    if (iodir != 0x00) {
        Serial.println("Error: IODIRA no se configuró correctamente.");
    }

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

// Función para leer un registro del MCP23017
uint8_t Chiller::readRegister(uint8_t reg) {
    this->wire->beginTransmission(MCP23017_ADDR);
    this->wire->write(reg);

    if (this->wire->endTransmission() != 0) {
        Serial.println("Error: Falló la transmisión al MCP23017");
        return 0;
    }

    this->wire->requestFrom(MCP23017_ADDR, 1);
    if (this->wire->available() < 1) {
        Serial.println("Error: No hay datos disponibles del MCP23017");
        return 0;
    }

    return this->wire->read();
}

void Chiller::writeRegister(uint8_t reg, uint8_t value) {
    this->wire->beginTransmission(MCP23017_ADDR);
    this->wire->write(reg);
    this->wire->write(value);
    this->wire->endTransmission();

    if (reg == GPIOA) {
        estadoGPIOA = value;  // Sincroniza el estado en RAM
    }
}

void Chiller::setGPIOA(uint8_t newState) {
    writeRegister(GPIOA, newState);  // 📝 Escribir en todo el banco GPIOA
}

// Método para encender el chiller
void Chiller::turnOn() {
    this->state = true;
    //writePin(this->pin, this->state);
}

// Método para apagar el chiller
void Chiller::turnOff() {
    this->state = false;
    //writePin(this->pin, this->state);
}

// Método para alternar el estado del chiller
void Chiller::toggle(uint8_t newRegister) {
    this->estadoGPIOA = newRegister ;
    uint8_t bitValue = (this->estadoGPIOA >> this->pin) & 1;
    this->state = bitValue;  // Alternar el estado
    writeRegister(GPIOA , newRegister);
    //writePin(this->pin, this->state);  // Actualizar el estado del pin
}

// Método para establecer el pin de control del chiller
void Chiller::setPin(uint8_t newPin) {
    this->pin = newPin;
}

// Método para obtener el estado actual del chiller
bool Chiller::getState() {
    return this->state;
}
void Chiller::setState(bool newState){
    this->state = newState;
}

#endif // ifndef USING_EMULATOR
