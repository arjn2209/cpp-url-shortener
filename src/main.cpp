#include "crow.h"
#include "UrlService.h"
#include <string>
#include <random>
#include <unordered_map>
#include <chrono>
#include <mutex>

std::string generateShortCode(int length = 6) {

    static const std::string chars =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string shortCode;

    for (int i = 0; i < length; ++i) {
        shortCode += chars[dist(gen)];
    }

    return shortCode;
}

std::unordered_map<std::string, std::vector<std::chrono::steady_clock::time_point>> requestLog;
std::mutex rateLimitMutex;

const int MAX_REQUESTS = 10;
const int WINDOW_SECONDS = 60;

bool isRateLimited(const std::string& ip) {

    std::lock_guard<std::mutex> lock(rateLimitMutex);

    auto now = std::chrono::steady_clock::now();

    auto& timestamps = requestLog[ip];

    timestamps.erase(
        std::remove_if(
            timestamps.begin(),
            timestamps.end(),
            [&](auto& t){
                return std::chrono::duration_cast<std::chrono::seconds>(now - t).count() > WINDOW_SECONDS;
            }),
        timestamps.end()
    );

    if (timestamps.size() >= MAX_REQUESTS) {
        return true;
    }

    timestamps.push_back(now);
    return false;
}
void log(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

int main() {

    UrlService::initDB();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "URL Shortener Backend Running 🚀";
    });


    CROW_ROUTE(app, "/shorten")
    ([](const crow::request& req){

        std::string ip = req.remote_ip_address;

        if(isRateLimited(ip)){
            return crow::response(429, "Too many requests. Try again later.");
        }

        auto url = req.url_params.get("url");

        if(!url){
            return crow::response(400,"URL parameter missing");
        }

        std::string original_url = url;

        UrlService::insertURL("", original_url);

        long long id = UrlService::getLastInsertId();

        std::string short_code = UrlService::encodeBase62(id);

        UrlService::updateShortCode(id, short_code);

        crow::json::wvalue result;
        result["short_url"] = "http://localhost:18080/" + short_code;
        result["original_url"] = original_url;
        log("Short URL created: " + short_code);
        return crow::response(result);  
    });


    CROW_ROUTE(app, "/<string>")
    ([](std::string code){

        std::string url = UrlService::getOriginalURL(code);

        if(url.empty()){
            return crow::response(404,"Short URL not found");
        }

        UrlService::incrementClicks(code);

        crow::response res;

        res.code = 302;
        res.set_header("Location", url);
        log("Redirect request for code: " + code);
        return res;
    });


    CROW_ROUTE(app, "/analytics/<string>")
    ([](std::string code){

        std::string url = UrlService::getOriginalURL(code);

        if(url.empty()){
            return crow::response(404,"Short URL not found");
        }

        int clicks = UrlService::getClickCount(code);

        crow::json::wvalue result;

        result["original_url"] = url;
        result["click_count"] = clicks;

        return crow::response(result);
    });


    app.port(18080).multithreaded().run();
}