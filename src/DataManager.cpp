#include <curl/curl.h>
#include <stdio.h>
#include <iostream>

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
    if (sessionToken.empty()) {
        return false;
    }

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
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "";
    }

    CURLcode res;
    std::string readBuffer;
    struct curl_slist* headers = nullptr;

    // Session-Cookie wird intern von libcurl verwaltet
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); // leere Zeichenkette = In-Memory-Cookie-Handling aktivieren

    // === 1. LOGIN ===
    const char* loginData = ""; // lookup in logindata.txt
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    curl_easy_setopt(curl, CURLOPT_URL, "https://entrez.enphaseenergy.com/login");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, loginData);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Login fehlgeschlagen: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return "";
    }

    std::cout << "[Login erfolgreich]" << std::endl;
    readBuffer.clear(); // für nächste Antwort leeren

    // === 2. TOKEN ANFORDERN ===
    const char* tokenData = ""; // lookup in logindata.txt
    std::string accessToken;

    // POST-Daten für Token setzen
    curl_easy_setopt(curl, CURLOPT_URL, "https://entrez.enphaseenergy.com/entrez_tokens");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, tokenData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    // COOKIE bleibt erhalten, da curl im selben Handle bleibt

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Token-Anfrage fehlgeschlagen: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "[Antwort auf Token-Anfrage]:\n" << readBuffer << std::endl;
        accessToken = this->extractToken(readBuffer);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return accessToken;

}


std::string DataManager::extractToken(std::string& html) {
    const std::string startTag = "<textarea name=\"accessToken\" id=\"JWTToken\"";
    const std::string closeTag = "</textarea>";

    size_t startPos = html.find(startTag);
    if (startPos == std::string::npos) {
        return "";
    }

    // springe zum schließenden ">" des <textarea ...>
    size_t tokenStart = html.find('>', startPos);
    if (tokenStart == std::string::npos) {
        return "";
    }

    tokenStart += 1; // direkt nach dem >

    size_t tokenEnd = html.find(closeTag, tokenStart);
    if (tokenEnd == std::string::npos) {
        return "";
    }

    return html.substr(tokenStart, tokenEnd - tokenStart);
}
