#include "sound.h"

Sound::Sound(std::string len) : RESTTOKEN = {
    this->len = len;
}

void Sound::addTone(std::string tone){
    tones.push_back(tone);
}

std::string Sound::toString(){
    std::string str = len + "/";
    for(Tones::iterator it = tones.begin(); it != tones.end(); ++it){
        str += " " + *it;
    }
    return "\"" + str + "\"";
}

const std::string RESTTOKEN = "SP";
