#include <curl/curl.h>
#include <stdio.h>

#include "DataManager.h"

size_t DataManager::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

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

    curl_easy_setopt(this->curlSession.handle, CURLOPT_HTTPHEADER, &this->curlSession.headers);
    curl_easy_setopt(this->curlSession.handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(this->curlSession.handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(this->curlSession.handle, CURLOPT_SSL_VERIFYHOST, 0L);

    return true;
}

void DataManager::destroyCurlSession() {
    if (this->curlSession.headers) {
        curl_slist_free_all(this->curlSession.headers);
    }
    if (this->curlSession.handle) {
        curl_easy_cleanup(this->curlSession.handle);
    }
    curl_global_cleanup();
}

std::string DataManager::createSessionToken() {
    // TODO: implement this
    return "dkfadkfjk";
}

