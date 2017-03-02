//
//  MonoClass.cpp
//  MonoReader
//
//  Created by Istvan Fehervari on 22/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "MonoClass.hpp"
#include "../Helpers/offsets.h"

namespace hearthmirror {
    
    MonoClass::MonoClass() {}

    MonoClass::MonoClass(HANDLE task, uint32_t pClass) : _task(task), _pClass(pClass) {}

    MonoClass::~MonoClass() {}
    
    MonoClass::MonoClass(const MonoClass* other) {
        _task = other->_task;
        _pClass = other->_pClass;
    }
    
    std::string MonoClass::getName() {
        uint32_t addr = ReadUInt32(_task, _pClass + kMonoClassName);
        if (addr == 0) {
            std::string result;
            return result;
        }
        char* pName = ReadCString(_task, addr);
        if (pName != NULL) {
            std::string name(pName);
            free(pName);
            return name;
        }
        
        throw std::runtime_error("Could not read MonoClass namespace");
    }
    
    std::string MonoClass::getNameSpace() {
        uint32_t addr = ReadUInt32(_task, _pClass + kMonoClassNameSpace);
        if (addr == 0) return "";
        char* pNamespace = ReadCString(_task, addr);
        if (pNamespace != NULL) {
            std::string ns(pNamespace);
            free(pNamespace);
            return ns;
        }

        throw std::runtime_error("Could not read MonoClass namespace");
    }
    
    std::string MonoClass::getFullName() {
        std::string name = getName();
        std::string ns = getNameSpace();
        
        MonoClass* nestedIn = getNestedIn();
        
        while(nestedIn != NULL)
        {
            name = nestedIn->getName() + "+" + name;
            ns = nestedIn->getNameSpace();
            
            MonoClass* nestedIn_t = nestedIn->getNestedIn();
            delete nestedIn;
            nestedIn = nestedIn_t;
        }
        return ns.size() == 0 ? name : ns + "." + name;
    }
    
    MonoClass* MonoClass::getNestedIn() {
        uint32_t pNestedIn = ReadUInt32(_task, _pClass + kMonoClassNestedIn);
        return pNestedIn == 0 ? NULL : new MonoClass(_task, pNestedIn);
    }
    
    uint32_t MonoClass::getVTable() {
        uint32_t rti = ReadUInt32(_task, _pClass + kMonoClassRuntimeInfo);
        return ReadUInt32(_task, rti + kMonoClassRuntimeInfoDomainVtables);
    }
    
    bool MonoClass::isValueType() {
        return 0 != (ReadByte(_task, _pClass + kMonoClassBitfields) & 8);
    }
    
    bool MonoClass::isEnum() {
        return 0 != (ReadUInt32(_task, _pClass + kMonoClassBitfields) & 0x10);
    }
    
    uint32_t MonoClass::size() {
        return ReadUInt32(_task, _pClass + kMonoClassSizes);
    }
    
    MonoClass* MonoClass::getParent() {
            uint32_t pParent = ReadUInt32(_task, _pClass + kMonoClassParent);
            return pParent == 0 ? NULL : new MonoClass(_task, pParent);
    }
    
    MonoType* MonoClass::byValArg() {
        return new MonoType(_task, _pClass + kMonoClassByvalArg);
    }
    
    uint32_t MonoClass::getNumFields() {
        return ReadUInt32(_task, _pClass + kMonoClassFieldCount);
    }

    std::vector<MonoClassField*> MonoClass::getFields() {

        uint32_t nFields = getNumFields();
        
        uint32_t nFieldsParent = 0;
        MonoClass* parent = getParent();
        if (parent) {
            nFieldsParent = parent->getNumFields();
        }
        
        uint32_t pFields = ReadUInt32(_task, _pClass + kMonoClassFields);
        std::vector<MonoClassField*> fs(nFields + nFieldsParent);
        
        for(uint32_t i = 0; i < nFields; i++) {
            fs[i] = new MonoClassField(_task, pFields + (uint32_t) i*kMonoClassFieldSizeof);
        }
        if (parent) {
            std::vector<MonoClassField*> pfs = parent->getFields();
            
            for(uint32_t i = 0; i < nFieldsParent; i++) {
                fs[nFields + i] = pfs[i];
            }
            // apparently returned fields from the parent might be more than the field reporting it
            for (int i = nFieldsParent; i< pfs.size(); i++) {
                    delete pfs[i];
            }
            delete parent;
        }
        return fs;
    }
    
    MonoValue MonoClass::operator[](const std::string& key) {
        
        std::vector<MonoClassField*> fields = getFields();
        MonoValue ret;
        ret.arrsize = 0;
        for (MonoClassField* mcf : fields) {
            if (mcf->getName() == key) {
                try {
                    ret = mcf->getStaticValue();
                } catch (std::exception ex) {
                    // could not read
                    //printf("failed to read");
                }
            }
            delete mcf;
        }
        
        return ret;
    }
    
    
} // namespace hearthmirror
