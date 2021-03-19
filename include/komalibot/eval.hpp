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

   // \"C:\\Program Files (x86)\\mingw-w64\\i686-8.1.0-posix-dwarf-rt_v6-rev0\\mingw32\\bin\\g++.exe\" WRITEFILE.cpp -L\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\TeamFoundation\\Team Explorer\\Git\\mingw32\\bin\" -o write && write
//cd \"C:\\Program Files (x86)\\mingw-w64\\i686-8.1.0-posix-dwarf-rt_v6-rev0\\mingw32\\bin\" && g++ -L\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\TeamFoundation\\Team Explorer\\Git\\mingw32\\bin\""
   const char* cmdstr = "g++ -std=c++17 WRITEFILE.cpp -lcurl -lwsock32 -o WRITE.exe && WRITE.exe";
  // std::cout << cmdstr;
   FILE* pipe = _popen(cmdstr, "r");
   if (!pipe) {
      return "popen failed!";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   int ret = _pclose(pipe);
   if (ret == 1) {
       result = "Error!";
   }
   return result;
} 



