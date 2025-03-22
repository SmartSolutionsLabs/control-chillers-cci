#ifndef _INC_CONTROL_
#define _INC_CONTROL_

#include <Module.hpp>
#include "GraphicLcd.hpp"
#include "Chiller.hpp"
#include "Pump.hpp"
#include "Network.hpp"

class Control : public Module {
private:
    Chiller **chillers;  // Control de los chillers
    Pump **pumps;        // Control de las bombas

    GraphicLCD *lcd;   // Pantalla gráfica

    Screen newScreen = MANUAL;  // Pantalla actual
    bool enter;               // Indica si se ha presionado "Enter"
    uint8_t currentOption;           // Opción seleccionada en el menú
    uint8_t *maxOptions;
    bool ScreenSelected;      // Indica si se ha seleccionado una pantalla
    bool optionSelected;      // Indica si se ha seleccionado una opción

    // Nuevos atributos para los modos y tiempos
    enum Mode {
        MANUAL_MODE,    // Modo manual
        AUTOMATIC_MODE   // Modo automático
    };

    enum ChillerMode{
        NONE_SELECTED,
        CHILLER_1_SELECTED,    // Modo manual
        CHILLER_2_SELECTED   // Modo automático
    };

    ChillerMode chillerMode;
    ChillerMode lastChillerMode = NONE_SELECTED;
    Mode currentMode;    // Modo actual (manual o automático)
    uint32_t delayCounter[2];     // Tiempo 1 para modo automático
    uint32_t timerDelayCounter[2];
    uint8_t delay[2];
    bool flag_process[2];
	Screen currentScreen = HOME;
    uint8_t GPIOA = 0x00;

    bool automaticSecuenceOn[2] = {false,false};
    bool automaticSecuenceOff[2] = {false,false};
    IPAddress IP;
    bool wifiStatus = false;

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

    // Métodos para obtener y establecer los tiempo

    void setDelay(uint8_t index , uint32_t newTime) {
        this->delay[index - 1] = newTime;
    }

    uint32_t getDelay(uint8_t index) const {
        return this->delay[index -1];
    }

	void nextScreen();

	void previousScreen();

	Screen getScreen();

    void manualControlDevice();

    void processChiller();

    void setProcessChiller(uint8_t index);

    void turnOnAutomaticSecuence(int index);
    void turnOffAutomaticSecuence(int index);

    void turnOnChiller(int index);
    void turnOffChiller(int index);
    
    void turnOnPump(int index);
    void turnOffPump(int index);

    void updateProgressBar(int index);

    void nextOption();
    void previousOption();

    void upValueOption();
    void downValueOption();
    
    void processOption();

    void setWifiIP(IPAddress newIP){ // aqui añadir metodos para impresion de pantalla
        this->IP = newIP;
        Serial.print("setWifiIP: ");
        Serial.println(this->IP);
    }

    void setWifiStatus(bool newWifiStatus){
        this->wifiStatus = newWifiStatus;
        Serial.print("setWifiStatus: ");
        Serial.println(this->wifiStatus);
    }

    void connectWifi(){
        Network::getInstance()->connect();
    }

    void disconectWifi(){
        //Network::getInstance()->disconnect(); // como george no lo incluyo en la clase
        WiFi.disconnect(true);
    }

    void setWifiSSID(String newSSID){
        Network::SSID = newSSID;
    }

    void setWifiPSW(String newPSWD){
        Network::PASSWORD = newPSWD;
    }
};

#endif