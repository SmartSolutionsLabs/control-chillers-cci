#ifndef _INC_PROGRESSBAR_
#define _INC_PROGRESSBAR_
#include <U8g2lib.h>
#include "Screens.hpp"
#include "textInputLCD.hpp"

class progressBarLCD {
    private:
        U8G2_ST7920_128X64_F_SW_SPI *u8g2;

        textInputLCD *textInputDelay;

        uint8_t ID;
        
        bool state;
        
        bool run;
        
        uint8_t width;

        uint32_t updateTimer;
        
        uint32_t timer;

        uint8_t xCenterLabel,yCenterLabel;

        uint8_t xPosition,yPosition;

        uint16_t value,percentage;

        uint32_t counter;

        bool selected,navigated,iconState;

        bool showLblState,showTxtInput;

    public:

        progressBarLCD();

        progressBarLCD(U8G2_ST7920_128X64_F_SW_SPI *lcd);

        textInputLCD* getTextInput() { return this->textInputDelay; }
        
        void drawCenteredText(int xCenter, int y, const char *text);

        void drawRotatedImage(int xPos, int yPos, const Bitmap &image, float angle);

        void drawImage(int xPos, int yPos, const Bitmap &image);

        void setID(uint8_t newID);
        uint8_t getID();

        void setState(bool newState);
        bool getState();

        void setRun(bool newState);
        bool getRun();

        void setValue(uint16_t newValue);
        uint16_t getValue();

        void incrementCounter();
        void resetCounter() ;
        uint32_t getCounter() const ;
        void setCounter(uint32_t newCounter);

        void setPercentage(uint8_t newPercentage);
        uint8_t getPercentage();

        void setUpdateTimer(uint32_t newUpdateTimer);
        uint32_t getUpdateTimer();

        void setTimer(uint32_t newTimer);
        uint32_t getTimer();

        void drawIcon();
        void showIcon();
        void hideIcon();

        void setLabelPosition(uint8_t xCenter , uint8_t yCenter);
        void drawLabelState();
        void showLabelState();
        void hideLabelState();

        void animate();

        void setPosition(uint8_t xpos , uint8_t ypos);

        void drawTextInput();
        void showTextInput();
        void hideTextInput();
        void showLabelInput();
        void hideLabelInput();

        void setSelected(bool isSelected);
        bool isSelected() const;
        void setNavigated(bool isNavigated);
        bool isNavigated() const;

        void update();
};

#endif