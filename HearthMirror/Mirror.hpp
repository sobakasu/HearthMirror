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

#ifdef __APPLE__
#define HEARTHMIRROR_API __attribute__((visibility("default")))
#else
#define HEARTHMIRROR_API __declspec(dllexport)
#endif

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

typedef struct _Deck {
    long id;
    std::u16string name;
    std::u16string hero;
    bool isWild;
    int type;
    int seasonId;
    int cardBackId;
    int heroPremium;
    std::vector<Card> cards;
} Deck;

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

typedef enum _RewardType {
    ARCANE_DUST,
    BOOSTER_PACK,
    CARD,
    CARD_BACK,
    CRAFTABLE_CARD,
    FORGE_TICKET,
    GOLD,
    MOUNT,
    CLASS_CHALLENGE
} RewardType;

typedef struct _RewardData {
    RewardType type;
    virtual ~_RewardData() {}
} RewardData;

typedef struct _ArcaneDustRewardData : RewardData {
    int amount;
} ArcaneDustRewardData;

typedef struct _BoosterPackRewardData : RewardData {
    int id;
    int count;
} BoosterPackRewardData;

typedef struct _CardRewardData : RewardData {
    std::u16string id;
    int count;
    bool premium;
} CardRewardData;

typedef struct _CardBackRewardData : RewardData {
    int id;
} CardBackRewardData;

typedef struct _ForgeTicketRewardData : RewardData {
    int quantity;
} ForgeTicketRewardData;

typedef struct _GoldRewardData : RewardData {
    int amount;
} GoldRewardData;

typedef struct _MountRewardData : RewardData {
    int mountType;
} MountRewardData;

typedef struct _ArenaInfo {
    Deck deck;
    int losses;
    int wins;
    int currentSlot;
    std::vector<RewardData*> rewards;
} ArenaInfo;

typedef struct _AccountId {
    long hi = 0;
    long lo = 0;
} AccountId;

typedef struct _BrawlInfo {
    int maxWins = -1;
    int maxLosses = -1;
    bool isSessionBased;
    int wins;
    int losses;
    int gamesPlayed;
    int winStreak;
} BrawlInfo;

namespace hearthmirror {

	class HEARTHMIRROR_API Mirror {
        
    public:
        
        Mirror();
        Mirror(int pid);
        ~Mirror();
        
        bool isBlocking() {return this->_isBlocking;}
        void setBlocking(bool blocking) {this->_isBlocking = blocking;}
        
        /** Initializes this Mirror object with the given PID. */
        int initWithPID(int pid);
        
        /** Returns the battletag of the current user. */
        BattleTag getBattleTag();
        
        /** Returns the collection of the user. */
        std::vector<Card> getCardCollection();

        /** Returns the information about server */
        InternalGameServerInfo getGameServerInfo();

        /** Returns the game type */
        int getGameType();

        /** Returns the match informations */
        InternalMatchInfo getMatchInfo();

        /** Returns the game format */
        int getFormat();

        /** Check if spectating */
        bool isSpectating();

        /** Returns the account id */
        AccountId getAccountId();

        /** Returns the decks */
        std::vector<Deck> getDecks();

        /** Returns the selected deck */
        long getSelectedDeckInMenu();

        /** Returns an arena deck */
        ArenaInfo getArenaDeck();

        /** Returns the choice for the arena draft */
        std::vector<Card> getArenaDraftChoices();

        /** Returns cards from an opening pack */
        std::vector<Card> getPackCards();

        /** Returns the informations about brawl */
        BrawlInfo getBrawlInfo();

        /** Returns the edited deck */
        Deck getEditedDeck();

    private:
		HANDLE _task;
        MonoImage* _monoImage = NULL;
        bool _isBlocking = true;
        
        MonoValue getObject(const HMObjectPath& path);
        MonoValue getObject(MonoValue from, const HMObjectPath& path);
        int getInt(const HMObjectPath& path);
        bool getBool(const HMObjectPath& path);
        long getLong(const HMObjectPath& path);

        Deck getDeck(MonoObject* inst);
        std::vector<RewardData*> parseRewards(MonoValue items);
    };
    
}

#endif /* HearthMirror_hpp */
