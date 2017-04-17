//
//  Mirror.cpp
//  Mirror
//
//  Created by Istvan Fehervari on 26/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include <codecvt>

#include "Mirror.hpp"
#include "memhelper.h"
#include "Helpers/offsets.h"

#include "Mono/MonoObject.hpp"
#include "Mono/MonoStruct.hpp"
#include <numeric>
#include <algorithm>

namespace hearthmirror {
    
    Mirror::Mirror() {
        
    }
    
    Mirror::Mirror(int pid, bool isBlocking) {
        initWithPID(pid, isBlocking);
    }
    
    Mirror::~Mirror() {
        delete _monoImage;
    }

    int Mirror::initWithPID(int pid, bool isBlocking) {
        if (_monoImage) delete _monoImage;
        _monoImage = NULL;
        
		// get handle to process
#ifdef __APPLE__
		kern_return_t kret = task_for_pid(mach_task_self(), pid, &_task);
        if (kret!=KERN_SUCCESS) {
            printf("task_for_pid() failed with message %s!\n",mach_error_string(kret));
            return 3;
        }
#else
		_task = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, pid);
#endif
        
        do {
            proc_address baseaddress = getMonoLoadAddress(_task);
            if (baseaddress == 0) return 4;
            
            // we need to find the address of "mono_root_domain"
            proc_address mono_grd_addr = getMonoRootDomainAddr(_task,baseaddress);
            if (mono_grd_addr == 0) return 5;
            
            uint32_t rootDomain;
            
            try {
#ifdef __APPLE__
                rootDomain = ReadUInt32(_task, baseaddress+mono_grd_addr);
#else
                rootDomain = ReadUInt32(_task, mono_grd_addr);
#endif
            } catch (std::runtime_error& err) {
                return 6;
            }
            if (rootDomain == 0) return 7;
            
            uint32_t pImage = 0;
            try {
                // iterate GSList *domain_assemblies;
                uint32_t next = ReadUInt32(_task, rootDomain+kMonoDomainDomainAssemblies); // GList*
                
                while(next != 0) {
                    uint32_t data = ReadUInt32(_task, (proc_address)next);
                    next = ReadUInt32(_task, (proc_address)next + 4);
                    
                    char* name = ReadCString(_task, ReadUInt32(_task, (proc_address)data + kMonoAssemblyName));
                    if(strcmp(name, "Assembly-CSharp") == 0) {
                        free(name);
                        pImage = ReadUInt32(_task, (proc_address)data + kMonoAssemblyImage);
                        break;
                    }
                    free(name);
                }
            } catch (std::runtime_error& err) {
                return 8;
            }
            
            // we have a pointer now to the right assembly image
            try {
                _monoImage = new MonoImage(_task,pImage); // apply life cycle
                if (_monoImage->hasClasses()) break;
                delete _monoImage;
            } catch (std::runtime_error& err) {
                delete _monoImage;
            }
        } while (isBlocking);
        
        return 0;
    }

	static MonoValue nullMonoValue(0);
	
    MonoValue Mirror::getObject(MonoValue from, const HMObjectPath& path) {
        MonoValue mv = from;
        if (IsMonoValueEmpty(mv) || path.size() < 1) return nullMonoValue;

        for (unsigned int i = 0; i< path.size(); i++) {
            MonoObject* mo = mv.value.obj.o;
            mv = (*mo)[path[i]];
            if (IsMonoValueEmpty(mv)) {
                delete mo;
                return nullMonoValue;
            }

            if (i>0) delete mo; // retain the original "from" object
        }
        return mv;
    }
	
    /** Helper function to find MonoObject at the given path. */
    MonoValue Mirror::getObject(const HMObjectPath& path) {
        if (path.size() < 2) return nullMonoValue;
        
        MonoClass* baseclass = _monoImage->get(path[0]); // no need to free
        if (!baseclass) return nullMonoValue;
        
        MonoValue mv = (*baseclass)[path[1]];
        if (IsMonoValueEmpty(mv)) return nullMonoValue;

        for (unsigned int i = 2; i< path.size(); i++) {
            MonoObject* mo = mv.value.obj.o;
            mv = (*mo)[path[i]];
            if (IsMonoValueEmpty(mv)) {
                delete mo;
                return nullMonoValue;
            }
            
            delete mo;
        }
        return mv;
    }

    /** Helper to get an int */
    int Mirror::getInt(const HMObjectPath& path) {
        MonoValue mv = getObject(path);
        if (IsMonoValueEmpty(mv)) return 0;
        int value = mv.value.i32;

        DeleteMonoValue(mv);
        return value;
    }

    int Mirror::getInt(MonoValue from, const HMObjectPath& path) {
        MonoValue mv = getObject(from, path);
        if (IsMonoValueEmpty(mv)) return 0;
        int value = mv.value.i32;

        DeleteMonoValue(mv);
        return value;
    }

    /** Helper to get a long */
    long Mirror::getLong(const HMObjectPath& path) {
        MonoValue mv = getObject(path);
        if (IsMonoValueEmpty(mv)) return 0;
        long value = mv.value.i64;

        DeleteMonoValue(mv);
        return value;
    }

    long Mirror::getLong(MonoValue from, const HMObjectPath& path) {
        MonoValue mv = getObject(from, path);
        if (IsMonoValueEmpty(mv)) return 0;
        long value = mv.value.i64;

        DeleteMonoValue(mv);
        return value;
    }

    /** Helper to get a bool */
    bool Mirror::getBool(const HMObjectPath& path) {
        MonoValue mv = getObject(path);
        if (IsMonoValueEmpty(mv)) return false;
        bool value = mv.value.b;

        DeleteMonoValue(mv);
        return value;
    }

    bool Mirror::getBool(MonoValue from, const HMObjectPath& path) {
        MonoValue mv = getObject(from, path);
        if (IsMonoValueEmpty(mv)) return false;
        bool value = mv.value.b;

        DeleteMonoValue(mv);
        return value;
    }
    
    BattleTag Mirror::getBattleTag() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        MonoValue mv = getObject({"BnetPresenceMgr","s_instance","m_myPlayer","m_account","m_battleTag"});
        if (IsMonoValueEmpty(mv)) throw std::domain_error("Bnet manager can't be found");
        
        MonoObject* m_battleTag = mv.value.obj.o;

        BattleTag result;
        result.name = ((*m_battleTag)["m_name"]).str;
        result.number = ((*m_battleTag)["m_number"]).value.i32;

        DeleteMonoValue(mv);

        return result;
    }

    AccountId Mirror::getAccountId() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue mv = getObject({"BnetPresenceMgr","s_instance","m_myGameAccountId"});
        if (IsMonoValueEmpty(mv)) throw std::domain_error("BNet presence manager can't be found");

        MonoObject* m_accountId = mv.value.obj.o;
        AccountId account;
        account.lo = ((*m_accountId)["m_lo"]).value.i64;
        account.hi = ((*m_accountId)["m_hi"]).value.i64;

        DeleteMonoValue(mv);
        return account;
    }

    InternalGameServerInfo Mirror::getGameServerInfo() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue mv = getObject({"Network","s_instance","m_lastGameServerInfo"});
        if (IsMonoValueEmpty(mv)) throw std::domain_error("Game server info can't be found");
        MonoObject* m_serverInfo = mv.value.obj.o;

        InternalGameServerInfo result;
        result.address = ((*m_serverInfo)["<Address>k__BackingField"]).str;
        result.auroraPassword = ((*m_serverInfo)["<AuroraPassword>k__BackingField"]).str;
        result.clientHandle = ((*m_serverInfo)["<ClientHandle>k__BackingField"]).value.i64;
        result.gameHandle = ((*m_serverInfo)["<GameHandle>k__BackingField"]).value.i32;
        result.mission = ((*m_serverInfo)["<Mission>k__BackingField"]).value.i32;
        result.port = ((*m_serverInfo)["<Port>k__BackingField"]).value.i32;
        result.resumable = ((*m_serverInfo)["<Resumable>k__BackingField"]).value.b;
        result.spectatorMode = ((*m_serverInfo)["<SpectatorMode>k__BackingField"]).value.b;
        result.spectatorPassword = ((*m_serverInfo)["<SpectatorPassword>k__BackingField"]).str;
        result.version = ((*m_serverInfo)["<Version>k__BackingField"]).str;

        DeleteMonoValue(mv);
        return result;
    }

    int Mirror::getGameType() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        return getInt({"GameMgr","s_instance","m_gameType"});
    }

    int Mirror::getFormat() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        return getInt({"GameMgr","s_instance","m_formatType"});
    }

    bool Mirror::isSpectating() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        return getBool({"GameMgr","s_instance","m_spectator"});
    }

    InternalMatchInfo Mirror::getMatchInfo() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        InternalMatchInfo matchInfo;

        MonoValue gameState = getObject({"GameState","s_instance"});
        MonoValue netCacheValues = getObject({"NetCache","s_instance","m_netCache","valueSlots"});
        if (!IsMonoValueEmpty(gameState)) {
            MonoValue playerIds = getObject(gameState, {"m_playerMap","keySlots"});
            MonoValue players = getObject(gameState, {"m_playerMap","valueSlots"});

            if (!IsMonoValueEmpty(players) && !IsMonoValueEmpty(playerIds)
                && IsMonoValueArray(players) && IsMonoValueArray(playerIds)) {

                for (unsigned int i=0; i< playerIds.arrsize; i++) {
                    MonoValue mv = players[i];
                    if (IsMonoValueEmpty(mv)) continue;
                    MonoObject* inst = mv.value.obj.o;

                    MonoClass* instclass = inst->getClass();
                    std::string icname = instclass->getName();
                    delete instclass;

                    if (icname != "Player") {
                        DeleteMonoValue(mv);
                        continue;
                    }

                    MonoValue _medalInfo = (*inst)["m_medalInfo"];
                    MonoObject *medalInfo = _medalInfo.value.obj.o;

                    int sRank = 0;
                    int sLegendRank = 0;
                    int wRank = 0;
                    int wLegendRank = 0;

                    // spectated games have no medalinfo
                    if (medalInfo != NULL) {

                        MonoValue _sMedalInfo = (*medalInfo)["m_currMedalInfo"];
                        if (!IsMonoValueEmpty(_sMedalInfo)) {
                            MonoObject *sMedalInfo = _sMedalInfo.value.obj.o;
                            MonoValue rank = (*sMedalInfo)["rank"];
                            if (!IsMonoValueEmpty(rank)) {
                                sRank = rank.value.i32;
                                DeleteMonoValue(rank);
                            }
                            MonoValue legendRank = (*sMedalInfo)["legendIndex"];
                            if (!IsMonoValueEmpty(legendRank)) {
                                sLegendRank = legendRank.value.i32;
                                DeleteMonoValue(legendRank);
                            }

                            DeleteMonoValue(_sMedalInfo);
                        }

                        MonoValue _wMedalInfo = (*medalInfo)["m_currWildMedalInfo"];
                        if (!IsMonoValueEmpty(_wMedalInfo)) {
                            MonoObject *wMedalInfo = _wMedalInfo.value.obj.o;
                            MonoValue rank = (*wMedalInfo)["rank"];
                            if (!IsMonoValueEmpty(rank)) {
                                wRank = rank.value.i32;
                                DeleteMonoValue(rank);
                            }
                            MonoValue legendRank = (*wMedalInfo)["legendIndex"];
                            if (!IsMonoValueEmpty(legendRank)) {
                                wLegendRank = legendRank.value.i32;
                                DeleteMonoValue(legendRank);
                            }

                            DeleteMonoValue(_wMedalInfo);
                        }
                    }

                    std::u16string name = ((*inst)["m_name"]).str;
                    if (name.empty()) throw std::domain_error("Found a player with an empty name");

                    int cardBack = ((*inst)["m_cardBackId"]).value.i32;
                    int id = playerIds[i].value.i32;

                    int side = ((*inst)["m_side"]).value.i32;
                    if (side == 1) {
                        int sStars = 0;
                        int wStars = 0;

                        MonoValue netCacheMedalInfo;
                        for (unsigned int i=0; i< netCacheValues.arrsize; i++) {
                            MonoValue netCache = netCacheValues[i];
                            MonoObject* net = netCache.value.obj.o;
                            if (net == NULL) {
                                continue;
                            }
                            MonoClass* instclass = net->getClass();
                            std::string icname = instclass->getName();
                            delete instclass;

                            if (icname != "NetCacheMedalInfo") {
                                continue;
                            }

                            netCacheMedalInfo = netCache;
                            break;
                        }
                        if (!IsMonoValueEmpty(netCacheMedalInfo)) {
                            MonoObject *net = netCacheMedalInfo.value.obj.o;
                            if (net != NULL) {
                                MonoValue vm = (*net)["<Standard>k__BackingField"];
                                if (!IsMonoValueEmpty(vm)) {
                                    MonoObject* stars = vm.value.obj.o;
                                    sStars = ((*stars)["<Stars>k__BackingField"]).value.i32;
                                    DeleteMonoValue(vm);
                                }
                                vm = (*net)["<Wild>k__BackingField"];
                                if (!IsMonoValueEmpty(vm)) {
                                    MonoObject* stars = vm.value.obj.o;
                                    wStars = ((*stars)["<Stars>k__BackingField"]).value.i32;
                                    DeleteMonoValue(vm);
                                }
                            }
                        }

                        matchInfo.localPlayer.name = name;
                        matchInfo.localPlayer.id = id;
                        matchInfo.localPlayer.standardRank = sRank;
                        matchInfo.localPlayer.standardLegendRank = sLegendRank;
                        matchInfo.localPlayer.standardStars = sStars;
                        matchInfo.localPlayer.wildRank = wRank;
                        matchInfo.localPlayer.wildLegendRank = wLegendRank;
                        matchInfo.localPlayer.wildStars = wStars;
                        matchInfo.localPlayer.cardBackId = cardBack;
                    } else {
                        matchInfo.opposingPlayer.name = name;
                        matchInfo.opposingPlayer.id = id;
                        matchInfo.opposingPlayer.standardRank = sRank;
                        matchInfo.opposingPlayer.standardLegendRank = sLegendRank;
                        matchInfo.opposingPlayer.standardStars = 0;
                        matchInfo.opposingPlayer.wildRank = wRank;
                        matchInfo.opposingPlayer.wildLegendRank = wLegendRank;
                        matchInfo.opposingPlayer.wildStars = 0;
                        matchInfo.opposingPlayer.cardBackId = cardBack;
                    }
                    
                    DeleteMonoValue(_medalInfo);
                }

                DeleteMonoValue(playerIds);
                DeleteMonoValue(players);
            }
            DeleteMonoValue(gameState);
        }

        MonoValue _gameMgr = getObject({"GameMgr","s_instance"});
        if (!IsMonoValueEmpty(_gameMgr)) {
            MonoObject *gameMgr = _gameMgr.value.obj.o;
            if (gameMgr != NULL) {
                matchInfo.missionId = getInt(_gameMgr, {"m_missionId"});
                matchInfo.gameType = getInt(_gameMgr, {"m_gameType"});
                matchInfo.formatType = getInt(_gameMgr, {"m_formatType"});

                int brawlGameTypes[] = {16, 17, 18};
                size_t size = sizeof(brawlGameTypes) / sizeof(int);
                int *end = brawlGameTypes + size;
                int *found = std::find(brawlGameTypes, end, 0);
                if (found != end) {
                    MonoValue mission = getCurrentBrawlMission();
                    matchInfo.brawlSeasonId = getInt(mission, {"tavernBrawlSpec","<SeasonId>k__BackingField"});
                    DeleteMonoValue(mission);
                }
            }

            DeleteMonoValue(_gameMgr);
        }

        for (unsigned int i=0; i< netCacheValues.arrsize; i++) {
            MonoValue netCache = netCacheValues[i];
            if (IsMonoValueEmpty(netCache)) continue;
            MonoObject* net = netCache.value.obj.o;
            MonoClass* instclass = net->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "NetCacheRewardProgress") {
                continue;
            }

            matchInfo.rankedSeasonId = ((*net)["<Season>k__BackingField"]).value.i32;
            break;
        }

        DeleteMonoValue(netCacheValues);

        return matchInfo;
    }

    MonoValue Mirror::getCurrentBrawlMission() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue missions = getObject({"TavernBrawlManager","s_instance","m_missions"});
        if (IsMonoValueEmpty(missions) || !IsMonoValueArray(missions)) { return NULL; }
        MonoValue record(0);
        for (unsigned int i=0; i< missions.arrsize; i++) {
            MonoValue mission = missions[i];
            if (IsMonoValueEmpty(mission)) continue;
            MonoObject* _mission = mission.value.obj.o;
            MonoClass* instclass = _mission->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "TavernBrawlMission") {
                continue;
            }

            return mission;
        }

        return nullMonoValue;
    }

    BrawlInfo Mirror::getBrawlInfo() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue mission = getCurrentBrawlMission();
        if (IsMonoValueEmpty(mission)) throw std::domain_error("Current brawl not found");

        BrawlInfo result;
        MonoObject *_mission = mission.value.obj.o;
        if (_mission == NULL) {
            DeleteMonoValue(mission);
            throw std::domain_error("Current brawl not found");
        }
        result.maxWins = ((*_mission)["_MaxWins"]).value.i32;
        result.maxLosses = ((*_mission)["_MaxLosses"]).value.i32;

        MonoValue records = getObject({"TavernBrawlManager","s_instance", "m_playerRecords"});
        if (IsMonoValueEmpty(records) || !IsMonoValueArray(records))
            throw std::domain_error("Brawl manager can't be found");

        MonoValue record(0);
        for (unsigned int i=0; i< records.arrsize; i++) {
            MonoValue r = records[i];
            if (IsMonoValueEmpty(r)) continue;
            MonoObject* _record = r.value.obj.o;
            MonoClass* instclass = _record->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "TavernBrawlPlayerRecord") {
                continue;
            }

            record = r;
            break;
        }
        if (IsMonoValueEmpty(record)) throw std::domain_error("Can't get record");

        result.gamesPlayed = getInt(record, {"_GamesPlayed"});
        result.winStreak = getInt(record, {"_WinStreak"});

        MonoObject *_record = record.value.obj.o;
        if (_record == NULL) {
            DeleteMonoValue(record);
            DeleteMonoValue(records);
            throw std::domain_error("Can't get record");
        }

        result.gamesPlayed = ((*_record)["_GamesPlayed"]).value.i32;
        result.winStreak = ((*_record)["_WinStreak"]).value.i32;
        result.isSessionBased = result.maxWins > 0 || result.maxLosses > 0;
        if (result.isSessionBased) {
            if (!((*_record)["HasSession"]).value.b) {
                DeleteMonoValue(mission);
                DeleteMonoValue(records);
                DeleteMonoValue(record);
                return result;
            }

            MonoValue session = getObject(record, {"_Session"});
            result.wins = getInt(session, {"<Wins>k__BackingField"});
            result.losses = getInt(session, {"<Losses>k__BackingField"});
            DeleteMonoValue(session);
        } else {
            result.wins = getInt(record, {"<GamesWon>k__BackingField"});
            result.losses = result.gamesPlayed - result.wins;
        }

        DeleteMonoValue(mission);
        DeleteMonoValue(records);
        return result;
    }

    Deck Mirror::getEditedDeck() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue taggedDecks = getObject({"CollectionManager","s_instance","m_taggedDecks"});
        if (IsMonoValueEmpty(taggedDecks)) {
            throw std::domain_error("Collection manager can't be found");
        }
        MonoObject *_taggedDecks = taggedDecks.value.obj.o;
        MonoValue tags = (*_taggedDecks)["keySlots"];
        MonoValue decks = (*_taggedDecks)["valueSlots"];
        if (IsMonoValueEmpty(tags) || IsMonoValueEmpty(decks)) {
            throw std::domain_error("No edited deck ?");
        }
        for (unsigned int i = 0; i < tags.arrsize; i++) {
            MonoValue tag = tags[i];
            MonoValue deck = decks[i];
            if (IsMonoValueEmpty(tag) || IsMonoValueEmpty(deck)) continue;

            MonoStruct *_tag = tag.value.obj.s;
            try {
                int v = ((*_tag)["value__"]).value.i32;
                if (v == 0) {
                    Deck ddeck = getDeck(deck.value.obj.o);
                    DeleteMonoValue(tags);
                    DeleteMonoValue(decks);
                    DeleteMonoValue(taggedDecks);
                    return ddeck;
                }
            } catch (std::exception& ex) {
                continue;
            }
        }

        DeleteMonoValue(tags);
        DeleteMonoValue(decks);
        DeleteMonoValue(taggedDecks);
        throw std::domain_error("No edited deck ?");
    }

    std::vector<Deck> Mirror::getDecks() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue values = getObject({"CollectionManager","s_instance","m_decks","valueSlots"});
        if (IsMonoValueEmpty(values) || !IsMonoValueArray(values)) {
            throw std::domain_error("Collection manager can't be found");
        }

        std::vector<Deck> result;

        for (unsigned int i=0; i< values.arrsize; i++) {
            MonoValue mv = values[i];
            if (IsMonoValueEmpty(mv)) continue;
            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "CollectionDeck") {
                continue;
            }

            Deck deck = getDeck(inst);
            // count cards
            int sum = std::accumulate(begin(deck.cards), end(deck.cards), 0,
                              [](const int& x, const Card& y) { return x + y.count; });
            if (deck.cards.size() == 0 || sum != 30) {
                continue;
            }

            // don't add the same deck multiple times
            auto iterator = std::find_if(result.begin(), result.end(),
                                    [&deck](const Deck& obj) { return obj.id == deck.id; });
            if (iterator == result.end()) {
                result.push_back(deck);
            }
        }

        DeleteMonoValue(values);
        return result;
    }

    long Mirror::getSelectedDeckInMenu() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        return getLong({"DeckPickerTrayDisplay","s_instance","m_selectedCustomDeckBox","m_deckID"});
    }

    Deck Mirror::getDeck(MonoObject* inst) {
        Deck deck;

        deck.id = ((*inst)["ID"]).value.i64;
        deck.name = ((*inst)["m_name"]).str;
        deck.hero = ((*inst)["HeroCardID"]).str;
        deck.isWild = ((*inst)["m_isWild"]).value.b;
        deck.type = ((*inst)["Type"]).value.i32;
        deck.seasonId = ((*inst)["SeasonId"]).value.i32;
        deck.cardBackId = ((*inst)["CardBackID"]).value.i32;
        deck.heroPremium = ((*inst)["HeroPremium"]).value.i32;

        MonoValue _cardList = (*inst)["m_slots"];
        if (IsMonoValueEmpty(_cardList)) return deck;

        MonoObject *cardList = _cardList.value.obj.o;

        MonoValue cards = (*cardList)["_items"];
        MonoValue sizemv = (*cardList)["_size"];
        if (IsMonoValueEmpty(cards) || IsMonoValueEmpty(sizemv)) {
            DeleteMonoValue(cards);
            DeleteMonoValue(sizemv);
            DeleteMonoValue(_cardList);
            return deck;
        }
        int size = sizemv.value.i32;
        for (int i = 0; i < size; i++) {
            MonoObject *card = cards[i].value.obj.o;

            std::u16string name = ((*card)["m_cardId"]).str;
            int count = ((*card)["m_count"]).value.i32;

            auto iterator = find_if(deck.cards.begin(), deck.cards.end(),
                              [&name](const Card& obj) { return obj.id == name; });
            if (iterator != deck.cards.end()) {
                auto index = std::distance(deck.cards.begin(), iterator);
                Card c = deck.cards[index];
                c.count += count;
                deck.cards[index] = c;
            } else {
                Card c = Card(name, count, false);
                deck.cards.push_back(c);
            }
        }

        DeleteMonoValue(cards);
        DeleteMonoValue(sizemv);
        DeleteMonoValue(_cardList);

        return deck;
    }

    std::vector<Card> Mirror::getPackCards() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue cards = getObject({"PackOpening","s_instance","m_director","m_hiddenCards","_items"});
        if (IsMonoValueEmpty(cards) || !IsMonoValueArray(cards)) {
            throw std::domain_error("Pack opening informations can't be found");
        }

        std::vector<Card> result;
        for (unsigned int i = 0; i < cards.arrsize; i++) {
            MonoValue mv = cards[i];
            if (IsMonoValueEmpty(mv)) continue;

            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "PackOpeningCard") continue;
            MonoValue def = getObject(mv, {"m_boosterCard","<Def>k__BackingField"});
            if (IsMonoValueEmpty(def)) continue;
            
            MonoObject *_def = def.value.obj.o;
            MonoValue namemv = (*_def)["<Name>k__BackingField"];
            MonoValue premiummv = (*_def)["<Premium>k__BackingField"];
            if (IsMonoValueEmpty(namemv) || IsMonoValueEmpty(premiummv)) {
                continue;
            }

            std::u16string name = namemv.str;
            bool premium = premiummv.value.i32 > 0;
            result.push_back(Card(name, 1, premium));
        }

        return result;
    }

    ArenaInfo Mirror::getArenaDeck() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        ArenaInfo result;

        MonoValue _draftManager = getObject({"DraftManager","s_instance"});
        if (IsMonoValueEmpty(_draftManager)) throw std::domain_error("Draft manager can't be found");
        MonoObject* draftManager = _draftManager.value.obj.o;

        MonoValue draftDeck = (*draftManager)["m_draftDeck"];
        if (IsMonoValueEmpty(draftDeck)) {
            DeleteMonoValue(_draftManager);
            throw std::domain_error("Draft deck can't be found");
        }

        MonoObject* inst = draftDeck.value.obj.o;
        Deck deck = getDeck(inst);
        result.deck = deck;

        MonoValue wins = (*draftManager)["m_wins"];
        MonoValue losses = (*draftManager)["m_losses"];
        MonoValue currentSlot = (*draftManager)["m_currentSlot"];

        result.wins = wins.value.i32;
        result.losses = losses.value.i32;
        result.currentSlot = currentSlot.value.i32;
        MonoValue chest = (*draftManager)["m_chest"];
        if (!IsMonoValueEmpty(chest)) {
            MonoObject *_chest = chest.value.obj.o;
            MonoValue rewards = (*_chest)["<Rewards>k__BackingField"];
            if (IsMonoValueEmpty(rewards)) {
                DeleteMonoValue(chest);
            } else {
                MonoObject *_rewards = rewards.value.obj.o;
                MonoValue items = (*_rewards)["_items"];
                if (!IsMonoValueEmpty(items) && IsMonoValueArray(items)) {
                    result.rewards = parseRewards(items);
                    DeleteMonoValue(items);
                }

                DeleteMonoValue(rewards);
            }
            DeleteMonoValue(chest);
        }

        DeleteMonoValue(draftDeck);
        DeleteMonoValue(_draftManager);
        return result;
    }

    std::vector<Card> Mirror::getArenaDraftChoices() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        std::vector<Card> result;
        MonoValue values = getObject({"DraftDisplay","s_instance","m_choices"});
        if (IsMonoValueEmpty(values)) throw std::domain_error("Draft choices can't be found");

        MonoObject *stacks = values.value.obj.o;
        MonoValue choices = (*stacks)["_items"];
        MonoValue sizemv = (*stacks)["_size"];

        int size = sizemv.value.i32;
        for (unsigned int i = 0; i < size; i++) {
            MonoValue mv = getObject(choices[i], {"m_actor","m_entityDef","m_cardId"});
            if (IsMonoValueEmpty(mv)) continue;

            result.push_back(Card(mv.str, 1, false));
        }

        DeleteMonoValue(values);
        return result;
    }

    std::vector<RewardData*> Mirror::parseRewards(MonoValue items) {
        std::vector<RewardData*> result;

        for (unsigned int i = 0; i < items.arrsize; i++) {
            MonoValue mv = items[i];
            if (IsMonoValueEmpty(mv)) continue;

            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname == "ArcaneDustRewardData") {
                ArcaneDustRewardData *data = new ArcaneDustRewardData();
                data->type = ARCANE_DUST;
                data->amount = ((*inst)["<Amount>k__BackingField"]).value.i32;
                result.push_back(data);
            } else if (icname == "BoosterPackRewardData") {
                BoosterPackRewardData *data = new BoosterPackRewardData();
                data->id = ((*inst)["<Id>k__BackingField"]).value.i32;
                data->type = BOOSTER_PACK;
                data->count = ((*inst)["<Count>k__BackingField"]).value.i32;
                result.push_back(data);
            } else if (icname == "CardRewardData") {
                CardRewardData *data = new CardRewardData();
                data->id = ((*inst)["<CardID>k__BackingField"]).str;
                data->count = ((*inst)["<Count>k__BackingField"]).value.i32;
                data->premium = ((*inst)["<Premium>k__BackingField"]).value.i32 > 0;
                data->type = CARD;
                result.push_back(data);
            } else if (icname == "CardBackRewardData") {
                CardBackRewardData *data = new CardBackRewardData();
                data->id = ((*inst)["<CardBackID>k__BackingField"]).value.i32;
                data->type = CARD_BACK;
                result.push_back(data);
            } else if (icname == "ForgeTicketRewardData") {
                ForgeTicketRewardData *data = new ForgeTicketRewardData();
                data->quantity = ((*inst)["<Quantity>k__BackingField"]).value.i32;
                data->type = FORGE_TICKET;
                result.push_back(data);
            } else if (icname == "GoldRewardData") {
                GoldRewardData *data = new GoldRewardData();
                data->amount = ((*inst)["<Amount>k__BackingField"]).value.i32;
                data->type = GOLD;
                result.push_back(data);
            } else if (icname == "MountRewardData") {
                MountRewardData *data = new MountRewardData();
                data->mountType = ((*inst)["<Mount>k__BackingField"]).value.i32;
                data->type = MOUNT;
                result.push_back(data);
            }
        }

        return result;
    }
    
    std::vector<Card> Mirror::getCardCollection() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");

        MonoValue valueSlots = getObject({"NetCache","s_instance","m_netCache","valueSlots"});
        if (IsMonoValueEmpty(valueSlots) || !IsMonoValueArray(valueSlots)) {
            throw std::domain_error("Net cache can't be found");
        }

        std::vector<Card> result;
        
        for (unsigned int i=0; i< valueSlots.arrsize; i++) {
            MonoValue mv = valueSlots[i];
            if (IsMonoValueEmpty(mv)) continue;
            MonoObject* inst = mv.value.obj.o;
            MonoClass* instclass = inst->getClass();
            std::string icname = instclass->getName();
            delete instclass;
            if (icname == "NetCacheCollection") {
                MonoValue stacksmv = (*inst)["<Stacks>k__BackingField"];
                if (IsMonoValueEmpty(stacksmv)) break;

                MonoObject* stacks = stacksmv.value.obj.o;
                MonoValue itemsmv = (*stacks)["_items"];
                MonoValue sizemv = (*stacks)["_size"];
				if (IsMonoValueEmpty(itemsmv) || IsMonoValueEmpty(sizemv)) break;
                int size = sizemv.value.i32;
                for (int i=0; i< size; i++) { // or itemsmv.arrsize?
                    MonoValue stackmv = itemsmv.value.arr[i];
                    if (IsMonoValueEmpty(stackmv)) continue;
                    MonoObject* stack = stackmv.value.obj.o;

                    MonoValue countmv = (*stack)["<Count>k__BackingField"];
                    if (IsMonoValueEmpty(countmv)) {
                        continue;
                    }
                    int count = countmv.value.i32;
                    
                    MonoValue defmv = (*stack)["<Def>k__BackingField"];
                    if (IsMonoValueEmpty(defmv)) {
                        DeleteMonoValue(countmv);
                        continue;
                    }
                    MonoObject* def = defmv.value.obj.o;
                    
                    MonoValue namemv = (*def)["<Name>k__BackingField"];
                    MonoValue premiummv = (*def)["<Premium>k__BackingField"];
					if (IsMonoValueEmpty(namemv) || IsMonoValueEmpty(premiummv)) {
                        DeleteMonoValue(countmv);
                        DeleteMonoValue(defmv);
                        continue;
                    }
                    
                    std::u16string name = namemv.str;
                    bool premium = premiummv.value.b;
                    result.push_back(Card(name,count,premium));
                    
                    DeleteMonoValue(countmv);
                    DeleteMonoValue(defmv);
                    DeleteMonoValue(namemv);
                    DeleteMonoValue(premiummv);
                }
                DeleteMonoValue(sizemv);
                DeleteMonoValue(itemsmv);
                DeleteMonoValue(stacksmv);
                
            }
        }
        
        // free all memory
        DeleteMonoValue(valueSlots);
        
        return result;
    }
    
    bool Mirror::isFriendsListVisible() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"ChatMgr","s_instance","m_friendListFrame"});
    }
    
    bool Mirror::isGameMenuVisible() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"GameMenu","s_instance","m_isShown"});
    }
    
    bool Mirror::isOptionsMenuVisible() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"OptionsMenu","s_instance","m_isShown"});
    }
    
    bool Mirror::isMulligan() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"MulliganManager","s_instance","mulliganChooseBanner"});
    }
    
    int Mirror::getNumMulliganCards() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"MulliganManager","s_instance","m_startingCards","_size"});
    }
    
    bool Mirror::isChoosingCard() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"ChoiceCardMgr","s_instance","m_subOptionState"}) ||
                getInt({"ChoiceCardMgr","s_instance","m_choiceStateMap","count"}) > 0;
    }

    int Mirror::getNumChoiceCards() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"ChoiceCardMgr","s_instance","m_lastShownChoices","_size"});
    }
    
    bool Mirror::isPlayerEmotesVisible() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"EmoteHandler","s_instance","m_emotesShown"});
    }
    
    bool Mirror::isEnemyEmotesVisible() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"EnemyEmoteHandler","s_instance","m_emotesShown"});
    }
    
    bool Mirror::isInBattlecryEffect() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"InputManager","s_instance","m_isInBattleCryEffect"});
    }
    
    bool Mirror::isDragging() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"InputManager","s_instance","m_dragging"});
    }
    
    bool Mirror::isTargetingHeroPower() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"InputManager","s_instance","m_targettingHeroPower"});
    }
    
    int Mirror::getBattlecrySourceCardZonePosition() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"InputManager","s_instance","m_battlecrySourceCard","m_zonePosition"});
    }
    
    bool Mirror::isHoldingCard() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"InputManager","s_instance","m_heldCard"});
    }
    
    bool Mirror::isTargetReticleActive() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"TargetReticleManager","s_instance","m_isActive"});
    }
    
    bool Mirror::isEnemyTargeting() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"InputManager","s_instance","m_isEnemyArrow"});
    }
    
    bool Mirror::isGameOver() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"GameState","s_instance","m_gameOver"});
    }
    
    bool Mirror::isInMainMenu() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"Box","s_instance","m_state"}) == (int)kBoxStateHubWithDrawer;
    }
    
    UI_WINDOW Mirror::getShownUiWindowId() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return (UI_WINDOW)getInt({"ShownUIMgr","s_instance","m_shownUI"});
    }
    
    SceneMode Mirror::GetCurrentSceneMode() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return (SceneMode)getInt({"SceneMgr","s_instance","m_mode"});
    }
    
    bool Mirror::isPlayerHandZoneUpdatingLayout() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"InputManager","s_instance","m_myHandZone", "m_updatingLayout"});
    }
    
    bool Mirror::isPlayerPlayZoneUpdatingLayout() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getBool({"InputManager","s_instance","m_myPlayZone", "m_updatingLayout"});
    }
    
    int Mirror::getNumCardsPlayerHand() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"InputManager","s_instance","m_myHandZone","m_cards","_size"});
    }
    
    int Mirror::getNumCardsPlayerBoard() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"InputManager","s_instance","m_myPlayZone","m_cards","_size"});
    }
    
    int Mirror::getNavigationHistorySize() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"Navigation","history","_size"});
    }
    
    int Mirror::getCurrentManaFilter() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"CollectionManagerDisplay","s_instance","m_manaTabManager","m_currentFilterValue"});
    }
    
    SetFilterItem Mirror::getCurrentSetFilter() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        MonoValue item = getObject({"CollectionManagerDisplay","s_instance","m_setFilterTray","m_selected"});
        if (IsMonoValueEmpty(item)) {
            DeleteMonoValue(item);
            return SetFilterItem();
        }
        
        DeleteMonoValue(item);
        SetFilterItem result = SetFilterItem();
        
        result.isAllStandard = getBool({"CollectionManagerDisplay","s_instance","m_setFilterTray","m_selected","m_isAllStandard"});
        result.isWild = getBool({"CollectionManagerDisplay","s_instance","m_setFilterTray","m_selected","m_isWild"});
        return result;
    }
    
    int Mirror::getLastOpenedBoosterId() {
        if (!_monoImage) throw std::domain_error("Mono image can't be found");
        
        return getInt({"PackOpening","s_instance","m_lastOpenedBoosterId"});
    }
    
}
