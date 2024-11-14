#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H



class WifiManager {
    public:
        WifiManager();
        ~WifiManager();
        void connectToWifi(const char* ssid, const char* password);
        void checkIfDisconnected();
    private:
        char* _ssid;
        char* _password;
};



#endif //WIFIMANAGER_H
