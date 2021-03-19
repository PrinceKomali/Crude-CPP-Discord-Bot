#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include <random>
#include <sstream>
using string = std::string;


class Utils {
public:

    string replace_all(string, const string&, const string&);
    bool startsWith(string s1, string s2){
        return (s1.rfind(s2, 0) == 0);
    }
    bool endsWith(std::string const& fullString, std::string const& ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else {
            return false;
        }
    }

    string trim(string str) {
        size_t endpos = str.find_last_not_of(" \t");
        size_t startpos = str.find_first_not_of(" \t");
        if (std::string::npos != endpos)
        {
            str = str.substr(0, endpos + 1);
            str = str.substr(startpos);
        }
        else {
            str = str.c_str();
            str.erase(std::remove(std::begin(str), std::end(str), ' '), std::end(str));
            str = string(str);
        }
        return str;
    }
    string codeblock_decode(string str) {
        str = trim(str);
        if (str.find("```") == string::npos || !startsWith(str, "```") || !endsWith(str, "```")) {
            return str;
        }
        str = str.substr(0,str.length() - 3);
        if (startsWith(str, "``` ") || startsWith(str, "```\n")) {
            str = str.substr(4, str.length());
        }
        if (str.find("\n") == string::npos) {
            str = str.substr(3, str.length());
        }
        std::stringstream test(str);
        std::string segment;
        std::vector<std::string> l;

        while (std::getline(test, segment, '\n'))

        {
            l.push_back(segment);

        }
        if (str.find("\n") != string::npos && l[0].find(" ") == string::npos) {
            l.erase(l.begin());
            const char* const delim = "\n";

            std::ostringstream imploded;
            std::copy(l.begin(), l.end(),
                std::ostream_iterator<std::string>(imploded, delim));
            str = imploded.str();
        }
        else {
            string::size_type i = str.find("```");
            str.erase(i, str.length());
        }
        //std::cout << str;
        return str;

    }
    string codeblock_encode(string str, string lang = "") {
        return "```" + lang + "\n" + str + "\n```";
    }
    int random_int(int st, int nd = 0) {
        if (nd == 0) {
            nd = st;
            st = 0;
        }
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(1.0, 10.0);


        return floor(dist(mt));
    }

};


string Utils::replace_all(string str, const string& from, const string& to) {
    //shamelessly stolen from stackoverflow
    size_t start_pos = 0;
    //std::cout << str << "\n";
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
      //  std::cout <<str << std::endl;
        start_pos += to.length(); 
    }
 //   std::cout << str << "\n"; //works
    return string(str); //crashes
 }



