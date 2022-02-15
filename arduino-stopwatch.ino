#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "LinkedList.h"
#include "EEPROM.h"

#include "Utils.h"
#include "Constants.h"
#include "views/MainMenu.h"

enum states mainState = MAIN_MENU;
enum states removeState = SELECT_PLAYER;
enum states raceState = SELECT_PLAYER;
enum states boardState = SELECT_PLAYER;

// Main menu variables
int menuCursor = 0;
String menuOptions [4] = {
    "Start race",
    "See best races",
    "Add player",
    "Remove player",
};

// Player variables
char newPlayer [20] = "";
int typingCursor = 0;
const char letters [28] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int currentLetter = 0;
int currentPlayer = 0;

LinkedList<String> players;
LinkedList<uint16_t*> boards;

// Race variables
uint16_t currentBoard [4] = {};
int unsigned currentPart = 0;
long unsigned startTime;


void setup() {
    Serial.begin(9600);

    LiquidCrystal_I2C lcd(0x27, 20, 4);
    lcd.init();
    lcd.backlight();

    splashScreen();

    pinMode(CHANGE_BUTTON, INPUT);
    pinMode(ENTER_BUTTON, INPUT);
    pinMode(LASER, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    loadData();

    Utils::calibrateLaser();
    currentView = MainMenu(&lcd);
}

void loop() {
    currentView = currentView.update();
}

void addPlayer() {
    if (refresh) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter player name:");
        lcd.setCursor(0, 1);
        lcd.print(newPlayer);
        
        lcd.setCursor(typingCursor, 1);
        lcd.print(letters[currentLetter]);
        lcd.setCursor(typingCursor, 2);
        lcd.print("^");
        refresh = false;
    }
    
    if (players.size() >= 10) {
        mainState = MAIN_MENU;
        lcd.clear();
        lcd.setCursor(4, 1);
        lcd.print("Memory full!");
        delay(1000);
        refresh = true;
    }
    
    if (Utils::readButton(CHANGE_BUTTON)) {
        currentLetter = (currentLetter + 1) % 27;
    }

    if (Utils::readButton(ENTER_BUTTON)) {
        newPlayer[typingCursor] = letters[currentLetter];
        currentLetter = 0;
        typingCursor++;

        if (typingCursor >= 20) {
            typingCursor = 0;
            players.add(newPlayer);
            uint16_t emptyBoard [4] = {};
            boards.add(emptyBoard);
            writeData();

            lcd.clear();
            lcd.setCursor(4, 1);
            lcd.print("Player added");
            delay(1000);
            mainState = MAIN_MENU;
            for (int i=0; i<20; i++) {
                newPlayer[i] = ' ';
            }
        }
    }
}

void removePlayer() {
    switch (removeState) {
        case SELECT_PLAYER:
            if (selectPlayer()) {
                removeState = CONFIRM_PLAYER;
            };
            break;
        case CONFIRM_PLAYER:
            if (refresh) {
                lcd.clear();
                lcd.setCursor(4, 1);
                lcd.print("Are you sure?");
                refresh = false;
            }
            
            if (Utils::readButton(CHANGE_BUTTON)) {
                mainState = MAIN_MENU;
                removeState = SELECT_PLAYER;
                currentPlayer = 0;
                
                lcd.clear();
                lcd.setCursor(1, 1);
                lcd.print("Operation canceled");
                refresh = true;
                delay(1000);
            }

            if (Utils::readButton(ENTER_BUTTON)) {
                players.remove(currentPlayer);
                boards.remove(currentPlayer);
                writeData();
                
                mainState = MAIN_MENU;
                removeState = SELECT_PLAYER;
                currentPlayer = 0;
                
                lcd.clear();
                lcd.setCursor(3, 1);
                lcd.print("Player deleted");
                refresh = true;
                delay(1000);
            }
            break;
        default:
            fottiti();
    }
}

void race() {
    long unsigned time;
    switch (raceState) {
        case SELECT_PLAYER:
            if (selectPlayer()) {
                raceState = READY_RACE;
            };
            break;
        case READY_RACE:
            if (refresh) {
                digitalWrite(LASER, HIGH);
                showBoard(currentBoard);
                refresh = false;
            }
            
            if (Utils::readLaser()) {
                raceState = RECORD_RACE;
                startTime = millis();
            }
            if (Utils::readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                raceState = SELECT_PLAYER;
                refresh = true;
            }
            break;
        case RECORD_RACE:
            time = millis(); 
            if (time > raceLastRefreshTime + 250) {
                currentBoard[currentPart] = time - startTime;
                showBoard(currentBoard);
                raceLastRefreshTime = time;
            }
            
            if (Utils::readLaser()) {
                currentBoard[currentPart] = time - startTime;
                currentPart++;
                if (currentPart >= 4) {
                    raceState = FINISH_RACE;
                    refresh = true;
                }
            }
            if (Utils::readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                raceState = SELECT_PLAYER;
                refresh = true;
            }
            break;
        case FINISH_RACE:
            // This will be executed only once thanks to the strict inequality
            if (currentBoard[3] < boards[currentPlayer][3]) {
                boards[currentPlayer] = currentBoard;
                writePlayerBoard(currentPlayer, currentBoard);
            }
            digitalWrite(LASER, LOW);
            if (refresh) {
                showBoard(currentBoard);
                refresh = false;
            }
            if (Utils::readButton(CHANGE_BUTTON)) {
                raceState = READY_RACE;
                currentPart = 0;
                for (int i=0; i<4; i++) {
                    currentBoard[i] = 0;
                }
            }
            if (Utils::readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                raceState = SELECT_PLAYER;
                currentPart = 0;
                for (int i=0; i<4; i++) {
                    currentBoard[i] = 0;
                }
            }
            break;
        default:
            fottiti();
    }
}

void board() {
    switch (boardState) {
        case SELECT_PLAYER:
            if (selectPlayer()) {
                boardState = SHOW_BOARD;
            };
            break;
        case SHOW_BOARD:
            if (refresh) {
                uint16_t* board = boards[currentPlayer];
                showBoard(board);
                refresh = false;
            }
            if (Utils::readButton(CHANGE_BUTTON)) {
                boardState = SELECT_PLAYER;
                refresh = true;
            }
            if (Utils::readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                boardState = SELECT_PLAYER;
                refresh = true;
            }
            break;
        default:
            fottiti();
    }
}


// UTILS



// Memory methods



// Bonus

void splashScreen() {
    lcd.setCursor(0, 0);
    lcd.print(" <================3 ");
    lcd.setCursor(0, 1);
    lcd.print("    Gentilmente,    ");
    lcd.setCursor(0, 2);
    lcd.print("  levati dal cazzo  ");
    lcd.setCursor(0, 3);
    lcd.print(" E================> ");
}

