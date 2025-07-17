#ifndef USING_EMULATOR

#include "Pump.hpp"

#define MCP23017_ADDR 0x27  // Dirección I2C del MCP23017 (ajústala si es necesario)
#define IODIRA 0x00         // Registro para configurar pines como entrada/salida (banco A)
#define GPIOA  0x12         // Registro de lectura/escritura de pines (banco A)

Pump::Pump() : Module("Pump", 1) {
    // Constructor por defecto
}

Pump::Pump(const char * name, int taskCore) : Module(name, taskCore) {
    // Constructor con nombre y núcleo de tarea
}

void Pump::connect(void *data) {
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

    this->state = false;  // Inicializar el estado del Pump como apagado
}

void Pump::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;

    while (1) {
        vTaskDelay(this->iterationDelay);
    }
}

// Función para leer un registro del MCP23017
uint8_t Pump::readRegister(uint8_t reg) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    
    if (Wire.endTransmission() != 0) {  
        Serial.println("Error: Falló la transmisión al MCP23017");
        return 0;  
    }

    Wire.requestFrom(MCP23017_ADDR, 1);
    if (Wire.available() < 1) {  
        Serial.println("Error: No hay datos disponibles del MCP23017");
        return 0;
    }

    return Wire.read();
}

// Función para escribir en un pin específico del banco A
void Pump::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();

    if (reg == GPIOA) {
        estadoGPIOA = value;  // 🟢 Sincroniza el estado en RAM
    }
}

void Pump::setGPIOA(uint8_t newState) {
    writeRegister(GPIOA, newState);  // 📝 Escribir en todo el banco GPIOA
}

// Método para encender el Pump
void Pump::turnOn() {
    this->state = true;
    //writePin(this->pin, this->state);
}

// Método para apagar el Pump
void Pump::turnOff() {
    this->state = false;
    //writePin(this->pin, this->state);
}

// Método para alternar el estado del Pump
void Pump::toggle(uint8_t newRegister) {
    this->estadoGPIOA = newRegister ;
    uint8_t bitValue = (this->estadoGPIOA >> this->pin) & 1;
    this->state = bitValue;  // Alternar el estado
    writeRegister(GPIOA , newRegister);
    //writePin(this->pin, this->state);  // Actualizar el estado del pin
}

// Método para establecer el pin de control del Pump
void Pump::setPin(uint8_t newPin) {
    this->pin = newPin;  // Almacenar el pin en la instancia
}

// Método para obtener el estado actual del Pump
bool Pump::getState() {
    return this->state;
}

void Pump::setState(bool newState){
    this->state = newState;
}

#endif // ifndef USING_EMULATOR
