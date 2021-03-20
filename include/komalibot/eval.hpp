#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <string>
#include <fstream>
using string = std::string;

string eval(string cmd, string channelid = "", string guildid = "", string author = "") {
 
    std::ofstream ree;
    
    string::size_type i = cmd.find("#include <string>");
    if (i != std::string::npos)
        cmd.erase(i, string("#include <string>").length());
    cmd = "#include <string>\nstd::string channel_id = \"" + channelid + "\";\nstd::string guild_id = \"" + guildid + "\";\nstd::string author_id = \"" + author + "\";\n" + cmd;
    std::cout << cmd;
    ree.open("WRITEFILE.cpp", std::ofstream::out | std::ofstream::trunc);
    ree << cmd;
    ree.close();
    char buffer[128];
   string result = "";
   const char* cmdstr = "g++ -std=c++17 WRITEFILE.cpp -lcurl -lwsock32 -o WRITE.exe 2>&1 && WRITE";
   FILE* pipe = _popen(cmdstr, "r");
   if (!pipe) {
      return "popen failed!";
   }

   while (!feof(pipe)) {

      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   int ret = _pclose(pipe);
   return result;
} 



