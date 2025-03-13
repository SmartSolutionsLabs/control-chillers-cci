#ifndef _INC_TEXTINPUTLCD_
#define _INC_TEXTINPUTLCD_

#include <U8g2lib.h>
#include <string>  // Para usar std::string

class textInputLCD {
private:
    U8G2_ST7920_128X64_F_SW_SPI *u8g2;  // Puntero al objeto LCD

    uint8_t ID;                         // Identificador del objeto
    bool state;                         // Estado actual (activo/inactivo)
    bool run;                           // Indica si el objeto está en ejecución
    uint32_t updateTimer;               // Temporizador para actualizaciones
    uint32_t timer;                     // Temporizador general

    uint8_t xCenterLabel, yCenterLabel; // Posición central del label
    uint8_t xPosition, yPosition;       // Posición del objeto en la pantalla
    uint16_t value, percentage;         // Valor y porcentaje
    uint32_t integer;                   // Valor entero
    uint8_t width, height;              // Ancho y alto del objeto
    uint8_t xLabelPosition, yLabelPosition; // Posición del label

    std::string labelInput;             // Texto del label (usando std::string)

public:
    // Constructores
    textInputLCD();
    textInputLCD(U8G2_ST7920_128X64_F_SW_SPI *lcd);

    // Métodos para dibujar texto e imágenes
    void drawCenteredText(int xCenter, int y, const char *text);

    // Métodos para gestionar el ID
    void setID(uint8_t newID);
    uint8_t getID() const;

    // Métodos para gestionar el estado
    void setState(bool newState);
    bool getState() const;

    // Métodos para gestionar la ejecución
    void setRun(bool newRun);
    bool getRun() const;

    // Métodos para gestionar los temporizadores
    void setUpdateTimer(uint32_t newUpdateTimer);
    uint32_t getUpdateTimer() const;

    void setTimer(uint32_t newTimer);
    uint32_t getTimer() const;

    // Métodos para gestionar el valor y el porcentaje
    void setValue(uint8_t newValue);
    uint8_t getValue() const;

    void setPercentage(uint8_t newPercentage);
    uint8_t getPercentage() const;

    // Métodos para gestionar la posición
    void setPosition(uint8_t xpos, uint8_t ypos);

    // Métodos para gestionar el label
    void setLabelPosition(uint8_t xCenter, uint8_t yCenter);
    void drawLabelInput(bool show);
    void showLabelInput();
    void hideLabelInput();
    void setLabelInput(const std::string &newLabelInput);  // Usamos std::string
    const std::string& getLabelInput() const;             // Devolvemos una referencia constante

    // Métodos para gestionar el entero
    void setInteger(uint32_t newInteger);

    // Métodos para dibujar y ocultar el objeto
    void draw(bool show);
    void show();
    void hide();

    // Métodos para gestionar el tamaño
    void setWidth(uint8_t newWidth);
    void setHeight(uint8_t newHeight);
};

#endif