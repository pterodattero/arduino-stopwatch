#pragma once

#include "../Utils.h"
#include "LiquidCrystal_I2C.h"

class View {
    public:
        View(LiquidCrystal_I2C* lcd=nullptr, int refreshInterval = 0);
        View* update();

        int player;
    protected:
        virtual void onChange();
        virtual void onEnter();
        virtual void onLaser();
        virtual void draw();

        void fottiti();
        
        static bool refresh;
        static LiquidCrystal_I2C* lcd;
        View* nextView;
        View* parent;
};
