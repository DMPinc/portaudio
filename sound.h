#ifndef SOUND
#define SOUND

#include <string>
#include <vector>
#include <map>
#include <iostream>
typedef std::vector<std::string> Tones;

class Sound{
    public:
        static const std::string RESTTOKEN;
        std::string len;
        Tones tones;
        
        Sound(std::string len = "0");
        void addTone(std::string tone);
        std::string toString();
};

typedef std::vector<Sound*> Sounds;

#endif
