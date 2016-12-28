//
//  MonoClassField.hpp
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef MonoClassField_hpp
#define MonoClassField_hpp

#include <string>
#include "memhelper.h"
#include "MonoType.hpp"

namespace hearthmirror {
    
    class MonoClass;
    class MonoObject;
    
    class MonoClassField {
    public:
        MonoClassField();
        MonoClassField(mach_port_t task, uint32_t pField);
        ~MonoClassField();
        
        std::string getName();
        int32_t getOffset();
        
        MonoType* getType();
        MonoClass* getParent();
        MonoValue getStaticValue();
        MonoValue getValue(MonoObject* o);
        
    private:
        mach_port_t _task;
        uint32_t _pField;
        
        MonoValue ReadValue(MonoTypeEnum type, mach_vm_address_t addr);
    };
    
} // end namespace


#endif /* MonoClassField_hpp */
