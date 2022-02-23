#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "LinkedList.h"
#include "EEPROM.h"

#include "Model.h"
#include "Utils.h"
#include "views/View.h"
#include "views/MainMenu.h"

View* currentView;

void setup() {
    Serial.begin(9600);

    LiquidCrystal_I2C lcd(0x27, 20, 4);
    View::setLcd(&lcd);
    View::splashScreen();

    Utils::initIO();
    Utils::calibrateLaser();

    Model::loadData();

    currentView = new MainMenu();
}

void loop() {
    currentView = currentView->update();
}
