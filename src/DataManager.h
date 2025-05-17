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
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

        DataManager();
        ~DataManager();

        bool initCurlSession();
        void destroyCurlSession();
    
    private:
        std::string createSessionToken();

        CurlSession curlSession;
};
