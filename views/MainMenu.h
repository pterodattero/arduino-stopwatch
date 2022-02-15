#pragma once

#include "Arduino.h"
#include "View.h"

class MainMenu : public View {
    public:
        MainMenu() {
            menuCursor = 0;
            children[RACE] = new MainMenu();
        }
    private:
        const enum Status {
            RACE = 0,
            BOARD = 1,
            ADD_PLAYER = 2,
            REMOVE_PLAYER = 3,
            MAIN_MENU,
        };
        const String menuOptions [4] = {
            "Start race",
            "See best races",
            "Add player",
            "Remove player",
        };
        const View* children[4];
        View* currentView = this;
        int menuCursor;
        enum Status status;

        void draw() {
            switch (status) {
                case MAIN_MENU:
                    drawMenu();
                    break;
                default:
                    children[status]->update();
            }
        }

        void onChange() {
            menuCursor = (menuCursor + 1) % 4;
        }

        void onEnter() {
            Serial.println("Entering state " + String(status));
                switch (menuCursor) {
                    case 0:
                        status = RACE;
                        break;
                    case 1:
                        status = BOARD;
                        break;
                    case 2:
                        status = ADD_PLAYER;
                        break;
                    case 3:
                        status = REMOVE_PLAYER;
                        break;
                    default:
                        status = MAIN_MENU;
                }
        }

        void drawMenu() {
            lcd->clear();
            for (int i=0; i<4 ; i++) {
                lcd->setCursor(2, i);
                lcd->print(menuOptions[i]);
            }
            lcd->setCursor(0, menuCursor);
            lcd->print(">");
        }
};