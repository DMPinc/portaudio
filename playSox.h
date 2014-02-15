#ifndef PLAYSOX
#define PLAYSOX

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

class PlaySox{
    private:
        const std::string PLAYCMD = "/usr/local/Cellar/sox/14.4.1/bin/play -n synth ";
        const std::string PLOPT = " pl ";
        const std::string RESTTOKEN = "SP";
        static Sound* parse(std::string soundStr);
        Sound* playedSound;
        Sounds sounds;
    public:
        PlaySox(std::string input);
        void chooseSound();
        void play();
        Sound* getPlayedSound();
};

#endif
