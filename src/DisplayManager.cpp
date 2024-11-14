#include "DisplayManager.h"
#include <Wire.h>

DisplayManager::DisplayManager(uint8_t SCREEN_WIDTH, uint8_t SCREEN_HEIGHT, uint8_t OLED_RESET, uint8_t SCREEN_ADDRESS) {
    _display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    _screenAddress = SCREEN_ADDRESS;
}

DisplayManager::~DisplayManager() {
}

void DisplayManager::init() {
    if(!_display.begin(SSD1306_SWITCHCAPVCC, _screenAddress)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    _display.display();
}

void DisplayManager::displayChangingSong() {
    _isSongBeingChanged = true;
    _display.clearDisplay();

    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0,0);
    _display.println("Changing song...");

    _display.display();
}

void DisplayManager::updateSong(String songName, String artistName) {
    if(songName != _songName || artistName != _artistName) {
        _songName = songName;
        _artistName = artistName;
        _songNameMaxOffset = 0;
        _artistNameMaxOffset = 0;

        if(songName.length() > 21) {
            _songNameMaxOffset = songName.length() - 21;
            _songNameCurrentOffset = 0;
        }
        if(artistName.length() > 21) {
            _artistNameMaxOffset = artistName.length() - 21;
            _artistNameCurrentOffset = 0;
        }

        _display.clearDisplay();
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(0,0);
        _display.println(songName.substring(0, 21));
        _display.setCursor(0,10);
        _display.println(artistName.substring(0, 21));
        updateSlider(127800, 40600);
        _display.display();
        _isSongBeingChanged = false;
    }
}

void DisplayManager::scrollTextWhereNeeded() {
    if(_artistName && _songName && !_isSongBeingChanged){
        if(_songNameMaxOffset != 0){
            _display.fillRect(0, 0, _display.width(), 10, SSD1306_BLACK);
        }
        if(_artistNameMaxOffset != 0){
            _display.fillRect(0, 10, _display.width(), 10, SSD1306_BLACK);
        }
        if(_songNameCurrentOffset > _songNameMaxOffset){
            _songNameCurrentOffset = 0;
        }
        if(_artistNameCurrentOffset > _artistNameMaxOffset){
            _artistNameCurrentOffset = 0;
        }
        _display.display();
        _display.setCursor(0,0);
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.println(_songName.substring(_songNameCurrentOffset, 21 + _songNameCurrentOffset));
        _display.setCursor(0,10);
        _display.println(_artistName.substring(_artistNameCurrentOffset, 21 + _artistNameCurrentOffset));
        _songNameCurrentOffset++;
        _artistNameCurrentOffset++;
        _display.display();
    }
}

void DisplayManager::updateSlider(int totalTime, int timePassed) {
    if(!_isSongBeingChanged) {
        _display.fillRect(0, 20, _display.width(), 12, SSD1306_BLACK);
        _display.display();
        _display.setCursor(0,25);

        const String passedTimeSeconds = String(timePassed/1000 - (timePassed/60000) * 60);

        _display.print(String(timePassed/60000) + ":" + (passedTimeSeconds.length() < 2 ? "0" : "") + passedTimeSeconds.substring(0,2));

        _display.fillRect(30, 28, _display.width()-70, 2, SSD1306_WHITE);
        const int positionOnSlider = (static_cast<float>(timePassed)/totalTime) * (_display.width() - 70);
        _display.fillCircle(positionOnSlider + 30, 28, 2, SSD1306_WHITE);

        _display.setCursor(_display.width()-35,25);

        const String totalTimeSeconds = String(totalTime/1000 - (totalTime/60000) * 60);
        _display.print(String(totalTime/60000) + ":" + (totalTimeSeconds.length() < 2 ? "0" : "") + totalTimeSeconds.substring(0,2));
        _display.display();
    }
}