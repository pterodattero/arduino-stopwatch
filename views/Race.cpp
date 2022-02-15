#pragma once

#include "Arduino.h"
#include "View.h"
#include "SelectPlayer.h"

class Race : public View {
    public:
        Race() : View(nullptr, 100) {
            nextView = SelectPlayer((View*)this);
        }
    private:
        int status;
        unsigned long time;

        void draw() {

        }

        void onChange() {
            
        }

        void onEnter() {
            
        }
};