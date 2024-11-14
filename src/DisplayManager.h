#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H
#include <Adafruit_SSD1306.h>

class DisplayManager {
    public:
        DisplayManager(uint8_t SCREEN_WIDTH, uint8_t SCREEN_HEIGHT, uint8_t OLED_RESET, uint8_t SCREEN_ADDRESS);
        ~DisplayManager();
        void init();
        void updateSong(String songName, String artistName);
        void displayChangingSong();
        void clear();
        void scrollTextWhereNeeded();
        void updateSlider(int totalTime, int timePassed);
    private:
        Adafruit_SSD1306 _display;
        uint8_t _screenAddress;
        String _songName;
        String _artistName;
        uint8_t _songNameMaxOffset;
        uint8_t _artistNameMaxOffset;
        uint8_t _songNameCurrentOffset;
        uint8_t _artistNameCurrentOffset;
        bool _isSongBeingChanged;
        bool test;
};



#endif //DISPLAYMANAGER_H
