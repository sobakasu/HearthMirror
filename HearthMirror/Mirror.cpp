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

namespace hearthmirror {
    
    Mirror::Mirror() {
        
    }
    
    Mirror::Mirror(int pid) {
        initWithPID(pid);
    }
    
    Mirror::~Mirror() {
        delete _monoImage;
    }

    int Mirror::initWithPID(int pid) {
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
		proc_address baseaddress = getMonoLoadAddress(_task);
        if (baseaddress == 0) return 4;
        
        // we need to find the address of "mono_root_domain"
		proc_address mono_grd_addr = getMonoRootDomainAddr(_task,baseaddress);
        if (mono_grd_addr == 0) return 5; // ASSERT
        
        uint32_t rootDomain = ReadUInt32(_task, baseaddress+mono_grd_addr);
        
        // iterate GSList *domain_assemblies;
        uint32_t next = ReadUInt32(_task, rootDomain+kMonoDomainDomainAssemblies); // GList*
        uint32_t pImage = 0;
        
        while(next != 0) {
            uint32_t data = ReadUInt32(_task, (proc_address)next);
            next = ReadUInt32(_task, (proc_address)next + 4);
            
            char* name = ReadCString(_task, ReadUInt32(_task, (proc_address)data + kMonoAssemblyName));
            if(strcmp(name, "Assembly-CSharp") == 0) {
                pImage = ReadUInt32(_task, (proc_address)data + kMonoAssemblyImage);
                break;
            }
        }
        
        // we have a pointer now to right assembly image
        _monoImage = new MonoImage(_task,pImage); // apply life cycle
        return 0;
    }
    
    /** Helper function to find MonoObject at the given path. */
    MonoValue Mirror::getObject(const HMObjectPath& path) {
        if (path.size() < 2) return NULL;
        
        MonoClass* baseclass = (*_monoImage)[path[0]]; // no need to free
        MonoValue mv = (*baseclass)[path[1]];
        
        // this function might blow up with structs/enums
        for (unsigned int i = 2; i< path.size(); i++) {
            MonoObject* mo = mv.value.obj.o;
            mv = (*mo)[path[i]];
            delete mo;
        }
        return mv;
    }

    /** Helper to get an int */
    int Mirror::getInt(const HMObjectPath& path) {
        MonoValue mv = getObject(path);
        if (IsMonoValueEmpty(mv)) return NULL;
        int value = mv.value.i32;

        DeleteMonoValue(mv);
        return value;
    }

    /** Helper to get a bool */
    bool Mirror::getBool(const HMObjectPath& path) {
        MonoValue mv = getObject(path);
        if (IsMonoValueEmpty(mv)) return NULL;
        bool value = mv.value.b;

        DeleteMonoValue(mv);
        return value;
    }
    
    BattleTag Mirror::getBattleTag() {
        BattleTag result;
        if (!_monoImage) return result;
        
        MonoValue mv = getObject({"BnetPresenceMgr","s_instance","m_myPlayer","m_account","m_battleTag"});
        if (IsMonoValueEmpty(mv)) return result;
        
        MonoObject* m_battleTag = mv.value.obj.o;
        
        result.name = ((*m_battleTag)["m_name"]).str;
        result.number = ((*m_battleTag)["m_number"]).value.i32;

        DeleteMonoValue(mv);

        return result;
    }

    AccountId Mirror::getAccountId() {
        AccountId account;
        MonoValue mv = getObject({"BnetPresenceMgr","s_instance","m_myGameAccountId"});
        if (IsMonoValueEmpty(mv)) return account;

        MonoObject* m_accountId = mv.value.obj.o;
        account.lo = ((*m_accountId)["m_lo"]).value.i64;
        account.hi = ((*m_accountId)["m_hi"]).value.i64;

        DeleteMonoValue(mv);
        return account;
    }

    InternalGameServerInfo Mirror::getGameServerInfo() {
        InternalGameServerInfo result;
        if (!_monoImage) return result;

        MonoValue mv = getObject({"Network","s_instance","m_lastGameServerInfo"});
        if (IsMonoValueEmpty(mv)) return result;
        MonoObject* m_serverInfo = mv.value.obj.o;

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
        return getInt({"GameMgr","s_instance","m_gameType"});
    }

    int Mirror::getFormat() {
        return getInt({"GameMgr","s_instance","m_formatType"});
    }

    bool Mirror::isSpectating() {
        return getBool({"GameMgr","s_instance","m_spectator"});
    }

    InternalMatchInfo Mirror::getMatchInfo() {
        InternalMatchInfo result;
        if (!_monoImage) return result;

        MonoValue playerIds = getObject({"GameState","s_instance","m_playerMap","keySlots"});
        MonoValue players = getObject({"GameState","s_instance","m_playerMap","valueSlots"});
        MonoValue netCacheValues = getObject({"NetCache","s_instance","m_netCache","valueSlots"});
        if (IsMonoValueEmpty(playerIds) || !IsMonoValueArray(playerIds)) return result;
        if (IsMonoValueEmpty(players) || !IsMonoValueArray(players)) return result;
        if (IsMonoValueEmpty(netCacheValues) || !IsMonoValueArray(netCacheValues)) return result;

        for (unsigned int i=0; i< playerIds.arrsize; i++) {
            MonoValue mv = players[i];
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

            int wRank = 0;
            int wLegendRank = 0;
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

            std::u16string name = ((*inst)["m_name"]).str;

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

                    MonoValue vm = (*net)["<Standard>k__BackingField"];
                    MonoObject* stars = vm.value.obj.o;
                    sStars = ((*stars)["<Stars>k__BackingField"]).value.i32;

                    vm = (*net)["<Wild>k__BackingField"];
                    stars = vm.value.obj.o;
                    wStars = ((*stars)["<Stars>k__BackingField"]).value.i32;
                    DeleteMonoValue(netCacheMedalInfo);
                }

                result.localPlayer.name = name;
                result.localPlayer.id = id;
                result.localPlayer.standardRank = sRank;
                result.localPlayer.standardLegendRank = sLegendRank;
                result.localPlayer.standardStars = sStars;
                result.localPlayer.wildRank = wRank;
                result.localPlayer.wildLegendRank = wLegendRank;
                result.localPlayer.wildStars = wStars;
                result.localPlayer.cardBackId = cardBack;
            } else {
                result.opposingPlayer.name = name;
                result.opposingPlayer.id = id;
                result.opposingPlayer.standardRank = sRank;
                result.opposingPlayer.standardLegendRank = sLegendRank;
                result.opposingPlayer.standardStars = 0;
                result.opposingPlayer.wildRank = wRank;
                result.opposingPlayer.wildLegendRank = wLegendRank;
                result.opposingPlayer.wildStars = 0;
                result.opposingPlayer.cardBackId = cardBack;
            }

            DeleteMonoValue(_medalInfo);
            DeleteMonoValue(mv);
        }

        MonoValue _brawlSeasonId = getObject({"TavernBrawlManager","s_instance","m_currentMission", "seasonId"});
        int brawlSeasonId = 0;
        if (!IsMonoValueEmpty(_brawlSeasonId)) {
            brawlSeasonId = _brawlSeasonId.value.i32;
        }
        result.brawlSeasonId = brawlSeasonId;
        DeleteMonoValue(_brawlSeasonId);

        MonoValue _missionId = getObject({"GameMgr","s_instance","m_missionId"});
        int missionId = 0;
        if (!IsMonoValueEmpty(_missionId)) {
            missionId = _missionId.value.i32;
        }
        result.missionId = missionId;
        DeleteMonoValue(_missionId);

        for (unsigned int i=0; i< netCacheValues.arrsize; i++) {
            MonoValue netCache = netCacheValues[i];
            MonoObject* net = netCache.value.obj.o;
            MonoClass* instclass = net->getClass();
            std::string icname = instclass->getName();
            delete instclass;

            if (icname != "NetCacheRewardProgress") {
                continue;
            }

            result.rankedSeasonId = ((*net)["<Season>k__BackingField"]).value.i32;
            break;
        }

        DeleteMonoValue(playerIds);
        DeleteMonoValue(players);
        DeleteMonoValue(netCacheValues);

        return result;
    }
    
    std::vector<Card> Mirror::getCardCollection() {
 
        std::vector<Card> result;
        
        MonoValue valueSlots = getObject({"NetCache","s_instance","m_netCache","valueSlots"});
        if (IsMonoValueEmpty(valueSlots) || !IsMonoValueArray(valueSlots)) return result;
        
        for (unsigned int i=0; i< valueSlots.arrsize; i++) {
            MonoValue mv = valueSlots[i];
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
                        DeleteMonoValue(stackmv);
                        continue;
                    }
                    int count = countmv.value.i32;
                    
                    MonoValue defmv = (*stack)["<Def>k__BackingField"];
                    if (IsMonoValueEmpty(defmv)) {
                        DeleteMonoValue(stackmv);
                        DeleteMonoValue(countmv);
                        continue;
                    }
                    MonoObject* def = defmv.value.obj.o;
                    
                    MonoValue namemv = (*def)["<Name>k__BackingField"];
                    MonoValue premiummv = (*def)["<Premium>k__BackingField"];
					if (IsMonoValueEmpty(namemv) || IsMonoValueEmpty(premiummv)) {
                        DeleteMonoValue(stackmv);
                        DeleteMonoValue(countmv);
                        DeleteMonoValue(defmv);
                        continue;
                    }
                    
                    std::u16string name = namemv.str;
                    bool premium = premiummv.value.b;
                    result.push_back(Card(name,count,premium));
                    
                    //std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
                    //std::string namestr = convert.to_bytes(name);
                    //printf("%s : %d | %d\n",namestr.c_str(),count,premium);
                    
                    DeleteMonoValue(stackmv);
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

}
