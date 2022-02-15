#pragma once

#include "Arduino.h"
#include "View.h"
#include "../Model.h"

class SelectPlayer : public View {
    public:
        SelectPlayer(View &parent) : View() {
            this->parent = &parent;
            selectedPlayer = 0;
        }
    private:
        int selectedPlayer;

        void draw() {
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("Select player:");
            lcd->setCursor(0, 1);
            lcd->print(Model::players.get(selectedPlayer));
        }

        void onChange() {
            selectedPlayer = (selectedPlayer + 1) % Model::players.size();
        }

        void onEnter() {
            parent->player = selectedPlayer;
            nextView = parent;
        }
};