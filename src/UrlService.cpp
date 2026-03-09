#include "UrlService.h"
#include <iostream>

sqlite3* DB;

void UrlService::initDB() {

    int exit = sqlite3_open("urlshortener.db", &DB);

    if (exit != SQLITE_OK) {
        std::cerr << "Error opening DB: " << sqlite3_errmsg(DB) << std::endl;
    } 
    else {
        std::cout << "Database opened successfully!" << std::endl;
    }

    std::string sql =
        "CREATE TABLE IF NOT EXISTS urls("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "short_code TEXT UNIQUE,"
        "original_url TEXT,"
        "click_count INTEGER DEFAULT 0);";

    char* errMsg;

    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &errMsg);

    if (exit != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}


void UrlService::insertURL(const std::string& shortCode, const std::string& originalUrl) {

    std::string sql = "INSERT INTO urls (short_code, original_url, click_count) VALUES (?, ?, 0);";

    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, shortCode.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, originalUrl.c_str(), -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}


std::string UrlService::getOriginalURL(const std::string& code) {

    std::string sql = "SELECT original_url FROM urls WHERE short_code = ?;";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);

    std::string result = "";

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);

    return result;
}


void UrlService::incrementClicks(const std::string& code) {

    std::string sql = "UPDATE urls SET click_count = click_count + 1 WHERE short_code = ?;";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}


int UrlService::getClickCount(const std::string& code) {

    std::string sql = "SELECT click_count FROM urls WHERE short_code = ?;";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);

    int count = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return count;
}