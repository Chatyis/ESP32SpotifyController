#include <SpotifyClient.h>
#include <WifiManager.h>
#include <DisplayManager.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define PLAY_PIN 13
#define PREVIOUS_PIN 18
#define NEXT_PIN 16

const char* ssid = "";
const char* password = "";
const String refreshToken = "";
const String authBasicToken = "Basic ";

unsigned long lastTime = 0;
unsigned long lastTimeScrolled = 0;
unsigned long lastTimeSliderUpdated = 0;
unsigned long timerDelay = 5000;
unsigned long displayScrollDelay = 750;
unsigned long sliderUpdateDelay = 1000;

SpotifyClient spotifyClient(refreshToken, authBasicToken);
WifiManager wifiManager;
DisplayManager displayManager(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET, SCREEN_ADDRESS);

struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button buttonPlay = {PLAY_PIN,  false};
Button buttonPrevious = {PREVIOUS_PIN,  false};
Button buttonNext = {NEXT_PIN,  false};

unsigned long buttonTime = 0;
unsigned long lastButtonTime = 0;

void IRAM_ATTR playButtonIsr() {
  buttonTime = millis();
  if (buttonTime - lastButtonTime > 250)
  {
    buttonPlay.pressed = true;
    lastButtonTime = buttonTime;
  }
}

void IRAM_ATTR previousButtonIsr() {
  buttonTime = millis();
  if (buttonTime - lastButtonTime > 250)
  {
    buttonPrevious.pressed = true;
    lastButtonTime = buttonTime;
  }
}

void IRAM_ATTR nextButtonIsr() {
  buttonTime = millis();
  if (buttonTime - lastButtonTime > 250)
  {
    buttonNext.pressed = true;
    lastButtonTime = buttonTime;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(PLAY_PIN, INPUT_PULLUP);
  pinMode(PREVIOUS_PIN, INPUT_PULLUP);
  pinMode(NEXT_PIN, INPUT_PULLUP);
  attachInterrupt(buttonPlay.PIN, playButtonIsr, FALLING);
  attachInterrupt(buttonPrevious.PIN, previousButtonIsr, FALLING);
  attachInterrupt(buttonNext.PIN, nextButtonIsr, FALLING);

  displayManager.init();
  wifiManager.connectToWifi(ssid, password);
  spotifyClient.refreshAccessToken();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    wifiManager.checkIfDisconnected();
    spotifyClient.getCurrentSong(displayManager);
    lastTime = millis();
  }

  if ((millis() - lastTimeScrolled) > displayScrollDelay) {
    displayManager.scrollTextWhereNeeded();
    lastTimeScrolled = millis();
  }

  if ((millis() - lastTimeSliderUpdated) > sliderUpdateDelay && !spotifyClient.isPaused) {
    spotifyClient.passedTrackTime += 1000;
    displayManager.updateSlider(spotifyClient.totalTrackTime, spotifyClient.passedTrackTime);
    lastTimeSliderUpdated = millis();
  }

  if (buttonPlay.pressed) {
    lastTime = millis();
    spotifyClient.togglePause();
    buttonPlay.pressed = false;
  }

  if (buttonPrevious.pressed) {
    lastTime = millis();
    spotifyClient.skipToPreviousSong(displayManager);
    buttonPrevious.pressed = false;
  }

  if (buttonNext.pressed) {
    lastTime = millis();
    spotifyClient.skipToNextSong(displayManager);
    buttonNext.pressed = false;
  }
}