#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <WsClientLib/WsClientLib.hpp>
#include <WsClientLib/WsClientLib.cpp>
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
#include <random>
#include <komalibot/httpdiscord.hpp>
//#include <komalibot/utils.hpp>
using json = nlohmann::json;
using string = std::string;
string get_token() {
    std::ifstream file("./auth.json");
    string content((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    json j = json::parse(content);
    string token = j["token"];
    token.erase(remove(token.begin(), token.end(), '\"'), token.end());
    return token;
}
Client client(get_token());
Utils utils;






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
                client.postMessage(channel_id, "sent from c++ \"(for real this time)");
            }
            if (content == "kb!cppemotes") {
                json emotes = client.get_guild_emotes(guild_id);
                client.postMessage(channel_id, string(emotes[0]["key"]));
            }
            if (args[0] == "kb!cppargs") {
                if (args.size() < 2) {
                    client.postMessage(channel_id, "No other args specified");
                }
                else {
                    client.postMessage(channel_id, args[1]);
                }
            }
            //std::cout << args[0];
            if (args[0] == "kb!roles") {    
                //json roles = client.get_self(guild_id);
                //   client.postMessage(channel_id, utils_codeblock_encode(roles.dump(2), "json"));
                if (args.size() > 1) {
                    client.add_role(guild_id, args[1], string(author["id"]));
                    client.postMessage(channel_id, utils.codeblock_encode(author.dump(2), "json"));
                }
                else {
                    client.postMessage(channel_id, "moar args pls");
                }
            }
            if (args[0] == "kb!cppslots") {
                string emotestr;
                int num = 3;
                json emotes = client.get_guild_emotes(guild_id);
                for (int i = 0; i < num; i++) {
                    emotestr += string(emotes[utils.random_int(int(emotes.size()))]["key"]);
                }
                client.postMessage(channel_id, emotestr);
                client.postMessage(channel_id, "Please Play Again");
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
    
    
    return 0;
}
