#include "ListLCD.hpp"

// Constructores
ListLCD::ListLCD() 
    : u8g2(nullptr), ID(0), currentState(InputState::NONE), run(false), updateTimer(0), timer(0),
      xCenterLabel(0), yCenterLabel(0), xPosition(0), yPosition(0), value(0),
      percentage(0), integer(0), width(0), height(0), xLabelPosition(0),
      yLabelPosition(0), labelInput("Delay") {}

ListLCD::ListLCD(U8G2_ST7920_128X64_F_SW_SPI *newu8g2) 
    : u8g2(newu8g2), ID(0), currentState(InputState::NONE), run(false), updateTimer(0), timer(0),
      xCenterLabel(0), yCenterLabel(0), xPosition(0), yPosition(0), value(0),
      percentage(0), integer(0), width(0), height(0), xLabelPosition(0),
      yLabelPosition(0), labelInput("Delay") {}

// Métodos para gestionar el ID
void ListLCD::setID(uint8_t newID) {
    this->ID = newID;
}

uint8_t ListLCD::getID() const {
    return this->ID;
}

// Métodos para gestionar el estado de ejecución
void ListLCD::setRun(bool newRun) {
    this->run = newRun;
}

bool ListLCD::getRun() const {
    return this->run;
}

// Métodos para gestionar los temporizadores
void ListLCD::setUpdateTimer(uint32_t newUpdateTimer) {
    this->updateTimer = newUpdateTimer;
}

uint32_t ListLCD::getUpdateTimer() const {
    return this->updateTimer;
}

void ListLCD::setTimer(uint32_t newTimer) {
    this->timer = newTimer;
}

uint32_t ListLCD::getTimer() const {
    return this->timer;
}

void ListLCD::setValue(uint8_t newValue) {
    this->value = newValue;
}

uint8_t ListLCD::getValue() const {
    return this->value;
}

// Métodos para gestionar la posición
void ListLCD::setPosition(uint8_t xpos, uint8_t ypos) {
    this->xPosition = xpos;
    this->yPosition = ypos;
}

void ListLCD::setLabelPosition(uint8_t newXlabelPosition, uint8_t newYlabelPosition) {
    this->xLabelPosition = newXlabelPosition;
    this->yLabelPosition = newYlabelPosition;
}

void ListLCD::drawLabelInput() {
    if (this->isShowed) {
        this->u8g2->drawStr(xLabelPosition, yLabelPosition, this->labelInput.c_str());  // Usar .c_str()
    }
}

void ListLCD::showLabelInput() {
    this->isLabelShowed = true;
}

void ListLCD::hideLabelInput() {
    this->isLabelShowed = false;
}

void ListLCD::setLabelInput(const std::string &newLabelInput) {
    this->labelInput = newLabelInput;  // Asignar directamente el std::string
}

const std::string& ListLCD::getLabelInput() const {
    return this->labelInput;  // Devolver una referencia constante
}

// Métodos para gestionar el estado (InputState)
void ListLCD::setState(InputState newState) {
    this->currentState = newState;
}

InputState ListLCD::getState() const {
    return this->currentState;
}

// Métodos para dibujar
void ListLCD::draw() {
    if (this->isShowed) {
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
                this->u8g2->setDrawColor(1);
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
        this->u8g2->setDrawColor(1);
    }
}

void ListLCD::show() {
    this->isShowed = true;
}

void ListLCD::hide() {
    this->isShowed = false;
}

// Métodos para gestionar el valor entero
void ListLCD::setInteger(uint32_t newInteger) {
    this->integer = newInteger;
}

uint32_t ListLCD::getInteger() const {
    return this->integer;
}

// Métodos para gestionar el ancho y alto
void ListLCD::setWidth(uint8_t newWidth) {
    this->width = newWidth;
}

uint8_t ListLCD::getWidth() const {
    return this->width;
}

void ListLCD::setHeight(uint8_t newHeight) {
    this->height = newHeight;
}

uint8_t ListLCD::getHeight() const {
    return this->height;
}

// Métodos para gestionar el porcentaje
void ListLCD::setPercentage(uint8_t newPercentage) {
    this->percentage = newPercentage;
}

uint8_t ListLCD::getPercentage() const {
    return this->percentage;
}

// Métodos para gestionar el centro del label
void ListLCD::setCenterLabel(uint8_t xCenter, uint8_t yCenter) {
    this->xCenterLabel = xCenter;
    this->yCenterLabel = yCenter;
}

uint8_t ListLCD::getXCenterLabel() const {
    return this->xCenterLabel;
}

uint8_t ListLCD::getYCenterLabel() const {
    return this->yCenterLabel;
}

void ListLCD::update(){
    this->draw();
    this->drawLabelInput();
}

void ListLCD::setListValue(int index , String name){
    
}