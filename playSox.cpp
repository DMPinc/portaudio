#include "playSox.h"
#include <time.h>
#include <sstream>

PlaySox::PlaySox(std::string input){
    // init playedSound and reset rand seed
    playedSound = NULL;
    srand((unsigned int)time(NULL));
    // set sounds
    size_t current = 0, found; 
    while((found = input.find_first_of(" ", current)) != std::string::npos){
        std::string soundStr = std::string(input, current, found - current);
        sounds.push_back(parse(soundStr));
        current = found + 1;
    }
    std::string soundStr = std::string(input, current, input.size() - current);
    sounds.push_back(parse(soundStr));
}

Sound* PlaySox::parse(std::string soundStr){
    Sound *snd = new Sound();
    size_t current = 0; 
    // get len and 
    size_t found = soundStr.find_first_of("/", current); 
    std::string tmp = std::string(soundStr, current, found - current);
    double l = atof(tmp.c_str()) / 1000.0;
    std::stringstream ss;
    ss << l;
    snd->len = ss.str();
    current = found + 1;

    // get tone
    while((found = soundStr.find_first_of(",", current)) != std::string::npos){
        snd->tones.push_back(std::string(soundStr, current, found - current));
        current = found + 1;
    }
//    std::cout << "tone = " << std::string(soundStr, current, soundStr.size() - current) << std::endl;
    snd->tones.push_back(std::string(soundStr, current, soundStr.size() - current));
    return snd;
}

Sound* PlaySox::getPlayedSound(){
    return playedSound;
}

void PlaySox::play(){
    if(playedSound == NULL){
        playedSound = *sounds.begin();
    }
    // set len
    std::string cmd = PlaySox::PLAYCMD + playedSound->len;
    // set tone
    for(Tones::iterator it = playedSound->tones.begin(); it != playedSound->tones.end(); ++it){
        if(*it == Sound::RESTTOKEN){ // rest case, wait for len
            int rest = atof(playedSound->len.c_str()) * 1000000;
            std::stringstream ss;
            ss << rest;
            // reset play cmd and and set usleep
            std::string cmd = "usleep " + ss.str();
            break;
        }
        else{    
            cmd += PlaySox::PLOPT + *it;
        }
    }
    
    // call cmd
    std::cout << "play cmd = " << cmd << std::endl;
    system(cmd.c_str());
}

void PlaySox::chooseSound(){
    int i = rand() % sounds.size() + 1;
    playedSound = sounds[i];
}


// for test
int main(){
    std::string input = "180/C1,C2 1180/G1,G2";
    PlaySox ps = PlaySox(input);
    
    ps.play();
}
