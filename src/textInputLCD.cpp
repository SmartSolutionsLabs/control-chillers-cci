#include "textInputLCD.hpp"

// Constructores
textInputLCD::textInputLCD() 
    : u8g2(nullptr), ID(0), currentState(InputState::NONE), run(false), updateTimer(0), timer(0),
      xCenterLabel(0), yCenterLabel(0), xPosition(0), yPosition(0), value(0),
      percentage(0), integer(0), width(0), height(0), xLabelPosition(0),
      yLabelPosition(0), labelInput("Delay") {}

textInputLCD::textInputLCD(U8G2_ST7920_128X64_F_SW_SPI *newu8g2) 
    : u8g2(newu8g2), ID(0), currentState(InputState::NONE), run(false), updateTimer(0), timer(0),
      xCenterLabel(0), yCenterLabel(0), xPosition(0), yPosition(0), value(0),
      percentage(0), integer(0), width(0), height(0), xLabelPosition(0),
      yLabelPosition(0), labelInput("Delay") {}

// Métodos para gestionar el ID
void textInputLCD::setID(uint8_t newID) {
    this->ID = newID;
}

uint8_t textInputLCD::getID() const {
    return this->ID;
}

// Métodos para gestionar el estado de ejecución
void textInputLCD::setRun(bool newRun) {
    this->run = newRun;
}

bool textInputLCD::getRun() const {
    return this->run;
}

// Métodos para gestionar los temporizadores
void textInputLCD::setUpdateTimer(uint32_t newUpdateTimer) {
    this->updateTimer = newUpdateTimer;
}

uint32_t textInputLCD::getUpdateTimer() const {
    return this->updateTimer;
}

void textInputLCD::setTimer(uint32_t newTimer) {
    this->timer = newTimer;
}

uint32_t textInputLCD::getTimer() const {
    return this->timer;
}

// Métodos para gestionar el valor
void textInputLCD::setValue(uint8_t newValue) {
    this->value = newValue;
}

uint8_t textInputLCD::getValue() const {
    return this->value;
}

// Métodos para gestionar la posición
void textInputLCD::setPosition(uint8_t xpos, uint8_t ypos) {
    this->xPosition = xpos;
    this->yPosition = ypos;
}

void textInputLCD::setLabelPosition(uint8_t newXlabelPosition, uint8_t newYlabelPosition) {
    this->xLabelPosition = newXlabelPosition;
    this->yLabelPosition = newYlabelPosition;
}

// Métodos para gestionar el label
void textInputLCD::drawLabelInput(bool show) {
    if (show) {
        this->u8g2->drawStr(xLabelPosition, yLabelPosition, this->labelInput.c_str());  // Usar .c_str()
    }
}

void textInputLCD::showLabelInput() {
    this->drawLabelInput(true);
}

void textInputLCD::hideLabelInput() {
    this->drawLabelInput(false);
}

void textInputLCD::setLabelInput(const std::string &newLabelInput) {
    this->labelInput = newLabelInput;  // Asignar directamente el std::string
}

const std::string& textInputLCD::getLabelInput() const {
    return this->labelInput;  // Devolver una referencia constante
}

// Métodos para gestionar el estado (InputState)
void textInputLCD::setState(InputState newState) {
    this->currentState = newState;
}

InputState textInputLCD::getState() const {
    return this->currentState;
}

// Métodos para dibujar
void textInputLCD::draw(bool show) {
    if (show) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%lu", this->integer);
        buffer[sizeof(buffer) - 1] = '\0';

        // Obtener el ancho del texto
        int textWidth = this->u8g2->getStrWidth(buffer);

        // Dibujar el rectángulo según el estado
        switch (this->currentState) {
            case InputState::SELECTED:
                // Fondo blanco, texto negro
                this->u8g2->setDrawColor(1);
                this->u8g2->drawBox(this->xPosition, this->yPosition, this->width, this->height);
                this->u8g2->setDrawColor(0);
                break;

            case InputState::NAVIGATED:
                // Borde resaltado, fondo normal
                this->u8g2->setDrawColor(1);
                this->u8g2->drawFrame(this->xPosition, this->yPosition, this->width, this->height);
                this->u8g2->setDrawColor(0);
                break;

            case InputState::NONE:
            default:
                // Fondo normal, texto normal
                this->u8g2->setDrawColor(1);
                break;
        }

        // Dibujar el texto centrado
        int textX = this->xPosition + (this->width - textWidth) / 2;  // Centrado horizontal
        int textY = this->yPosition + (this->height / 2) + 4;         // Centrado vertical

        this->u8g2->setFont(u8g2_font_6x10_tf);
        this->u8g2->drawStr(textX, textY, buffer);

        // Dibujar el label
        this->drawLabelInput(true);  // Dibujar el label
    }
}

void textInputLCD::show() {
    this->draw(true);
}

void textInputLCD::hide() {
    this->draw(false);
}

// Métodos para gestionar el valor entero
void textInputLCD::setInteger(uint32_t newInteger) {
    this->integer = newInteger;
}

uint32_t textInputLCD::getInteger() const {
    return this->integer;
}

// Métodos para gestionar el ancho y alto
void textInputLCD::setWidth(uint8_t newWidth) {
    this->width = newWidth;
}

uint8_t textInputLCD::getWidth() const {
    return this->width;
}

void textInputLCD::setHeight(uint8_t newHeight) {
    this->height = newHeight;
}

uint8_t textInputLCD::getHeight() const {
    return this->height;
}

// Métodos para gestionar el porcentaje
void textInputLCD::setPercentage(uint8_t newPercentage) {
    this->percentage = newPercentage;
}

uint8_t textInputLCD::getPercentage() const {
    return this->percentage;
}

// Métodos para gestionar el centro del label
void textInputLCD::setCenterLabel(uint8_t xCenter, uint8_t yCenter) {
    this->xCenterLabel = xCenter;
    this->yCenterLabel = yCenter;
}

uint8_t textInputLCD::getXCenterLabel() const {
    return this->xCenterLabel;
}

uint8_t textInputLCD::getYCenterLabel() const {
    return this->yCenterLabel;
}