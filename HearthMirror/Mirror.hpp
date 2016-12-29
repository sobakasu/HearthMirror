//
//  Mirror.hpp
//  Mirror
//
//  Created by Istvan Fehervari on 26/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef HearthMirror_hpp
#define HearthMirror_hpp

#include <locale>
#include <vector>
#include "Mono/MonoImage.hpp"

typedef std::vector<std::string> HMObjectPath;

// Return types
// ------------

typedef struct _BattleTag {
    std::u16string name;
    int number;
} BattleTag;


typedef struct _Card {
    std::u16string id;
    int count;
    bool premium;
    
    _Card(std::u16string id, int count, bool premium) : id(id), count(count), premium(premium) { }
} Card;

typedef struct _InternalGameServerInfo {
    std::u16string address;
    std::u16string auroraPassword;
    long clientHandle;
    int gameHandle;
    int mission;
    int port;
    bool resumable;
    bool spectatorMode;
    std::u16string spectatorPassword;
    std::u16string version;
} InternalGameServerInfo;

typedef struct _InternalPlayer {
    std::u16string name;
    int id;
    int standardRank;
    int standardLegendRank;
    int standardStars;
    int wildRank;
    int wildLegendRank;
    int wildStars;
    int cardBackId;
} InternalPlayer;

typedef struct _InternalMatchInfo {
    InternalPlayer localPlayer;
    InternalPlayer opposingPlayer;
    int brawlSeasonId;
    int missionId;
    int rankedSeasonId;
} InternalMatchInfo;

namespace hearthmirror {
    
    class Mirror {
        
    public:
        
        Mirror();
        Mirror(int pid);
        ~Mirror();
        
        /** Initializes this Mirror object with the given PID. */
        int initWithPID(int pid);
        
        /** Returns the battletag of the current user. */
        BattleTag getBattleTag();
        
        /** Returns the collection of the user. */
        std::vector<Card> getCardCollection();

        /* Returns the information about server */
        InternalGameServerInfo getGameServerInfo();

        /* Returns the game type */
        int getGameType();

        /* Returns the match informations */
        InternalMatchInfo getMatchInfo();

        /* Returns the game format */
        int getFormat();
        
    private:
        mach_port_t _task;
        MonoImage* _monoImage = NULL;
        
        MonoValue getObject(const HMObjectPath& path);
    };
    
}

#endif /* HearthMirror_hpp */
