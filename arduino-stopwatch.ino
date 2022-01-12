#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>

#define CHANGE_BUTTON 2
#define ENTER_BUTTON 3

LiquidCrystal_I2C lcd(0x27, 20, 4);
bool refresh = true;

// Main state
enum states {
    MAIN_MENU,
    RACE,
    BOARD,
    ADD_PLAYER,
    REMOVE_PLAYER,
};
enum states mainState = MAIN_MENU;

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

std::list<String> players;

// Button variables
long unsigned buttonLastActivation = 0;
const int buttonInactivityTime = 200;


void setup()
{
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();

    pinMode(CHANGE_BUTTON, INPUT);
    pinMode(ENTER_BUTTON, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    switch (mainState) {
        case MAIN_MENU:
            mainMenu();
            break;
        case ADD_PLAYER:
            addPlayer();
            break;
        default:
            fottiti();
    }
}


void mainMenu()
{
    bool exit = false;
    if (readButton(CHANGE_BUTTON)) {
        menuCursor = (menuCursor + 1) % 4;
    }

    if (readButton(ENTER_BUTTON)) {
        Serial.println("Entering state " + mainState);
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
        exit = true;
    }

    if (refresh & !exit) {
        lcd.clear();
        for (int i=0; i<4 ; i++) {
            lcd.setCursor(2, i);
            lcd.print(menuOptions[i]);
        }
        lcd.setCursor(0, menuCursor);
        lcd.print(">");
        refresh = false;
    }
}

void drawOptions() {
    for (int i=0; i<4 ; i++) {
        lcd.setCursor(2, i);
        lcd.print(menuOptions[i]);
    }
}

void drawCursor() {
    for (int i=0; i<4 ; i++) {
        lcd.setCursor(0, i);
        if (i==menuCursor) {
            lcd.print(">");
        } else {
            lcd.print(" ");
        }
    }
}

bool readButton(int buttonPin) {
    const long unsigned time = millis(); 
    if (digitalRead(buttonPin) & time > buttonLastActivation + buttonInactivityTime) {
        buttonLastActivation = time;
        refresh = true;
        Serial.println("Pressed button " + buttonPin);
        return true;
    }
    return false;
}

void fottiti() {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("Fottiti!");
    lcd.setCursor(2, 2);
    lcd.print("Pelato di merda!");
    delay(5000);
    mainState = MAIN_MENU;
    lcd.clear();
    refresh = true;
}

void addPlayer() {
    bool exit = false;
    
    if (players.size() >= 10) {
        mainState = MAIN_MENU;
        lcd.clear();
        lcd.setCursor(4, 1);
        lcd.print("Memory full!");
        exit = true;
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
            players.push_back(newPlayer);
            lcd.clear();
            lcd.setCursor(4, 1);
            lcd.print("Player added");
            delay(2000);
            mainState = MAIN_MENU;
            exit = true;
            for (int i=0; i<20; i++) {
                newPlayer[i] = ' ';
            }
        }
    }

    if (refresh && !exit) {
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
}
