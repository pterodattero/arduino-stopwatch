#include "Arduino.h"
#include "Constants.h"

class Utils {
    public:
        static bool readButton(const int buttonPin) {
            const long unsigned time = millis(); 
            if (digitalRead(buttonPin) & time > buttonLastActivation + buttonInactivityTime) {
                buttonLastActivation = time;
                Serial.println("Pressed button " + String(buttonPin));
                return true;
            }
            return false;
        }

        static bool readLaser(bool active=true) {
            if (!active) {
                digitalWrite(LASER, LOW);
                return false;
            }
            const long unsigned time = millis(); 
            if (time > laserLastActivation + laserInactivityTime - laserUnsafeOverlap) {
                digitalWrite(LASER, HIGH);
            } else {
                digitalWrite(LASER, LOW);
            }

            if (analogRead(DILDO) < dildoThresh & time > laserLastActivation + laserInactivityTime) {
                laserLastActivation = time;

                // Safety switch
                digitalWrite(LASER, LOW);
                return true;
            }

            return false;
        }

        static void calibrateLaser() {
            digitalWrite(LASER, HIGH);
            delay(100);
            int lowLight = analogRead(DILDO);
            
            digitalWrite(LASER, LOW);
            delay(100);
            int highLight = analogRead(DILDO);
            
            dildoThresh = (highLight + lowLight) / 2;
        }

        static void initIO() {
            pinMode(CHANGE_BUTTON, INPUT);
            pinMode(ENTER_BUTTON, INPUT);
            pinMode(LASER, OUTPUT);
            pinMode(LED_BUILTIN, OUTPUT);
        }

    private:
        static unsigned long buttonLastActivation;
        static const unsigned int buttonInactivityTime = 200;

        static long unsigned laserLastActivation;
        static const int laserInactivityTime = 1000;
        static const int laserUnsafeOverlap = 100;

        static int dildoThresh;
};