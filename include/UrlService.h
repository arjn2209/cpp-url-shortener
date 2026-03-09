#pragma once
#include <string>
#include <sqlite3.h>

class UrlService {
public:
    static void initDB();

    static void insertURL(const std::string& shortCode, const std::string& originalUrl);

    static std::string getOriginalURL(const std::string& shortCode);

    static void incrementClicks(const std::string& shortCode);

    static int getClickCount(const std::string& shortCode);
};