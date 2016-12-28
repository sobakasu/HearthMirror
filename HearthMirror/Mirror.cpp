//
//  Mirror.cpp
//  Mirror
//
//  Created by Istvan Fehervari on 26/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include <codecvt>

#include "Mirror.hpp"
#include "machhelper.h"
#include "offsets.h"

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
        
        kern_return_t kret;
        
        kret = task_for_pid(mach_task_self(), pid, &_task);
        if (kret!=KERN_SUCCESS) {
            printf("task_for_pid() failed with message %s!\n",mach_error_string(kret));
            return 3;
        }
        
        vm_address baseaddress = getMonoLoadAddress(_task);
        if (baseaddress == 0) return 4;
        
        // we need to find the address of "mono_root_domain"
        vm_address mono_grd_addr = getMonoRootDomainAddr(_task,baseaddress);
        if (mono_grd_addr == 0) return 5; // ASSERT
        
        uint32_t rootDomain = ReadUInt32(_task, baseaddress+mono_grd_addr);
        
        // iterate GSList *domain_assemblies;
        uint32_t next = ReadUInt32(_task, rootDomain+kMonoDomainDomainAssemblies); // GList*
        uint32_t pImage = 0;
        
        while(next != 0) {
            uint32_t data = ReadUInt32(_task, next);
            next = ReadUInt32(_task, next + 4);
            
            char* name = ReadCString(_task, ReadUInt32(_task, data + kMonoAssemblyName));
            if(strcmp(name, "Assembly-CSharp") == 0) {
                pImage = ReadUInt32(_task, data + kMonoAssemblyImage);
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
    
    std::vector<Card> Mirror::getCardCollection() {
 
        std::vector<Card> result;
        
        MonoValue valueSlots = getObject({"NetCache","s_instance","m_netCache","valueSlots"});
        if (IsMonoValueEmpty(valueSlots) || !IsMonoValueArray(valueSlots)) return result;
        
        for (int i=0; i< valueSlots.arrsize; i++) {
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
                if (IsMonoValueEmpty(itemsmv) || IsMonoValueEmpty(sizemv)) break;
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
                    if (IsMonoValueEmpty(namemv) || IsMonoValueEmpty(premiummv)) {
                        DeleteMonoValue(stackmv);
                        DeleteMonoValue(countmv);
                        DeleteMonoValue(defmv);
                        continue;
                    }
                    
                    std::u16string name = namemv.str;
                    int premium = premiummv.value.i32;
                    result.push_back(Card(name,count,premium));
                    
                    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
                    std::string namestr = convert.to_bytes(name);
                    printf("%s : %d | %d\n",namestr.c_str(),count,premium);
                    
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
