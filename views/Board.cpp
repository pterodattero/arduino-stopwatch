#pragma once

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "View.h"
#include "Race.h"
#include "SelectPlayer.h"
#include "../Model.h"

class Board : public View {
    public:
        Board() {
            delete nextView;
            nextView = new SelectPlayer(this);
            player = 0;
        }
    private:
        int player;
        void draw() {
            showBoard(Model::getBoard(player));
        }

        void onChange() {
            delete nextView;
            nextView = new SelectPlayer(this);
        }

        void onEnter() {
            delete nextView;
            nextView = parent;
        }
};