#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <LinkedList.h>

#define CHANGE_BUTTON 2
#define ENTER_BUTTON 3
#define LASER 14

LiquidCrystal_I2C lcd(0x27, 20, 4);
bool refresh = true;

// Main state
enum states {
    MAIN_MENU,
    RACE,
    BOARD,
    ADD_PLAYER,
    REMOVE_PLAYER,
    SELECT_PLAYER,
    CONFIRM_PLAYER,
    READY_RACE,
    RECORD_RACE,
    FINISH_RACE,
};
enum states mainState = MAIN_MENU;
enum states removeState = SELECT_PLAYER;
enum states raceState = SELECT_PLAYER;

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

// Race variables
const String boardLabels [4] = {
    "PT1",
    "PT2",
    "PT3",
    "END",
};
int unsigned currentBoard [4] = {};
int unsigned currentPart = 0;
long unsigned startTime;
long unsigned raceLastRefreshTime = 0;


// Button variables
long unsigned buttonLastActivation = 0;
const int buttonInactivityTime = 200;
long unsigned laserLastActivation = 0;
const int laserInactivityTime = 1000;


void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();

    pinMode(CHANGE_BUTTON, INPUT);
    pinMode(ENTER_BUTTON, INPUT);
    pinMode(LASER, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    players.add("CAZZO MOLLE");
    players.add("CAZZO DURO");
    players.add("CAZZO MARMOREO");
}

void loop() {
    switch (mainState) {
        case MAIN_MENU:
            mainMenu();
            break;
        case ADD_PLAYER:
            addPlayer();
            break;
        case REMOVE_PLAYER:
            removePlayer();
            break;
        case RACE:
            race();
            break;
        default:
            fottiti();
    }
}

// Views
void mainMenu() {
    if (refresh) {
        lcd.clear();
        for (int i=0; i<4 ; i++) {
            lcd.setCursor(2, i);
            lcd.print(menuOptions[i]);
        }
        lcd.setCursor(0, menuCursor);
        lcd.print(">");
        refresh = false;
    }
    if (readButton(CHANGE_BUTTON)) {
        menuCursor = (menuCursor + 1) % 4;
    }

    if (readButton(ENTER_BUTTON)) {
        Serial.println("Entering state " + String(mainState));
        switch (menuCursor) {
            case 0:
                mainState = RACE;
                break;
            case 1:
                mainState = BOARD;
                break;
            case 2:
                mainState = ADD_PLAYER;
                break;
            case 3:
                mainState = REMOVE_PLAYER;
                break;
            default:
                mainState = MAIN_MENU;
        }
    }
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
        refresh = true;
    }
    
    if (readButton(CHANGE_BUTTON)) {
        currentLetter = (currentLetter + 1) % 27;
    }

    if (readButton(ENTER_BUTTON)) {
        newPlayer[typingCursor] = letters[currentLetter];
        currentLetter = 0;
        typingCursor++;

        if (typingCursor >= 20) {
            typingCursor = 0;
            players.add(newPlayer);
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
            
            if (readButton(CHANGE_BUTTON)) {
                mainState = MAIN_MENU;
                removeState = SELECT_PLAYER;
                currentPlayer = 0;
                
                lcd.clear();
                lcd.setCursor(1, 1);
                lcd.print("Operation canceled");
                refresh = true;
                delay(1000);
            }

            if (readButton(ENTER_BUTTON)) {
                players.remove(currentPlayer);
                
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
                showBoard();
                refresh = false;
            }
            
            //if (readLaser()) {
            if (readButton(CHANGE_BUTTON)) {
                raceState = RECORD_RACE;
                startTime = millis();
            }
            if (readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                raceState = SELECT_PLAYER;
                refresh = true;
            }
            break;
        case RECORD_RACE:
            time = millis(); 
            if (time > raceLastRefreshTime + 100) {
                currentBoard[currentPart] = time - startTime;
                showBoard();
                raceLastRefreshTime = time;
            }
            
            //if (readLaser()) {
            if (readButton(CHANGE_BUTTON)) {
                currentBoard[currentPart] = time - startTime;
                currentPart++;
                if (currentPart >= 4) {
                    raceState = FINISH_RACE;
                    refresh = true;
                }
            }
            if (readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                raceState = SELECT_PLAYER;
                refresh = true;
            }
            break;
        case FINISH_RACE:
            digitalWrite(LASER, LOW);
            if (refresh) {
                showBoard();
                refresh = false;
                Serial.println("Porco dio");
            }
            if (readButton(CHANGE_BUTTON)) {
                raceState = READY_RACE;
                currentPart = 0;
                for (int i=0; i<4; i++) {
                    currentBoard[i] = 0;
                }
                refresh = true;
            }
            if (readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                raceState = SELECT_PLAYER;
                currentPart = 0;
                for (int i=0; i<4; i++) {
                    currentBoard[i] = 0;
                }
                refresh = true;
            }
            break;
        default:
            fottiti();
    }
}

void fottiti() {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("Fottiti!");
    lcd.setCursor(2, 2);
    lcd.print("Pelato di merda!");
    delay(1000);
    mainState = MAIN_MENU;
    lcd.clear();
    refresh = true;
}


// UTILS
void showBoard() {
    lcd.clear();
    for (int i=0; i<4; i++) {
        lcd.setCursor(0, i);
        lcd.print(boardLabels[i]);
        String ss = String(currentBoard[i] / 1000);
        String ms = String(currentBoard[i] % 1000);
        while (ss.length() < 2) {
            ss = '0' + ss;
        }
        while (ms.length() < 3) {
            ms = '0' + ms;
        }
        lcd.setCursor(4, i);
        lcd.print(ss + ':' + ms);
    }
}

/*void showBoard(bool lazy=false) {
    if (!lazy) lcd.clear();
    for (int i=0; i<4; i++) {
        if (!lazy) {
            lcd.setCursor(0, i);
            lcd.print(boardLabels[i]);
        }
        if (i <= currentPart | (lazy & i==currentPart)) {
            String ss = String(currentBoard[i] / 1000);
            String ms = String(currentBoard[i] % 1000);
            while (ss.length() < 2) {
                ss = '0' + ss;
            }
            while (ms.length() < 2) {
                ms = '0' + ms;
            }
            lcd.setCursor(4, i);
            lcd.print(ss + ':' + ms);
        }
    }
}*/

bool selectPlayer() {
    if (refresh) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Select player:");
        lcd.setCursor(0, 1);
        lcd.print(players.get(currentPlayer));
        refresh = false;
    }
    
    if (readButton(CHANGE_BUTTON)) {
        currentPlayer = (currentPlayer + 1) % players.size();
    }

    if (readButton(ENTER_BUTTON)) {
        return true;
    }

    return false;
}

bool readButton(int buttonPin) {
    const long unsigned time = millis(); 
    if (digitalRead(buttonPin) & time > buttonLastActivation + buttonInactivityTime) {
        buttonLastActivation = time;
        refresh = true;
        Serial.println("Pressed button " + String(buttonPin));
        return true;
    }
    return false;
}

bool readLaser() {
    const long unsigned time = millis(); 
    if (digitalRead(LASER) & time > laserLastActivation + laserInactivityTime) {
        buttonLastActivation = time;
        refresh = true;
        Serial.println("Laserrrrrrrrrrrrrrrrr");
        return true;
    }
    // SAFETY SWITCH TO IMPLEMENT
    return false;
}