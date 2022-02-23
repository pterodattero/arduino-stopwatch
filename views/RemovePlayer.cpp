#pragma once

#include "Arduino.h"
#include "View.h"
#include "SelectPlayer.h"
#include "../Model.h"

class RemovePlayer : public View {
    public:
        RemovePlayer() {
            delete nextView;
            nextView = new SelectPlayer(this);
            player = 0;
        }
    private:
        int player;
        void draw() {
            showMessage({ "Are you sure?" }, 0);
        }

        void onChange() {
            showMessage({ "Operation canceled" });
            nextView = parent;
        }

        void onEnter() {
            Model::removePlayer(player)
            showMessage({ "Player deleted" });
            nextView = parent;
        }
};