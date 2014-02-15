#ifndef SOUND
#define SOUND

#include <string>
#include <vector>
#include <map>
#include <iostream>
typedef std::vector<std::string> Tones;

class Sound{
    public:
        std::string len;
        Tones tones;
};

typedef std::vector<Sound*> Sounds;

#endif
