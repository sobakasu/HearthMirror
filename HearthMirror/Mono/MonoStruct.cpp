//
//  MonoStruct.cpp
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "MonoStruct.hpp"
#include "MonoClass.hpp"
#include "MonoObject.hpp"

namespace hearthmirror {
    
    MonoStruct::MonoStruct() {}
    
    MonoStruct::MonoStruct(HANDLE task, MonoClass* mClass, uint32_t pStruct) : _task(task), pStruct(pStruct) {
        monoClass = mClass;
    }
    
    MonoStruct::~MonoStruct() {
        delete monoClass;
    }
    
    std::map<std::string, MonoValue> MonoStruct::getFields() {
        
        std::vector<MonoClassField*> fields = monoClass->getFields();
        
        std::map<std::string, MonoValue> res;
        
        for (MonoClassField* f : fields) {
            MonoType* type = f->getType();
            if (!type->isStatic()) {
                MonoObject* o = new MonoObject(_task, pStruct - 8);
                res[f->getName()] = f->getValue(o);
                delete o;
            }
            delete type;
            delete f;
        }
        
        return res;
    }
    
    MonoValue MonoStruct::operator[](const std::string& key) {
        std::map<std::string, MonoValue> fields =  getFields();
        MonoValue res(0);
        for (auto it = fields.begin(); it != fields.end(); it++) {
            if (it->first == key) {
                res = it->second;
            } else {
                DeleteMonoValue(it->second);
            }
        }
        return res;
    }
}
