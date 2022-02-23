#pragma once

#include "../Constants.h"
#include "../Model.h"
#include "Arduino.h"
#include "View.h"
#include "SelectPlayer.h"

class Race : public View {
    public:
        Race() : View(100) {
            status = READY;
            initBoard();
            delete nextView;
            nextView = new SelectPlayer(this);
        }
    private:
        int player;
        int currentPart;
        uint16_t currentBoard[BOARD_SIZE];
        enum Status { READY, RUNNING, FINISHED, NEW_RECORD };
        enum Status status;
        unsigned long startTime;
        boolean newRecord;

        void draw() {
            if (status == NEW_RECORD) {
                lcd->setCursor(1, 4);
                lcd->print("New record!!");
                lcd->setCursor(2, 3);
                lcd->print("Wanna save it?");
            } else {
                if (status == RUNNING) {
                    currentBoard[currentPart] = millis() - startTime;
                }
                showBoard(currentBoard, currentPart);
            }
        }

        void onChange() {
            switch (status) {
                case READY:
                    delete nextView;
                    nextView = new SelectPlayer(this);
                    break;
                case NEW_RECORD:
                case RUNNING:
                case FINISHED:
                    initBoard();
                    status = READY;
                    break;
            }
        }

        void onEnter() {
            if (status == NEW_RECORD) {
                Model::setBoard(player, currentBoard);
                status = FINISHED;
            } else {
                nextView = parent;
            }
        }

        void onLaser() {
            switch (status) {
                case READY:
                    startTime = millis();
                    status = RUNNING;
                    break; 
                case RUNNING:
                    currentBoard[currentPart] = millis();
                    currentPart++;
                    if (currentPart == BOARD_SIZE) {
                        laser = false;
                        if (currentBoard[BOARD_SIZE-1] < Model::getBoard(player)[BOARD_SIZE-1]) status = NEW_RECORD;
                        else status = FINISHED;
                    }
                    break; 
                default:
                    break; 
            }
        }

        void initBoard() {
            currentPart = 0;
            for (int i=0; i<BOARD_SIZE; i++) {
                currentBoard[i] = 0;
            };
        }
};