#ifndef SPOTIFYCLIENT_H
#define SPOTIFYCLIENT_H
#include <DisplayManager.h>
#include <HTTPClient.h>

class SpotifyClient {
    public:
        SpotifyClient(String refreshToken, String authBasicToken);
        ~SpotifyClient();
        int totalTrackTime;
        int passedTrackTime;
        void getCurrentSong(DisplayManager& displayManager);
        void getPlaybackState();
        void skipToPreviousSong(DisplayManager& displayManager);
        void skipToNextSong(DisplayManager& displayManager);
        void play();
        void pause();
        void togglePause();
        void refreshAccessToken();
        bool isPaused = false;
    private:
        HTTPClient _http;
        // String _deviceId;
        bool _handleResponseCode(int httpResponseCode);
        void _getAvailableDevices();
        void _addAuthHeaders();
        String _getRequest(const String& endpoint);
        void _postRequest(const String& endpoint);
        void _putRequest(const String& endpoint);
        String _refreshToken;
        String _accessToken;
        String _authBasicToken;
        const String _serverName = "https://api.spotify.com/v1";
        const String _authServerName = "https://accounts.spotify.com/api";
};

#endif //SPOTIFYCLIENT_H
