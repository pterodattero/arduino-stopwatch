#pragma once

#include "../Constants.h"
#include "LiquidCrystal_I2C.h"

class View {
    public:
        View(int refreshInterval=0);
        View* update();
        
        static void setLcd(LiquidCrystal_I2C* lcd);
        static void splashScreen();

        int player;
    protected:
        virtual void onChange();
        virtual void onEnter();
        virtual void onLaser();
        virtual void draw();

        void fottiti();
        void showBoard(uint16_t* board, int part=BOARD_SIZE-1);
        void showMessage(String* lines, int timer=1000);
        
        static bool refresh;
        static bool laser;
        static LiquidCrystal_I2C* lcd;
        View* nextView;
        View* parent;
};
