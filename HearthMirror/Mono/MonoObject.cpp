//
//  MonoObject.cpp
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "MonoObject.hpp"
#include "MonoClass.hpp"

namespace hearthmirror {
    
    MonoObject::MonoObject() {}

    MonoObject::MonoObject(mach_port_t task, uint32_t pObject) : _task(task), pObject(pObject) {
        _vtable = ReadUInt32(_task, pObject);
    }

    MonoObject::~MonoObject() {}
    
    MonoClass* MonoObject::getClass() {
        return new MonoClass(_task, ReadUInt32(_task,_vtable));
    }
    
    std::map<std::string, MonoValue> MonoObject::getFields() {
        MonoClass* c = getClass();
        std::vector<MonoClassField*> fields = c->getFields();
        delete c;
        
        std::map<std::string, MonoValue> res;
        
        for (MonoClassField* f : fields) {
            MonoType* type = f->getType();
            if (!type->isStatic()) {
                std::string n = f->getName();
                //printf("%s : 0x%x\n",n.c_str(),type->getType());
                res[n] = f->getValue(this);
            }
            delete type;
            delete f;
        }

        return res;
    }
    
    MonoValue MonoObject::operator[](const std::string& key) {
        std::map<std::string, MonoValue> fields =  getFields();
        MonoValue res;
        res.arrsize = 0;
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

