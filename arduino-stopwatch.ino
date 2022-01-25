#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <LinkedList.h>
#include <EEPROM.h>

#define CHANGE_BUTTON 2
#define ENTER_BUTTON 3
#define LASER 14
#define DILDO A3

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
    SHOW_BOARD,
};
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
const String boardLabels [4] = {
    "PT1",
    "PT2",
    "PT3",
    "END",
};
uint16_t currentBoard [4] = {};
int unsigned currentPart = 0;
long unsigned startTime;
long unsigned raceLastRefreshTime = 0;


// Button variables
long unsigned buttonLastActivation = 0;
const int buttonInactivityTime = 200;
long unsigned laserLastActivation = 0;
const int laserInactivityTime = 1000;
int dildoThresh;


void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();

    splashScreen();

    pinMode(CHANGE_BUTTON, INPUT);
    pinMode(ENTER_BUTTON, INPUT);
    pinMode(LASER, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    loadData();

    calibrateLaser();
}

void loop() {
    switch (mainState) {
        case MAIN_MENU:
            mainMenu();
            break;
        case BOARD:
            board();
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
        delay(1000);
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
            
            if (readLaser()) {
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
            if (time > raceLastRefreshTime + 250) {
                currentBoard[currentPart] = time - startTime;
                showBoard(currentBoard);
                raceLastRefreshTime = time;
            }
            
            if (readLaser()) {
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
            if (readButton(CHANGE_BUTTON)) {
                raceState = READY_RACE;
                currentPart = 0;
                for (int i=0; i<4; i++) {
                    currentBoard[i] = 0;
                }
            }
            if (readButton(ENTER_BUTTON)) {
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
                boardState = READY_RACE;
            };
            break;
        case SHOW_BOARD:
            if (refresh) {
                uint16_t* board = boards[currentPlayer];
                showBoard(board);
                refresh = false;
            }
            if (readButton(CHANGE_BUTTON)) {
                boardState = READY_RACE;
                refresh = true;
            }
            if (readButton(ENTER_BUTTON)) {
                mainState = MAIN_MENU;
                boardState = SELECT_PLAYER;
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
void showBoard(uint16_t* board) {
    lcd.clear();
    for (int i=0; i<4; i++) {
        lcd.setCursor(0, i);
        lcd.print(boardLabels[i]);
        String ss = String(board[i] / 1000);
        String ms = String(board[i] % 1000);
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
    if (analogRead(DILDO) < dildoThresh & time > laserLastActivation + laserInactivityTime) {
        laserLastActivation = time;
        refresh = true;

        // Safety switch
        digitalWrite(LASER, LOW);
        Serial.println("Laserrrrrrrrrrrrrrrrr");
        return true;
    }

    if (time > laserLastActivation + laserInactivityTime - 30) {
        digitalWrite(LASER, HIGH);
    }
    return false;
}

// Memory methods
String readPlayerName(int player) {
    if (player < 0 || player >= 10) {
        return "";
    }

    String name;
    for (int i=0; i<20; i++) {        
        const int offset = player * 28;
        name += (char)EEPROM.read(offset + i);
    }

    return name;
}

uint16_t* readPlayerBoard(int player) {
    static uint16_t emptyBoard [4];
    if (player < 0 || player >= 10) {
        return emptyBoard;
    }

    static uint16_t board [4];
    for (int part=0; part<4; part++) {
        const int offset = player * 28 + 20;
        uint8_t firstOctet = (uint8_t)EEPROM.read(offset + 2 * part);
        uint8_t secondOctet = (uint8_t)EEPROM.read(offset + 2 * part + 1);
        board[part] = firstOctet * 256 + secondOctet;
    }

    return board;
}

void writePlayerName(int player) {
    if (player >= 0 & player < 10) {
        String name = players.get(player);

        const int offset = player * 28;
        for (int i=0; i<20 & i<name.length(); i++) {
            EEPROM.put(offset + i, (uint8_t)name[i]);
        }
    }
}

void writePlayerBoard(int player, uint16_t* board) {
    if (player >= 0 & player < 10) {
        String name = players.get(player);

        for (int part=0; part<4; part++) {
            const int offset = player * 28 + 20;
            uint8_t firstOctet = board[part] / 256;
            uint8_t secondOctet = board[part] % 256;
            EEPROM.put(offset + 2 * part, firstOctet);
            EEPROM.put(offset + 2 * part + 1, secondOctet);
        }
    }
}

void loadData() {
    for (int i=0; i<10; i++) {
        String name = readPlayerName(i);
        if (name == "                    ") break;
        players.add(i, name);

        uint16_t* board = readPlayerBoard(i);
        boards.add(i, board);
    }
}

void writeData() {
    for (int i=0; i<players.size(); i++) {
        writePlayerName(i);
        writePlayerBoard(i, boards[i]);
    }
}


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

void calibrateLaser() {
    delay(1000);
    digitalWrite(LASER, HIGH);
    delay(100);
    int lowLight = analogRead(DILDO);
    digitalWrite(LASER, LOW);
    delay(100);
    int highLight = analogRead(DILDO);
    
    dildoThresh = (highLight + lowLight) / 2;
    refresh = true;
}
