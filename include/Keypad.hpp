#ifndef _INC_KEYPAD_
#define _INC_KEYPAD_

#include <Module.hpp>
#include "Control.hpp"
#include <Wire.h>

#define MCP23017_ADDR 0x27  // Dirección I2C del MCP23017
#define IODIRA 0x00         // Registro para configurar pines como entrada/salida (banco A)
#define IODIRB 0x01         // Registro para configurar pines como entrada/salida (banco B)
#define GPPUB 0x0D          // Registro para habilitar resistencias pull-up (banco B)
#define GPINTENB 0x05       // Registro para habilitar interrupciones (banco B)
#define INTCONB 0x09        // Registro para configurar el modo de interrupción (banco B)
#define DEFVALB 0x07        // Registro para definir el valor de comparación (banco B)
#define INTCAPB 0x11        // Registro de captura de interrupción (banco B)
#define GPIOB 0x13

#define INTFBB 0x0F

class Keypad : public Module {
private:
    Control * control;
    uint8_t buttonPins[4] = {8, 9, 10, 11};  // Pines de los 4 botones en el MCP23017 (banco B)
    unsigned long lastInterruptTime = 0;     // Variable para el control de tiempo
    const unsigned long debounceDelay = 50;  // Tiempo de debounce en milisegundos

     // Variables para el control de pulsaciones repetidas
     bool interruptFlag = false;  // Bandera de interrupción
     bool keyPressed = false;     // Indica si un pulsador está actualmente presionado
     char lastKeyPressed = '\0';  // Última tecla presionada
     unsigned long lastKeyPressTime = 0;  // Tiempo de la última pulsación
     const unsigned long initialDelay = 750;  // Retardo inicial antes de repetir (750 ms)
     const unsigned long repeatInterval = 100;  // Intervalo de repetición (100 ms)

     bool lastKeyStates[8] = {true, true, true, true, true, true, true, true};
     uint32_t timerAutomatic;
public:
    Keypad(const char * name, int taskCore = 1);
    void connect(void* data);
    void run(void* data);
    void setControl(Control * newControl) {
        control = newControl;
    };

    void handleInterruptA();  // Método para manejar interrupciones del bloque A
    void handleInterruptB();  // Método para manejar interrupciones del bloque B
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
};

#endif