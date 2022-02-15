#include "../Constants.h"
#include "../Utils.h"
#include "LiquidCrystal_I2C.h"

class View {
    public:
        View(LiquidCrystal_I2C* lcd, int refreshInterval = 0) {
            refresh = true;
            lastRefreshTime = 0;
            this->refreshInterval = refreshInterval;

            this->lcd = lcd;
            nextView = this;

            digitalWrite(LASER, LOW);
        }

        View* update () {
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
            if (nextView != this) return nextView;
            return this;
        }
        
    protected:
        unsigned int refreshInterval = 0;
        static bool refresh;
        unsigned long lastRefreshTime;

        static LiquidCrystal_I2C* lcd;

        View* nextView;

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

        void showBoard(uint16_t* board) {
            const String boardLabels [4] = {
                "PT1",
                "PT2",
                "PT3",
                "END",
            };
            lcd->clear();
            for (int i=0; i<4; i++) {
                lcd->setCursor(0, i);
                lcd->print(boardLabels[i]);
                String ss = String(board[i] / 1000);
                String ms = String(board[i] % 1000);
                while (ss.length() < 2) {
                    ss = '0' + ss;
                }
                while (ms.length() < 3) {
                    ms = '0' + ms;
                }
                lcd->setCursor(4, i);
                lcd->print(ss + ':' + ms);
            }
        }
};