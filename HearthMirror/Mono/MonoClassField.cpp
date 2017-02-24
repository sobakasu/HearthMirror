//
//  MonoClassField.cpp
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "MonoClassField.hpp"
#include "../Helpers/offsets.h"
#include "MonoClass.hpp"
#include "MonoObject.hpp"
#include "MonoStruct.hpp"

namespace hearthmirror {
    
    MonoClassField::MonoClassField() {}
    
    MonoClassField::MonoClassField(HANDLE task, uint32_t pField) : _task(task), _pField(pField) {}
    
    MonoClassField::~MonoClassField() {}

    std::string MonoClassField::getName() {
        try {
            char* pName = ReadCString(_task, ReadUInt32(_task, _pField + kMonoClassFieldName));
            if (pName == NULL) return std::string("");
            std::string name(pName);
            free(pName);
            return name;
        } catch (std::exception& exp) {
            return std::string("");
        }
    }
    
    int32_t MonoClassField::getOffset() {
        return ReadInt32(_task, _pField + kMonoClassFieldOffset);
    }
    
    MonoType* MonoClassField::getType() {
        return new MonoType(_task, ReadUInt32(_task, _pField + kMonoClassFieldType));
    }
    
    MonoClass* MonoClassField::getParent() {
        return new MonoClass(_task, ReadUInt32(_task, _pField + kMonoClassFieldParent));
    }
    
    MonoValue MonoClassField::getStaticValue() {
        
        MonoType* type = getType();
        MonoValue ret;
        if (type->isStatic()) {
            ret =  getValue(NULL);
        } else {
            ret.arrsize = 0;
        }
        delete type;
        return ret;
    }
    
    MonoValue MonoClassField::getValue(MonoObject* o) {
        
        int32_t offset = getOffset();
        MonoType* type = getType();
        MonoTypeEnum typeType = type->getType();

        bool isRef;
        // get data type
        switch(typeType)
        {
            case MonoTypeEnum::String:
            case MonoTypeEnum::Szarray:
                isRef = false;
                break;
            case MonoTypeEnum::Object:
            case MonoTypeEnum::Class:
            case MonoTypeEnum::Array:
                isRef = true;
                break;
            case MonoTypeEnum::GenericInst: {
                uint32_t genericClass = type->getData();
                MonoClass* container = new MonoClass(_task, ReadUInt32(_task, genericClass));
                isRef = !container->isValueType();
                delete container;
                break;
            }
            default:
                isRef = type->byRef();
                break;
        }
        
        if(type->isStatic()) {
            
            MonoClass* parent = getParent();
            uint32_t data = ReadUInt32(_task, parent->getVTable() + kMonoVTableData);
            if(isRef) {
                uint32_t po = ReadUInt32(_task, data + offset);
                delete parent;
                delete type;
                if (po == 0) {
                    return MonoValue(0);
                } else {
                    MonoValue mv;
                    mv.type = typeType;
                    mv.value.obj.o=  new MonoObject(_task, po);
                    return mv;
                }
            }
            delete parent;
            
            if(typeType == MonoTypeEnum::ValueType) {
                
                MonoClass* sClass = new MonoClass(_task, type->getData());
                if(sClass->isEnum()) {
                    MonoClass* c = new MonoClass(_task, ReadUInt32(_task,type->getData() ));
                    MonoType* bva = c->byValArg();
                    MonoValue mv = ReadValue(bva->getType(), data + offset);
                    delete bva;
                    delete c;
                    delete type;
                    delete sClass;
                    return mv;
                }
                delete type;
                MonoValue mv;
                mv.type = ValueType;
                MonoClass* c2 = new MonoClass(sClass);
                mv.value.obj.s = new MonoStruct(_task, c2, (uint32_t) (data + offset));
                delete sClass;
                return mv;
            }
            delete type;
            if (typeType == MonoTypeEnum::GenericInst) {
                return MonoValue(0);
            } else {
                return ReadValue(typeType, data + offset);
            }
        }
        
        if(isRef) {
            
            uint32_t po = ReadUInt32(_task, o->pObject + offset);
            delete type;
            if (po == 0) {
                return MonoValue(0);
            } else {
                MonoValue mv;
                mv.type = Object;
                mv.value.obj.o = new MonoObject(_task, po);
                return mv;
            }
        }
        
        if(typeType == MonoTypeEnum::ValueType) {
            
            MonoClass*  sClass = new MonoClass(_task, type->getData());
            if(sClass->isEnum()) {
                MonoClass* c = new MonoClass(_task, ReadUInt32(_task,type->getData() ));
                MonoType* bva = c->byValArg();
                MonoValue res = ReadValue(bva->getType(), o->pObject + offset);
                delete bva;
                delete c;
                delete type;
                delete sClass;
                
                return res;
            }
            delete type;
            MonoValue mv;
            mv.type = MonoTypeEnum::ValueType;
            
            MonoClass* c2 = new MonoClass(sClass);
            mv.value.obj.s = new MonoStruct(_task, c2, (uint32_t) (o->pObject + offset));
            delete sClass;
            return mv;
        }
        
        delete type;
        if (typeType == MonoTypeEnum::GenericInst) {
            MonoValue mv;
            mv.arrsize = 0;
            return mv;
        } else {
            return ReadValue(typeType, o->pObject + offset);
        }
    }
    
    MonoValue MonoClassField::ReadValue(MonoTypeEnum type, proc_address addr) {
        MonoValue result;
        result.type = type;
        switch(type)
        {
            case MonoTypeEnum::Boolean: {
                result.value.b = ReadBool(_task, addr);
                return result;
            }
            case MonoTypeEnum::U1: {
                result.value.u8 = ReadByte(_task, addr);
                return result;
            }
            case MonoTypeEnum::I1: {
                result.value.i8 = ReadSByte(_task, addr);
                return result;
            }
            case MonoTypeEnum::I2: {
                result.value.i16 = ReadShort(_task, addr);
                return result;
            }
            case MonoTypeEnum::U2: {
                result.value.u16 = ReadUShort(_task, addr);
                return result;
            }
            case MonoTypeEnum::Char: {
                result.value.c = (char)ReadUShort(_task, addr);
                return result;
            }
            case MonoTypeEnum::I:
            case MonoTypeEnum::I4: {
                result.value.i32 = ReadInt32(_task, addr);
                return result;
            }
            case MonoTypeEnum::U:
            case MonoTypeEnum::U4: {
                result.value.u32 = ReadUInt32(_task, addr);
                return result;
            }
            case MonoTypeEnum::I8: {
                result.value.i64 = ReadInt64(_task, addr);
                return result;
            }
            case MonoTypeEnum::U8: {
                result.value.u64 = ReadUInt64(_task, addr);
                return result;
            }
            case MonoTypeEnum::R4: {
                result.value.f = ReadFloat(_task, addr);
                return result;
            }
            case MonoTypeEnum::R8:{
                result.value.d = ReadDouble(_task, addr);
                return result;
            }
            case MonoTypeEnum::Szarray: {
                addr = ReadUInt32(_task, addr); // deref object
                uint32_t vt = ReadUInt32(_task, addr);
                uint32_t pArrClass = ReadUInt32(_task, vt);
                MonoClass* arrClass = new MonoClass(_task, pArrClass);
                MonoClass* elClass = new MonoClass(_task, ReadUInt32(_task, pArrClass));
                //bool avt = arrClass->isValueType();
                uint32_t count = ReadInt32(_task, addr + 12);
                uint32_t start = (uint32_t)addr + 16;
                result.arrsize = count;
                if (count > 0) {
                    result.value.arr = new MonoValue[count];
                    for (uint32_t i = 0; i < count; i++) {
                        
                        uint32_t ea = start + i* arrClass->size();
                        if(elClass->isValueType()) {
                            MonoType* mt = elClass->byValArg();
                            if(mt->getType() == MonoTypeEnum::ValueType) {
                                MonoClass* c2 = new MonoClass(elClass);
                                MonoStruct* stc = new MonoStruct(_task, c2, (uint32_t) ea);
                                MonoValue mv;
                                mv.type = MonoTypeEnum::ValueType;
                                mv.value.obj.s = stc;
                                result[i] = mv;
                            } else {
                                result[i] = ReadValue(mt->getType(), ea);
                            }
                            delete mt;
                        } else {
                            uint32_t po = ReadUInt32(_task, ea);
                            MonoValue mv;
                            if (po == 0) {
                                mv.arrsize = 0;
                                result[i] = mv;
                            } else {
                                mv.type = GenericInst;
                                mv.value.obj.o = new MonoObject(_task, po);
                                result[i] = mv;
                            }
                        }
                    }
                }
                
                delete arrClass;
                delete elClass;
                return result;
            }
            case MonoTypeEnum::String: {
                uint32_t pArr = ReadUInt32(_task, addr);
                result.str =  std::u16string();
                if(pArr == 0) {
                    return result;
                }
   
                int32_t strlen = ReadInt32(_task, pArr + 8);
                if(strlen == 0) {
                    return result;
                }
                
                uint32_t size = strlen*2;
                uint8_t* buf = new uint8_t[size];
                
                ReadBytes(_task, (proc_address)buf, size, pArr + 12);
                result.str = std::u16string((char16_t*)buf, strlen);
				delete[]  buf;
                return result;
            }
            default:
                printf("Error: %d not implemented\n",type);
        }
        result.arrsize = 0;
        return result;
    }
}
