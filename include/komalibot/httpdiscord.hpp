#include <nlohmann/json.hpp>
#include <iostream>
#include <curl/curl.h>
#include <komalibot/utils.hpp>
#ifndef UTIL_CLIENT_ _
#define UTIL_CLIENT_ _
#endif
using json = nlohmann::json;
using string = std::string;

size_t writeFunction(void* ptr, size_t size, size_t nmemb, string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

class Client {
    string token;
public:
    Client(string);
    string tokentest() {
        return token;
    };
    json postMessage(string channel, string content, json embed = json::parse("{}")) {
        Utils utils; 
        CURL* curl;
        CURLcode res;
        string url = "https://discord.com/api/v8/channels/" + channel + "/messages";

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            struct curl_slist* chunk = NULL;
            string header = "Authorization: Bot " + token;
            string header2 = "Content-Type: application/json";
            chunk = curl_slist_append(chunk, header.c_str());
            chunk = curl_slist_append(chunk, header2.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            //std::cout << "function: " << replace_all(content, "\"", "\\\"") << "\n"; //works
            string newcontent = utils.replace_all(content, "\"", "\\\""); //doesnt...?
            //std::cout << "newcontent: " << newcontent << "\n";
            string body = string("{ \"content\": \"" + string(newcontent) + "\"}");
            //std::cout << body;
            body = utils.replace_all(body, string("\n"), string("\\n"));


            //std::cout << body << std::endl;
            //std::cout << (body.find(" ") != string::npos) << std::endl;
            string resp;
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            //std::cout<< json::parse(resp).dump(2) << std::endl;
            return json::parse(resp);
        }

        curl_global_cleanup();
        return json::parse("{}");
    }

    json api_call(string path) {
        string url = "https://discord.com/api/v8/" + string(path);
        //char *c_url = url.c_str();
        curl_global_init(CURL_GLOBAL_DEFAULT);
        auto curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            struct curl_slist* chunk = NULL;
            string header = "Authorization: Bot " + string(token);
            string header2 = "Content-Type: application/json";

            chunk = curl_slist_append(chunk, header.c_str());
            chunk = curl_slist_append(chunk, header2.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

            string response_string;
            string header_string;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

            curl_easy_perform(curl);
            //std::cout << response_string;

            curl_easy_cleanup(curl);
            curl_global_cleanup();
            curl = NULL;


            return json::parse(response_string);
        }
        return json::parse("{\"Error\": \"curl request failed\"}");
    }
    json get_self(string guild = "NO_VALUE") {
        if (guild == "NO_VALUE") {
            return api_call("users/@me");
        }
        string userid = string(api_call("users/@me")["id"]);
        return api_call(string("guilds/") + guild + "/members/" + userid);
    }
    json get_guilds() {
        return api_call("users/guilds");
    }
    json get_guild_channels(string guild) {
        return api_call("guilds/" + guild + "/channels");
    }
    json get_guild_emotes(string guild, string json_str = "array") {
        json j = api_call("guilds/" + guild + "/emojis");
        json outjson;
        for (int i = 0; i < j.size(); i++) {
            string animated = "";
            if (j[i]["animated"] == true) {
                animated = "a";
            }
            if (json_str == "json") {
                outjson[i] = "<" + animated + ":" + string(j[i]["name"]) + ":" + string(j[i]["id"]) + ">";
            }
            else {
                j[i]["key"] = "<" + animated + ":" + string(j[i]["name"]) + ":" + string(j[i]["id"]) + ">";
                outjson[i] = j[i];
            }

        }
        return outjson;
    }
    void add_role(string guild, string roleid, string user = "@me") {
        if (user == "@me") {
            user = string(get_self()["id"]);
        }
        CURLcode ret;
        CURL* hnd;
        struct curl_slist* slist1;
        curl_global_init(CURL_GLOBAL_ALL);
        slist1 = NULL;
        string header = "Authorization: Bot " + token;
        slist1 = curl_slist_append(slist1, header.c_str());
        slist1 = curl_slist_append(slist1, "Content-Length: 0");
        string url = "https://discord.com/api/v8/guilds/" + guild + "/members/" + user + "/roles/" + roleid;
        hnd = curl_easy_init();
        if (hnd) {
            curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
            curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
            curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "");


            curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
            curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "PUT");

            ret = curl_easy_perform(hnd);
        }
        curl_global_cleanup();
    }
    void remove_role(string guild, string roleid, string user = "@me") {
        if (user == "@me") {
            user = string(get_self()["id"]);
        }
        CURLcode ret;
        CURL* hnd;
        struct curl_slist* slist1;
        curl_global_init(CURL_GLOBAL_ALL);
        slist1 = NULL;
        string header = "Authorization: Bot " + token;
        slist1 = curl_slist_append(slist1, header.c_str());
        slist1 = curl_slist_append(slist1, "Content-Length: 0");
        string url = "https://discord.com/api/v8/guilds/" + guild + "/members/" + user + "/roles/" + roleid;
        hnd = curl_easy_init();
        if (hnd) {
            curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
            curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
            curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "");


            curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
            curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");

            ret = curl_easy_perform(hnd);
        }
        curl_global_cleanup();
    }
    json message_update(string channelid, string messageid) {
        CURLcode ret;
        CURL* hnd;
        struct curl_slist* slist1;
        string url = "https://discord.com/api/v8/channels/" + channelid + "/messages/" + messageid;
        slist1 = NULL;
        slist1 = curl_slist_append(slist1, "Content-Type: application/json");
        string h = "Authorization: Bot " + token;
        slist1 = curl_slist_append(slist1, h.c_str());

        hnd = curl_easy_init();
        if (hnd) {
            curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
            curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
            curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "{\"content\": \"it work\"}");
            curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)22);
            curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.55.1");
            curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
            curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "PATCH");
            curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
            string response;

            curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);
            ret = curl_easy_perform(hnd);
            std::cout << response;
            return json::parse(response);
        }
        return json::parse("{}");
    }

};
Client::Client(string token_a) {
    token = token_a;
}
