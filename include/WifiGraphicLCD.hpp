#ifndef _INC_WIFIGRAPHICLCD_
#define _INC_WIFIGRAPHICLCD_
#include <U8g2lib.h>
#include "Screens.hpp"

class WifiGraphicLCD {
    private:
        U8G2_ST7920_128X64_F_SW_SPI *u8g2;

        uint8_t ID;
        
        bool state;
        
        bool run;
        
        uint32_t updateTimer;
        
        uint32_t timer;

        uint8_t xCenterLabel,yCenterLabel;

        uint8_t xPosition,yPosition;

        char *labelText;

        bool selected = false;

        bool navigated = false;

        bool animated = false;

        bool labelState = false;

        bool iconState = false;
        
        IPAddress ip;
        char *ssid ;

        
        uint8_t xCenterLabelIp;
        uint8_t yCenterLabelIp;

    public:

        WifiGraphicLCD();

        WifiGraphicLCD(U8G2_ST7920_128X64_F_SW_SPI *lcd);

        void drawCenteredText(int xCenter, int y, const char *text);

        void drawRotatedImage(int xPos, int yPos, const Bitmap &image, float angle);

        void drawImage(int xPos, int yPos, const Bitmap &image);

        void setID(uint8_t newID);
        uint8_t getID();

        void setState(bool newState);
        bool getState();

        void setRun(bool newState);
        bool getRun();

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

        void setAnimation( bool newAnimation);
        void animate();
        void deanimate();

        void drawIp();
        void setIpPosition(uint8_t xpos , uint8_t ypos);
        void setPosition(uint8_t xpos , uint8_t ypos);

        void setSelected(bool isSelected) {
            this->selected = isSelected;
        }

        void setNavigated(bool isNavigated) {
            this->navigated = isNavigated;
        }
    
        bool isSelected() const {
            return this->selected;
        }
        
        bool isNavigated() const {
            return this->navigated;
        }
        void update();

        
        void setLabelText(IPAddress newIp);
};
#endif