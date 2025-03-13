#include "textInputLCD.hpp"

textInputLCD::textInputLCD() : u8g2(nullptr), ID(0), state(false), run(false), updateTimer(0), timer(0),
                               xCenterLabel(0), yCenterLabel(0), xPosition(0), yPosition(0), value(0),
                               percentage(0), integer(0), width(0), height(0), xLabelPosition(0),
                               yLabelPosition(0), labelInput("") {}

textInputLCD::textInputLCD(U8G2_ST7920_128X64_F_SW_SPI *newu8g2) : u8g2(newu8g2), ID(0), state(false),
                                                                   run(false), updateTimer(0), timer(0),
                                                                   xCenterLabel(0), yCenterLabel(0),
                                                                   xPosition(0), yPosition(0), value(0),
                                                                   percentage(0), integer(0), width(0),
                                                                   height(0), xLabelPosition(0),
                                                                   yLabelPosition(0), labelInput("") {}

void textInputLCD::setID(uint8_t newID) {
    this->ID = newID;
}

uint8_t textInputLCD::getID() const {
    return this->ID;
}

void textInputLCD::setState(bool newState) {
    this->state = newState;
}

bool textInputLCD::getState() const {
    return this->state;
}

void textInputLCD::setRun(bool newRun) {
    this->run = newRun;
}

bool textInputLCD::getRun() const {
    return this->run;
}

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

void textInputLCD::setValue(uint8_t newValue) {
    this->value = newValue;
}

uint8_t textInputLCD::getValue() const {
    return this->value;
}

void textInputLCD::setPosition(uint8_t xpos, uint8_t ypos) {
    this->xPosition = xpos;
    this->yPosition = ypos;
}

void textInputLCD::setLabelPosition(uint8_t newXlabelPosition, uint8_t newYlabelPosition) {
    this->xLabelPosition = newXlabelPosition;
    this->yLabelPosition = newYlabelPosition;
}

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

void textInputLCD::draw(bool show) {
    if (show) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%lu", this->integer);
        buffer[sizeof(buffer) - 1] = '\0';

        // Obtener el ancho del texto
        int textWidth = this->u8g2->getStrWidth(buffer);

        // Dibujar el rectángulo
        this->u8g2->setDrawColor(1);
        this->u8g2->drawBox(this->xPosition, this->yPosition, this->width, this->height);
        this->u8g2->setDrawColor(0);
        this->u8g2->drawBox(this->xPosition + 1, this->yPosition + 1, this->width - 2, this->height - 2);
        this->u8g2->setDrawColor(1);

        // Calcular la posición del texto para centrarlo dentro del rectángulo
        int textX = this->xPosition + (this->width - textWidth) / 2;  // Centrado horizontal
        int textY = this->yPosition + (this->height / 2) + 4;         // Centrado vertical

        // Dibujar el texto centrado
        this->u8g2->setFont(u8g2_font_6x10_tf);
        this->u8g2->drawStr(textX, textY, buffer);

        // Obtener el ancho del label
        int labelWidth = this->u8g2->getStrWidth(this->labelInput.c_str());

        // Margen entre el label y el rectángulo
        int margin = 5;

        // Posición X del label (a la izquierda del rectángulo)
        int labelX = this->xPosition - labelWidth - margin;

        // Obtener la altura de la fuente
        int fontHeight = 10;  // Altura de la fuente u8g2_font_6x10_tf

        // Posición Y del label (centrado verticalmente respecto al rectángulo)
        int labelY = this->yPosition + (this->height / 2) + (fontHeight / 2);

        // Establecer la posición del label
        this->setLabelPosition(labelX, labelY);

        // Dibujar el label
    }
}

void textInputLCD::show() {
    this->draw(true);
}

void textInputLCD::hide() {
    this->draw(false);
}

void textInputLCD::setInteger(uint32_t newInteger) {
    this->integer = newInteger;
}

void textInputLCD::setWidth(uint8_t newWidth) {
    this->width = newWidth;
}

void textInputLCD::setHeight(uint8_t newHeight) {
    this->height = newHeight;
}