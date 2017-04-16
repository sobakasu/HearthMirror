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

} // namespace HSReader
