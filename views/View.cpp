#include "../Constants.h"
#include "../Utils.h"
#include "LiquidCrystal_I2C.h"

class View {
    public:
        View(int refreshInterval = 0) {
            refresh = true;
            lastRefreshTime = 0;
            this->refreshInterval = refreshInterval;

            nextView = this;
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
            if (Utils::readLaser(laser)) {
                onLaser();
                refresh = true;
            }
            if (nextView != this) return nextView;
            return this;
        }

        static void setLcd(LiquidCrystal_I2C* const& lcd) {
            View::lcd = lcd;
            lcd->init();
            lcd->backlight();
        }

        static void splashScreen() {
            showMessage({
                "<================8",
                "Gentilmente,",
                "levati dal cazzo",
                "8================>"
            });
        }
        
    protected:
        unsigned int refreshInterval = 0;
        static bool refresh;
        unsigned long lastRefreshTime;
        bool laser;

        static LiquidCrystal_I2C* lcd;

        View* nextView;

        virtual void draw() = 0;
        virtual void onChange() {};
        virtual void onEnter() {};
        virtual void onLaser() {};

        void fottiti() {
            showMessage({
                "Fottiti!",
                "Pelato di merda!"
            });
        }

        void showBoard(uint16_t* board, int part=BOARD_SIZE-1) {
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

        static void showMessage(String lines[], int timer=1000) {
            int nLines = *(&lines + 1) - lines;
            int startY = (4 - nLines) / 2;
            for (int i=0; i<nLines; i++) {
                int startX = (20 - lines[i].length()) / 2;
                lcd->setCursor(startX, startY);
                lcd->print(lines[i]);
            }
            delay(timer);
        }
};