#pragma once

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "View.h"
#include "Race.h"
#include "SelectPlayer.h"
#include "../Model.h"

class Board : public View {
    public:
        Board() : View() {
            if ( Model::getPlayersSize() >= 10 ) {
                nextView = parent;
                lcd->clear();
                lcd->setCursor(4, 1);
                lcd->print("Memory full!");
                delay(1000);
            }
            playerName = "";
            typingCursor = 0;
            currentLetter = 0;
        }
    private:
        String playerName;
        int typingCursor;
        int currentLetter;

        const char letters [28] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        void draw() {
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("Enter player name:");
            lcd->setCursor(0, 1);
            lcd->print(playerName);
            
            lcd->setCursor(typingCursor, 1);
            lcd->print(letters[currentLetter]);
            lcd->setCursor(typingCursor, 2);
            lcd->print("^");
        }

        void onChange() {
            currentLetter = (currentLetter + 1) % 27;
        }

        void onEnter() {
            playerName[typingCursor] = letters[currentLetter];
            currentLetter = 0;
            typingCursor++;

            if (typingCursor >= 20) {
                Model::addPlayer(playerName);

                lcd->clear();
                lcd->setCursor(4, 1);
                lcd->print("Player added");
                delay(1000);
                nextView = parent;
            }
        }
};