#pragma once

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "View.h"
#include "Race.h"

class MainMenu : public View {
    public:
        MainMenu() : View() {
            menuCursor = 0;
        }
    private:
        int menuCursor;
        int status;
        const String menuOptions [4] = {
            "Start race",
            "See best races",
            "Add player",
            "Remove player",
        };

        void draw() {
            lcd->clear();
            for (int i=0; i<4 ; i++) {
                lcd->setCursor(2, i);
                lcd->print(menuOptions[i]);
            }
            lcd->setCursor(0, menuCursor);
            lcd->print(">");
        }

        void onChange() {
            menuCursor = (menuCursor + 1) % 4;
        }

        void onEnter() {
            Serial.println("Entering state " + String(status));
            switch (status) {
                case 0:
                    View view = Race();
                    nextView = &view;
                case 1:
                    View view = Race();
                    nextView = &view;
            }
        }
};