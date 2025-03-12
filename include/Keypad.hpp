#ifndef _INC_KEYPAD_
#define _INC_KEYPAD_

#include <Module.hpp>
#include "Control.hpp"
#include <Wire.h>
#include <PCF8574.h>
#define SCAN_INTERVAL 50    // Intervalo de escaneo en milisegundos

class Keypad : public Module {
private:
    Control * control;
    PCF8574 * pcf;
    uint8_t address;
    uint8_t rowPins[4] = {3, 2, 1, 0};  // Pines de filas en el PCF8574
    uint8_t colPins[4] = {4, 5, 6, 7};  // Pines de columnas en el PCF8574
    char keys[4][4] = {  // Mapa de teclas
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    unsigned long lastScanTime = 0;  // Variable para el control de tiempo

    bool enter;
    uint8_t option;
    
public:
    Keypad(const char * name, int taskCore = 1);
    void connect(void* data);
    void run(void* data);
    char getKey();
    void setControl(Control * newControl){
        control = newControl;
    };

    void writeRegister(uint8_t reg, uint8_t value);
    
    // Función para leer un registro del MCP23017
    uint8_t readRegister(uint8_t reg);
    
    bool leerPin(uint8_t pin);
};

#endif
