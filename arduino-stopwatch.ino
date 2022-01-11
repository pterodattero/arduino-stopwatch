#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Main state
enum states {
    MAIN_MENU,
    RACE,
    BOARD,
    ADD_PLAYER,
    REMOVE_PLAYER_CHOOSE,
    REMOVE_PLAYER_CONFIRM,
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

void setup()
{
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
}

// the loop function runs over and over again forever
void loop()
{
    switch (mainState) {
        case MAIN_MENU:
            mainMenu();
            break;
        default:
            fottiti();
    }
}

void mainMenu()
{
    if (buttonChange()) {
        menuCursor = (menuCursor + 1) % 4;
    }

    if (buttonEnter()) {
        switch (menuCursor) {
            case 0:
                mainState = RACE;
                break;
            case 1:
                mainState = RACE;
                break;
            case 2:
                mainState = RACE;
                break;
            case 3:
                mainState = RACE;
                break;
            default:
                mainState = MAIN_MENU;
        }
    }

    drawOptions();
    drawCursor();

    delay(2000);
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

bool buttonChange() {
    return true;
}

bool buttonEnter() {
    if (millis() > 5000 && millis() < 6000) {
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
}