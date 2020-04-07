#include "opcservicecommon.h"
#include "stockdefs.h"
#include <string>
#include <fstream>
#include <streambuf>

MRL::OpcServiceCommon *MRL::OpcServiceCommon::_instance = nullptr;

MRL::OpcServiceCommon::OpcServiceCommon() {
    _instance = this;
    _data.set(STOCKDEFS::SettingsSection, true);
    _data.set(STOCKDEFS::ConfigureSection, true);
    _data.set(STOCKDEFS::RuntimeSection, true);
}

bool MRL::OpcServiceCommon::loadConfiguration(const std::string &n) { // load the named configuration
    try {
        instance()->_name = n; // save the server name
        std::string f = settingFileName(n);
        std::ifstream is(f, std::ifstream::in);
        if (is.is_open()) {
            Wt::Json::Object v;
            std::stringstream strStream;
            strStream << is.rdbuf();//read the file
            if(stringToJson(strStream.str(),v))
            {
                auto n = data().node(STOCKDEFS::ConfigureSection);
                data().fromJson(n, v);
                return true;
            }
        }
    }
    catch (...) {

    }

    return false;
}

bool MRL::OpcServiceCommon::loadSettings() {
    try {
        std::string f = globalFileName();
        std::ifstream is(f, std::ifstream::in);
        if (is.is_open()) {
            Wt::Json::Object v;
            std::stringstream strStream;
            strStream << is.rdbuf();//read the file
            if(stringToJson(strStream.str(),v))
            {
                auto n = data().node(STOCKDEFS::SettingsSection);
                data().fromJson(n, v);
                return true;
            }
        }
    }
    catch (...) {

    }

    return false;
}

bool MRL::OpcServiceCommon::saveConfiguration(const std::string &n) { // load the named configuration
    try {
        std::string f;
        if (n.empty()) {
            f  = settingFileName(instance()->name());
        }
        else {
            f =  settingFileName(n);
        }

        std::ofstream os(f);
        if (os.is_open()) {
            Wt::Json::Object v;
            auto n = data().node(STOCKDEFS::ConfigureSection);
            data().toJson(n, v);
            std::string s;
            if(jsonToString(v,s))
            {
                os << s;
                return true;
            }
        }
    }
    catch (...) {

    }

    return false;
}

bool MRL::OpcServiceCommon::saveSettings() {
    try {
        std::string f = globalFileName();
        std::ofstream os(f);

        if (os.is_open()) {
            Wt::Json::Object v;
            auto n = data().node(STOCKDEFS::ConfigureSection);
            data().toJson(n, v);
            std::string s;
            if(jsonToString(v,s))
            {
                os << s;
                return true;
            }
        }
    }
    catch (...)
    {

    }
    return false;
}

bool MRL::stringToBool(const std::string &s) {
    static const char *trueStr[] = {"True", "true", "1"};
    for (int i = 0; i < 3; i++)
        if (s == trueStr[i]) return true;
    return false;
}

int  MRL::stringTimeToInt(const std::string &s) {
    int ret = 0;
    boost::char_separator<char> sep(":");
    tokenizer tokens(s, sep);
    std::vector<std::string> l;
    for (auto i = tokens.begin(); i != tokens.end(); i++) {
        l.push_back(*i);
    }

    switch (l.size()) {
    case 1:
        ret = std::stoi(l[0]);
        break;
    case 2:
        ret = std::stoi(l[1]) * 60 + std::stoi(l[0]);
        break;
    case 3:
        ret = std::stoi(l[0]) * 3600 + std::stoi(l[1]) * 60 + std::stoi(l[2]) ;
        break;
    default:
        break;
    }

    return ret;
}
