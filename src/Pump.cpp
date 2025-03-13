#include "Pump.hpp"

#define MCP23017_ADDR 0x27  // Dirección I2C del MCP23017 (ajústala si es necesario)
#define IODIRA 0x00         // Registro para configurar pines como entrada/salida (banco A)
#define IODIRB 0x01         // Registro para configurar pines como entrada/salida (banco B)
#define GPIOA  0x12         // Registro de lectura de pines (banco A)
#define GPIOB  0x13         // Registro de lectura de pines (banco B)

Pump::Pump() : Module("Pump", 1) {
    // Constructor por defecto necesario para permitir new Pump[2]
}

Pump::Pump(const char * name, int taskCore) : Module(name, taskCore) {
}

void Pump::connect(void *data) {
	this->wire = static_cast<TwoWire*>(data);  // Asignar el puntero a Wire desde los datos

    if (this->wire) {
        // Configurar todos los pines del MCP23017 como entrada
        this->writeRegister(IODIRA, 0xFF);  // Todos los pines del banco A como entrada
        this->writeRegister(IODIRB, 0xFF);  // Todos los pines del banco B como entrada
    }

	pinMode(15,OUTPUT);
    pinMode(41,OUTPUT);
    digitalWrite(15,HIGH);
    digitalWrite(41,HIGH);
    this->writeRegister(IODIRA, 0xFF);  // Todos los pines del banco A como entrada
    this->writeRegister(IODIRB, 0xFF);  // Todos los pines del banco B como entrada
    // Aquí puedes realizar cualquier inicialización adicional que requiera Wire
	this->state = false;
}

void Pump::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;
	if (this->wire) {
        // Usar Wire para comunicación I2C
    }
	while (1) {
		vTaskDelay(this->iterationDelay);
	}
}




// Función para escribir en un registro del MCP23017
void Pump::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// Función para leer un registro del MCP23017
uint8_t Pump::readRegister(uint8_t reg) {
    Wire.beginTransmission(MCP23017_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(MCP23017_ADDR, 1);
    return Wire.available() ? Wire.read() : 0;
}

void Pump::writePin(uint8_t pin, bool state) {
    if (this->wire) {
        uint8_t registro = (pin < 8) ? GPIOA : GPIOB;  // Determinar si está en GPIOA o GPIOB
        uint8_t estadoActual = this->readRegister(registro);  // Leer el estado actual del banco

        if (state) {
            estadoActual |= (1 << (pin % 8));  // Encender el pin (establecer el bit en 1)
        } else {
            estadoActual &= ~(1 << (pin % 8));  // Apagar el pin (establecer el bit en 0)
        }

        this->writeRegister(registro, estadoActual);  // Escribir el nuevo estado en el banco
    }
}

void Pump::turnOn(){
	this->state = true;
	writePin(this->pin,this->state);
}

void Pump::turnOff(){
	this->state = false;
	writePin(this->pin,this->state);
}

void Pump::setPin(uint8_t newPin){
	this->pin = newPin;
}

bool Pump::getState(){
	return this->state;
}