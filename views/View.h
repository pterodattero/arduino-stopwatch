#pragma once

#include "../Utils.h"
#include "LiquidCrystal_I2C.h"

class View {
    public:
        void update();
    protected:
        virtual void onChange();
        virtual void onEnter();
        virtual void onLaser();
        virtual void draw();

        void fottiti();
        
        static bool refresh;
        static LiquidCrystal_I2C* lcd;
};