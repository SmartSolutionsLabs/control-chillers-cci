#ifndef _INC_CONTROL_
#define _INC_CONTROL_

#include <Module.hpp>
#include "GraphicLcd.hpp"
#include "Chiller.hpp"
#include "Pump.hpp"

class Control : public Module {
private:
    Chiller **chillers;  // Control de los chillers
    Pump **pumps;        // Control de las bombas

    GraphicLCD *lcd;   // Pantalla gráfica

    Screen newScreen = HOME;  // Pantalla actual
    bool enter;               // Indica si se ha presionado "Enter"
    uint8_t option;           // Opción seleccionada en el menú
    bool ScreenSelected;      // Indica si se ha seleccionado una pantalla
    bool optionSelected;      // Indica si se ha seleccionado una opción

    // Nuevos atributos para los modos y tiempos
    enum Mode {
        MANUAL_MODE,    // Modo manual
        AUTOMATIC_MODE   // Modo automático
    };

    Mode currentMode;    // Modo actual (manual o automático)
    uint32_t delay1;     // Tiempo 1 para modo automático
    uint32_t delay2;     // Tiempo 2 para modo automático

	Screen currentScreen = HOME;

public:
    Control(const char *name, int taskCore = 1);

    void connect(void *data) override;
    void run(void *data) override;
    void handleKey(char key);

    void setGraphicLCD(GraphicLCD *newlcd) {
        this->lcd = newlcd;
    }

	void setChiller(uint8_t index, Chiller *newChiller);
	void setPump(uint8_t index, Pump *newPump);

    void proccessEnterKey();
    void proccessBackKey();
    void proccessUpKey();
    void proccessDownKey();

    // Métodos para obtener y establecer el modo
    void setMode(Mode mode) {
        currentMode = mode;
    }

    Mode getMode() const {
        return currentMode;
    }

    // Métodos para obtener y establecer los tiempos
    void setDelay1(uint32_t time) {
        delay1 = time;
    }

    uint32_t getDelay1() const {
        return delay1;
    }

    void setDelay2(uint32_t time) {
        delay2 = time;
    }

    uint32_t getDelay2() const {
        return delay2;
    }

	void nextScreen();

	void previousScreen();

	Screen getScreen();
};

#endif