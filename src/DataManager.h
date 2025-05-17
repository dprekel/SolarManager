#pragma once

#include <curl/curl.h>
#include <string>


struct CurlSession {
    CURL* handle = nullptr;
    struct curl_slist* headers = nullptr;
    std::string token;
};


class DataManager {
    public:
        DataManager();
        ~DataManager();

        bool initCurlSession();
    
    private:
        std::string createSessionToken();

        CurlSession curlSession;
};
