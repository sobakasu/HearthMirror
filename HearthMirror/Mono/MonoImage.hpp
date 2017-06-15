//
//  MonoImage.hpp
//  MonoReader
//
//  Created by Istvan Fehervari on 22/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef MonoImage_hpp
#define MonoImage_hpp

#include <map>
#include <string>

#include "../memhelper.h"
#include "MonoClass.hpp"

namespace hearthmirror {
    class MonoImage {
    public:
        MonoImage(HANDLE task, uint32_t pImage);
        ~MonoImage();
        
        MonoClass* get(const std::string& key);
        const std::map<std::string,MonoClass*> getClasses() {return _classes;};
        bool hasClasses();
        
        static int getMonoImage(int pid, bool isBlocking, HANDLE* handle, MonoImage** monoimage);
        
    private:
		HANDLE _task;
        uint32_t _pImage;
        std::map<std::string,MonoClass*> _classes;
        void LoadClasses();
    };
    
    
}


#endif /* MonoImage_hpp */
