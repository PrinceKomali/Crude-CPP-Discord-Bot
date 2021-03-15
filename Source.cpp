#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "WsClientLib/WsClientLib.hpp"
#include "WsClientLib/WsClientLib.cpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <typeinfo>
#include <sstream>
#include <string>
#include <memory>
#include <stdlib.h>
#include <ctime>
#include <stdexcept>
#include <array>
#include <stdio.h>



using json = nlohmann::json;
using string = std::string;
string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}
string get_token() {
    std::ifstream file("./auth.json");
    string content((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    json j = json::parse(content);
    string token = j["token"];
    token.erase(remove(token.begin(), token.end(), '\"'), token.end());
    return token;
}
size_t writeFunction(void* ptr, size_t size, size_t nmemb, string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}
json api_call(string path) {
    string url = "https://discord.com/api/v8/" + string(path);
    //char *c_url = url.c_str();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist* chunk = NULL;
        string header = "Authorization: Bot " + string(get_token());
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
json postMessage(string channel, string content) {
    string token = get_token();
    CURL* curl;
    CURLcode res;
    string url = "https://discord.com/api/v8/channels/" + channel + "/messages";
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        //std::cout << content;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        struct curl_slist* chunk = NULL;
        string header = "Authorization: Bot " + token;
        string header2 = "Content-Type: application/json";
        chunk = curl_slist_append(chunk, header.c_str());
        chunk = curl_slist_append(chunk, header2.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        string body = string("{ \"content\": \"") + ReplaceAll(content, string("\""), string("\\\"")) + +"\"}";

        body = ReplaceAll(body, string("\n"), string("\\n"));
       

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
        return json::parse(resp);
    }
    
    curl_global_cleanup();
    return json::parse("{}");
}

json client_get_self(string guild = "NO_VALUE") {
    if (guild == "NO_VALUE") {
        return api_call("users/@me");
    }
    string userid = string(api_call("users/@me")["id"]);
    return api_call(string("guilds/") + guild + "/members/" + userid);
}
json client_get_guilds() {
    return api_call("users/guilds");
}
json client_get_guild_channels(string guild) {
    return api_call("guilds/" + guild + "/channels");
}
json client_get_guild_emotes(string guild, string json_str = "array") {
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
void client_add_role(string guild, string roleid, string user = "@me") {
    if (user == "@me") {
        user = string(client_get_self()["id"]);
    }
    CURLcode ret;
    CURL* hnd;
    struct curl_slist* slist1;
    curl_global_init(CURL_GLOBAL_ALL);
    slist1 = NULL;
    string header = "Authorization: Bot " + get_token();
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
void client_remove_role(string guild, string roleid, string user = "@me") {
    if (user == "@me") {
        user = string(client_get_self()["id"]);
    }
    CURLcode ret;
    CURL* hnd;
    struct curl_slist* slist1;
    curl_global_init(CURL_GLOBAL_ALL);
    slist1 = NULL;
    string header = "Authorization: Bot " + get_token();
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
json client_message_update(string channelid, string messageid) {
    CURLcode ret;
    CURL* hnd;
    struct curl_slist* slist1;
    string url = "https://discord.com/api/v8/channels/" + channelid + "/messages/" + messageid;
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");
    string h = "Authorization: Bot " + get_token();
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

string utils_codeblock_encode(string str, string lang = "") {
    return "```" + lang + "\n" + str + "\n```";
}
string utils_codeblock_decode(string str) {
    //note to self: do this later
    return "e";
}
void utils_exec_compile(string code) {
    
}

void OnErrorCallback(const WsClientLib::WSError& err, void* pUserData)
{
    std::cout << "error code: " << err.code << ", error: " << err.message << std::endl;

}
void OnMessage(const string& m, void* pUserData)
{   
   json message = json::parse(m);
    if (message["t"].dump() == "\"MESSAGE_CREATE\"") {
        try {
            string content = string(message["d"]["content"]);

            string channel_id = string(message["d"]["channel_id"]);
            string guild_id = string(message["d"]["guild_id"]);
            json author = message["d"]["author"];
            std::stringstream test(content);
            std::string segment;
            std::vector<std::string> args;
            if (content.find(" ") == string::npos) {
                args.push_back(content);
            }
            else if (content == "") {
                args.push_back("");
            }
            else {

                while (std::getline(test, segment, ' '))
                
                {
                    args.push_back(segment);
                }
            }
            if (content == "kb!cpptest") {
                postMessage(channel_id, "sent from c++ (for real this time)");
            }
            if (content == "kb!cppemotes") {
                json emotes = client_get_guild_emotes(guild_id);
                postMessage(channel_id, string(emotes[0]["key"]));
            }
            if (args[0] == "kb!cppargs") {
                if (args.size() < 2) {
                    postMessage(channel_id, "No other args specified");
                }
                else {
                    postMessage(channel_id, args[1]);
                }
            }
            //std::cout << args[0];
            if (args[0] == "kb!roles") {    
                //json roles = client_get_self(guild_id);
                //   postMessage(channel_id, utils_codeblock_encode(roles.dump(2), "json"));
                if (args.size() > 1) {
                    client_add_role(guild_id, args[1], string(author["id"]));
                    postMessage(channel_id, utils_codeblock_encode(author.dump(2), "json"));
                }
                else {
                    postMessage(channel_id, "moar args pls");
                }
            }
            if (args[0] == "kb!cppslots") {
                string emotestr;
                int num = 3;
                json emotes = client_get_guild_emotes(guild_id);
                for (int i = 0; i < num; i++) {
                    emotestr += string(emotes[i]["key"]);
                }
                postMessage(channel_id, emotestr);
                postMessage(channel_id, "Please Play Again");
            }
        }
        catch (string err) {
            std::cout << err << std::endl;      
        }
        
    }
 }
WsClientLib::WebSocket::pointer ws = nullptr;
void websocketconnect(int argc, char* argv[]) {
    string token = get_token();
    string auth = "{ \"op\": 2, \"d\": { \"token\": \"" + string(token) + "\", \"intents\": 32767, \"properties\": { \"$os\": \"linux\", \"$browser\": \"my_library\", \"$device\": \"my_library\" } } }";
    using namespace WsClientLib;
    ws = WebSocket::from_url("wss://gateway.discord.gg", "");
    ws->connect();
    bool isOpen = false;

        while (ws->getReadyState() != WebSocket::readyStateValues::CLOSED) {
        if (ws->getReadyState() == WebSocket::readyStateValues::OPEN && !isOpen) {
            ws->send(auth);
            isOpen = true;
        }
        else {
            WebSocket::pointer wsp = ws;
            ws->poll(0, OnErrorCallback, nullptr);
            ws->dispatch(OnMessage, OnErrorCallback, nullptr);
        }
    }

    websocketconnect(argc, argv);
}
int main(int argc, char* argv[])
{
     websocketconnect(argc, argv);
   // client_message_update("754061738634379366", "821058775783309353");
    
    
    return 0;
}
