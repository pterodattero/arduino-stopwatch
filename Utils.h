#pragma once

class Utils {
    public:
        static bool readButton(const int buttonPin);
        static bool readLaser();
        static void calibrateLaser();

    private:
        static unsigned long buttonLastActivation;
        static const unsigned int buttonInactivityTime;
        static long unsigned laserLastActivation;
        static const int laserInactivityTime;
        static int dildoThresh;
};
