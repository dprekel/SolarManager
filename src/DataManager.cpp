#include <curl/curl.h>
#include <stdio.h>

#include "DataManager.h"


DataManager::DataManager() {
    printf("DataManager object initialized\n");
}

DataManager::~DataManager() {
    printf("DataManager object destructed\n");
}

bool DataManager::initCurlSession() {
    std::string sessionToken = this->createSessionToken();

    curl_global_init(CURL_GLOBAL_DEFAULT);
    this->curlSession.handle = curl_easy_init();
    if (!curlSession.handle) {
        return false;
    }

    this->curlSession.token = sessionToken;
    this->curlSession.headers = curl_slist_append(nullptr, ("Authorization: Bearer " + this->curlSession.token).c_str());
    return true;
}

std::string DataManager::createSessionToken() {
    // TODO: implement this
    return "dkfadkfjk";
}
