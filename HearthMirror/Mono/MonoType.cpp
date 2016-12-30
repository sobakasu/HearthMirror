//
//  MonoType.cpp
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "MonoType.hpp"
#include "MonoObject.hpp"
#include "../Helpers/offsets.h"

namespace hearthmirror {
    
    MonoType::MonoType() {}
    
    MonoType::MonoType(HANDLE task, uint32_t pType) : _task(task), _pType(pType) {}
    
    MonoType::~MonoType() {}
    
    uint32_t MonoType::getAttrs() {
        return ReadUShort(_task, _pType + kMonoTypeAttrs);
    }
    
    uint32_t MonoType::getData() {
        return ReadUInt32(_task, _pType);
    }
    
    bool MonoType::isStatic() {
        return 0 != (getAttrs() & 0x10);
    }
    
    bool MonoType::isPublic() {
        return 6 == (getAttrs() & 0x7);
    }
    
    bool MonoType::isLiteral() {
        return 0 != (getAttrs() & 0x40);
    }
    
    bool MonoType::hasDefault() {
        return 0 != (getAttrs() & 0x8000);
    }
    
    bool MonoType::hasFieldRva() {
        return 0 != (getAttrs() & 0x100);
    }
    
    bool MonoType::byRef() {
        return 0 != (getAttrs() & 0x40000000);
    }
    
    MonoTypeEnum MonoType::getType() {
#ifdef __APPLE__
		return (MonoTypeEnum)ReadByte(_task, _pType + kMonoTypeType);
#else
		return (MonoTypeEnum)(0xff & (getAttrs() >> 16));
#endif // __APPLE__

        
    }

    void DeleteMonoValue(MonoValue& mv) {
        if (mv.arrsize == 0) return; // value is null
        switch (mv.type) {
            case Object:
            case Class:
                delete mv.value.obj.o;
                break;
            case ValueType:
                delete mv.value.obj.s;
                break;
            case Szarray:
                for (unsigned int i =0; i< mv.arrsize; i++) {
                    DeleteMonoValue(mv[i]);
                }
                delete[] mv.value.arr;
                break;
            default:
                break;
        }
    }
    
    bool IsMonoValueEmpty(const MonoValue& mv) {
        return (mv.arrsize == 0) || ((mv.type == Class || mv.type == GenericInst || mv.type == Object) && mv.value.obj.o == NULL);
    }
    
    
    bool IsMonoValueArray(const MonoValue& mv) {
        return mv.type == Array || mv.type == Szarray;
    }
}
