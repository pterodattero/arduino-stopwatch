#include "../Constants.h"
#include "../Utils.h"
#include "LiquidCrystal_I2C.h"

class View {
    public:
        View(LiquidCrystal_I2C* lcd) {
            refresh = true;
            lastRefreshTime = 0;

            this->lcd = lcd;
        }

        void update () {
            if (refreshInterval > 0) {
                const unsigned long time = millis(); 
                if (time > lastRefreshTime + refreshInterval) refresh = true;
            }
            if (refresh) {
                draw();
                refresh = false;
            }
            if (Utils::readButton(CHANGE_BUTTON)) {
                onChange();
                refresh = true;
            }
            if (Utils::readButton(ENTER_BUTTON)) {
                onEnter();
                refresh = true;
            }
            if (Utils::readLaser()) {
                onLaser();
                refresh = true;
            }
        }
        
    protected:
        const unsigned int refreshInterval = 0;
        static bool refresh;
        unsigned long lastRefreshTime;

        static LiquidCrystal_I2C* lcd;

        virtual void draw() = 0;
        virtual void onChange() {};
        virtual void onEnter() {};
        virtual void onLaser() {};

        void fottiti() {
            lcd->clear();
            lcd->setCursor(6, 1);
            lcd->print("Fottiti!");
            lcd->setCursor(2, 2);
            lcd->print("Pelato di merda!");
            delay(1000);
            lcd->clear();
            refresh = true;
        }
};