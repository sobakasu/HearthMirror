//
//  MonoImage.cpp
//  MonoReader
//
//  Created by Istvan Fehervari on 22/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "MonoImage.hpp"
#include "../Helpers/offsets.h"

namespace hearthmirror {
    
    MonoImage::MonoImage(HANDLE task, uint32_t pImage) : _task(task), _pImage(pImage) {
    
        this->LoadClasses();
    }

    MonoImage::~MonoImage() {
        // free all classes
        for (auto it = _classes.begin(); it != _classes.end(); it++) {
            delete it->second;
        }
        _classes.clear();
    }
    
    bool MonoImage::hasClasses() {
        size_t size = this->_classes.size();
        return size > 0;
    }

    MonoClass* MonoImage::get(const std::string& key) {
        if (this->_classes.count(key)) {
            return this->_classes.at(key);
        }
        return NULL;
    }

    void MonoImage::LoadClasses() {
    
        for (auto it = _classes.begin(); it != _classes.end(); it++) {
            delete it->second;
        }
        _classes.clear();
        
        uint32_t ht = _pImage + kMonoImageClassCache;
        uint32_t size = ReadUInt32(_task, ht + kMonoInternalHashTableSize);
        uint32_t table = ReadUInt32(_task, ht + kMonoInternalHashTableTable);

        for(uint32_t i = 0; i < size; i++) {
            uint32_t pClass = ReadUInt32(_task, table + 4*i);
            while (pClass != 0) {
                MonoClass* klass = new MonoClass(_task, pClass);
                std::string cfname = klass->getFullName();
				if (cfname != "") {
					_classes[cfname] = klass;
				}
                pClass = ReadUInt32(_task, pClass + kMonoClassNextClassCache);
            }
        }
    
    }
    
    int MonoImage::getMonoImage(int pid, bool isBlocking, HANDLE* handle, MonoImage** monoimage) {
#ifdef __APPLE__
        kern_return_t kret = task_for_pid(mach_task_self(), pid, handle);
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
            proc_address baseaddress = getMonoLoadAddress(*handle);
            if (baseaddress == 0) return 4;
            
            // we need to find the address of "mono_root_domain"
            proc_address mono_grd_addr = getMonoRootDomainAddr(*handle,baseaddress);
            if (mono_grd_addr == 0) return 5;
            
            uint32_t rootDomain;
            
            try {
#ifdef __APPLE__
                rootDomain = ReadUInt32(*handle, baseaddress+mono_grd_addr);
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
                uint32_t next = ReadUInt32(*handle, rootDomain+kMonoDomainDomainAssemblies); // GList*
                
                while(next != 0) {
                    uint32_t data = ReadUInt32(*handle, (proc_address)next);
                    next = ReadUInt32(*handle, (proc_address)next + 4);
                    
                    char* name = ReadCString(*handle, ReadUInt32(*handle, (proc_address)data + kMonoAssemblyName));
                    if(strcmp(name, "Assembly-CSharp") == 0) {
                        free(name);
                        pImage = ReadUInt32(*handle, (proc_address)data + kMonoAssemblyImage);
                        break;
                    }
                    free(name);
                }
            } catch (std::runtime_error& err) {
                return 8;
            }
            
            // we have a pointer now to the right assembly image
            try {
                *monoimage = new MonoImage(*handle, pImage); // apply life cycle
                if ((*monoimage)->hasClasses()) break;
                
                delete *monoimage;
                *monoimage = NULL;
            } catch (std::runtime_error& err) {
                delete *monoimage;
                *monoimage = NULL;
            }
        } while (isBlocking);
        
        return *monoimage == NULL ? 10 : 0;
    }

} // namespace HSReader
