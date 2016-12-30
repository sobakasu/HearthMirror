//
//  MonoObject.hpp
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef MonoObject_hpp
#define MonoObject_hpp

#include <map>
#include <string>

#include "../memhelper.h"
#include "MonoType.hpp"

namespace hearthmirror {
    
    class MonoClass;
    
    class MonoObject {
    public:
        MonoObject();
        MonoObject(HANDLE task, uint32_t pObject);
        ~MonoObject();
        
        uint32_t pObject;
        
        MonoClass* getClass();
        std::map<std::string, MonoValue> getFields();
        
        MonoValue operator[](const std::string& key);
        
    private:
		HANDLE _task;
        uint32_t _vtable;
        
    };
    
} // end namespace


#endif /* MonoObject_hpp */
