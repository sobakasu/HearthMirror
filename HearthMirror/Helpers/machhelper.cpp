//
//  machhelper.cpp
//  MacOS memory reading functions
//
//  Created by Istvan Fehervari on 22/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "machhelper.h"
#include <mach/mach_vm.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <MacTypes.h>
#include <assert.h>

struct dyld_image_info_32 {
    uint32_t imageLoadAddress;
    uint32_t imageFilePath;
    uint32_t imageFileModDate;
};

struct load_command_with_segname {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t segname;
};

struct dyld_image_info_64 {
    uint64_t imageLoadAddress;
    uint64_t imageFilePath;
    uint64_t imageFileModDate;
};

struct dyld_all_image_infos_32 {
    uint32_t version;
    uint32_t infoArrayCount;
    uint32_t infoArray;
    uint32_t notification;
    bool processDetachedFromSharedRegion;
    bool libSystemInitialized;
    uint32_t dyldImageLoadAddress;
    uint32_t jitInfo;
    uint32_t dyldVersion;
    uint32_t errorMessage;
    uint32_t terminationFlags;
    uint32_t coreSymbolicationShmPage;
    uint32_t systemOrderFlag;
    uint32_t uuidArrayCount;
    uint32_t uuidArray;
    uint32_t dyldAllImageInfosAddress;
    uint32_t initialImageCount;
    uint32_t errorKind;
    uint32_t errorClientOfDylibPath;
    uint32_t errorTargetDylibPath;
    uint32_t errorSymbol;
    uint32_t sharedCacheSlide;
    uint8_t	 sharedCacheUUID[16];
    uint32_t reserved[16];
};

struct dyld_all_image_infos_64 {
    uint32_t version;
    uint32_t infoArrayCount;
    uint64_t infoArray;
    uint64_t notification;
    int32_t processDetachedFromSharedRegion;
    int32_t libSystemInitialized;
    uint64_t dyldImageLoadAddress;
    uint64_t jitInfo;
    uint64_t dyldVersion;
    uint64_t errorMessage;
    uint64_t terminationFlags;
    uint64_t coreSymbolicationShmPage;
    uint64_t systemOrderFlag;
    uint64_t uuidArrayCount;
    uint64_t uuidArray;
    uint64_t dyldAllImageInfosAddress;
    uint64_t initialImageCount;
    uint64_t errorKind;
    uint64_t errorClientOfDylibPath;
    uint64_t errorTargetDylibPath;
    uint64_t errorSymbol;
    uint64_t sharedCacheSlide;
    uint8_t	 sharedCacheUUID[16];
    uint64_t reserved[16];
};

#if __LP64__
//#define mem_address mach_vm_address_t
#else
//#define mem_address mach_vm_address_t
#endif

/** @see _copyin_string() */
#define kRemoteStringBufferSize 2048

uint32_t findLibBaseAddress32(mach_port_t task, const char* libname, task_dyld_info_data_t dyld_info) {
    
    vm_address_t address = (vm_address_t)dyld_info.all_image_info_addr;
    mach_msg_type_number_t size = sizeof(struct dyld_all_image_infos);
    vm_offset_t readMem;
    vm_read(task,address,size,&readMem,&size);
    
    struct dyld_all_image_infos_32* infos = (struct dyld_all_image_infos_32 *) readMem;
    size = sizeof(struct dyld_image_info_32) * infos->infoArrayCount;
    vm_read(task,(vm_address_t) infos->infoArray,size,&readMem,&size);
    
    struct dyld_image_info_32* info = (struct dyld_image_info_32*) readMem;
    mach_msg_type_number_t sizeMax=512;
    
    for (int i=0; i < infos->infoArrayCount; i++) {
        vm_read(task,(vm_address_t) info[i].imageFilePath,sizeMax,&readMem,&sizeMax);
        char *path = (char *) readMem;
        if (strstr(path, libname) != NULL) {
            return info[i].imageLoadAddress;
        }
    }
    return NULL;
}

uint64_t findLibBaseAddress64(mach_port_t task, const char* libname, task_dyld_info_data_t dyld_info) {
    
    vm_address_t address = (vm_address_t)dyld_info.all_image_info_addr;
    mach_msg_type_number_t size = sizeof(struct dyld_all_image_infos);
    vm_offset_t readMem;
    vm_read(task,address,size,&readMem,&size);
    
    struct dyld_all_image_infos_64* infos = (struct dyld_all_image_infos_64 *) readMem;
    size = sizeof(struct dyld_image_info_64) * infos->infoArrayCount;
    vm_read(task,(vm_address_t) infos->infoArray,size,&readMem,&size);
    
    struct dyld_image_info_64* info = (struct dyld_image_info_64*) readMem;
    mach_msg_type_number_t sizeMax=512;
    
    for (int i=0; i < infos->infoArrayCount; i++) {
        vm_read(task,(vm_address_t) info[i].imageFilePath,sizeMax,&readMem,&sizeMax);
        char *path = (char *) readMem;
        if (strstr(path, libname) != NULL) {
            return info[i].imageLoadAddress;
        }
    }
    return NULL;
}

proc_address getLibLoadAddress(mach_port_t task, const char* libname) {

    task_dyld_info_data_t dyld_info;
    mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
    if (task_info(task, TASK_DYLD_INFO, (task_info_t)&dyld_info, &count) == KERN_SUCCESS)
    {
        if (dyld_info.all_image_info_format == TASK_DYLD_ALL_IMAGE_INFO_32) {
            return findLibBaseAddress32(task, libname, dyld_info);
        } else {
            return findLibBaseAddress64(task, libname, dyld_info);
        }
    }
    
    return NULL;
}

proc_address getMonoLoadAddress(HANDLE task) {
    return getLibLoadAddress(task, "libmono.0.dylib");
}

proc_address getMonoRootDomainAddr(HANDLE task, proc_address baseAddress) {
    // lookup "mono_root_domain"
    const char *symbol_name = "mono_root_domain";
    
    int err = KERN_FAILURE;
    
    if (baseAddress == 0) {
        return 0;
    }
    
    mach_vm_size_t size = sizeof(struct mach_header);
    struct mach_header header = {0};
    err = mach_vm_read_overwrite(task, baseAddress, size, (mach_vm_address_t)&header, &size);
    if (err != KERN_SUCCESS) return 0;
    
    bool sixtyfourbit = (header.magic == MH_MAGIC_64);
    //*imageFromSharedCache = ((header.flags & kImageFromSharedCacheFlag) == kImageFromSharedCacheFlag);
    
    // We don't support anything but i386 and x86_64
    if (header.magic != MH_MAGIC && header.magic != MH_MAGIC_64) {
        return 0;
    }
    
    mach_vm_address_t symtab_addr = 0;
    mach_vm_address_t linkedit_addr = 0;
    mach_vm_address_t text_addr = 0;
    mach_vm_address_t data_addr = 0;
    
    size_t mach_header_size = sizeof(struct mach_header);
    if (sixtyfourbit) {
        mach_header_size = sizeof(struct mach_header_64);
    }
    mach_vm_address_t command_addr = baseAddress + mach_header_size;
    struct load_command command = {0};
    size = sizeof(command);
    
    for (uint32_t i = 0; i < header.ncmds; i++) {
        err = mach_vm_read_overwrite(task, command_addr, size, (mach_vm_address_t)&command, &size);
        if (err != KERN_SUCCESS) return 0;
        
        if (command.cmd == LC_SYMTAB) {
            symtab_addr = command_addr;
        } else if (command.cmd == LC_DYSYMTAB) {
        } else if (command.cmd == LC_SEGMENT || command.cmd == LC_SEGMENT_64) {
            /* struct load_command only has two fields (cmd & cmdsize), while its "child" type
             * struct segment_command has way more fields including `segname` at index 3, so we just
             * pretend that we have a real segment_command and skip first two fields away */
            size_t segname_field_offset = sizeof(command);
            mach_vm_address_t segname_addr = command_addr + segname_field_offset;
            char *segname = ReadCString(task, segname_addr);//_copyin_string(task, segname_addr);
            if (0 == strcmp(SEG_TEXT, segname)) {
                text_addr = command_addr;
            } else if (0 == strcmp(SEG_DATA, segname)) {
                data_addr = command_addr;
            } else if (0 == strcmp(SEG_LINKEDIT, segname)) {
                linkedit_addr = command_addr;
            }
            free(segname);
        }
        // go to next load command
        command_addr += command.cmdsize;
    }
    
    if (!symtab_addr || !linkedit_addr || !text_addr) {
        return 0;
    }
    
    struct symtab_command symtab = {0};
    size = sizeof(struct symtab_command);
    err = mach_vm_read_overwrite(task, symtab_addr, size, (mach_vm_address_t)&symtab, &size);
    if (err != KERN_SUCCESS) return 0;
    
    if (sixtyfourbit) {
        struct segment_command_64 linkedit = {0};
        size = sizeof(struct segment_command_64);
        err = mach_vm_read_overwrite(task, linkedit_addr, size,
                                     (mach_vm_address_t)&linkedit, &size);
        if (err != KERN_SUCCESS) return 0;
        struct segment_command_64 text = {0};
        err = mach_vm_read_overwrite(task, text_addr, size, (mach_vm_address_t)&text, &size);
        if (err != KERN_SUCCESS) return 0;
        
        uint64_t file_slide = linkedit.vmaddr - text.vmaddr - linkedit.fileoff;
        uint64_t strings = baseAddress + symtab.stroff + file_slide;
        uint64_t sym_addr = baseAddress + symtab.symoff + file_slide;
        
        for (uint32_t i = 0; i < symtab.nsyms; i++) {
            struct nlist_64 sym = {{0}};
            size = sizeof(struct nlist_64);
            err = mach_vm_read_overwrite(task, sym_addr, size, (mach_vm_address_t)&sym, &size);
            if (err != KERN_SUCCESS) return 0;
            sym_addr += size;
            
            if (!sym.n_value) continue;
            
            uint64_t symname_addr = strings + sym.n_un.n_strx;
            char *symname = ReadCString(task, symname_addr);
            // Ignore the leading "_" character in a symbol name
            if (0 == strcmp(symbol_name, symname+1)) {
                free(symname);
                return (mach_vm_address_t)sym.n_value;
            }
            free(symname);
        }
    } else {
        struct segment_command linkedit = {0};
        size = sizeof(struct segment_command);
        err = mach_vm_read_overwrite(task, linkedit_addr, size,
                                     (mach_vm_address_t)&linkedit, &size);
        if (err != KERN_SUCCESS) return 0;
        struct segment_command text = {0};
        err = mach_vm_read_overwrite(task, text_addr, size, (mach_vm_address_t)&text, &size);
        if (err != KERN_SUCCESS) return 0;
        
        if (data_addr != 0) {
            struct segment_command data = {0};
            err = mach_vm_read_overwrite(task, data_addr, size, (mach_vm_address_t)&data, &size);
            if (err != KERN_SUCCESS) return 0;
        }
        uint32_t file_slide = linkedit.vmaddr - text.vmaddr - linkedit.fileoff;
        uint32_t strings = (uint32_t)baseAddress + symtab.stroff + file_slide;
        uint32_t sym_addr = (uint32_t)baseAddress + symtab.symoff + file_slide;
        
        for (uint32_t i = 0; i < symtab.nsyms; i++) {
            struct nlist sym = {{0}};
            size = sizeof(struct nlist);
            err = mach_vm_read_overwrite(task, sym_addr, size, (mach_vm_address_t)&sym, &size);
            if (err != KERN_SUCCESS) return 0;
            sym_addr += size;
            
            if (!sym.n_value) continue;
            
            uint32_t symname_addr = strings + sym.n_un.n_strx;
            char *symname = ReadCString(task, symname_addr);
            /* Ignore the leading "_" character in a symbol name */
            if (0 == strcmp(symbol_name, symname+1)) {                
                free(symname);
                return (mach_vm_address_t)sym.n_value;
            }
            free(symname);
        }
    }
    
    return 0;
}

float ToFloat(Byte* buffer, int start=0) {
    float f;
    memcpy(&f, &buffer, sizeof(f));
    return f;
}

double ToDouble(Byte* buffer, int start=0) {
    double f;
    memcpy(&f, &buffer, sizeof(f));
    return f;
}

uint64_t ReadUInt64(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 8;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    assert(err == KERN_SUCCESS);
    
    uint64_t v = 0;
    memcpy((char *)&v, (Byte*)readMem, sizeof(uint64_t));
    return v;
}

int64_t ReadInt64(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 8;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    int64_t v = 0;
    memcpy((char *)&v, (Byte*)readMem, sizeof(int64_t));
    return v;
}

uint32_t ReadUInt32(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 4;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    uint32_t v = 0;
    memcpy((char *)&v, (Byte*)readMem, sizeof(uint32_t));
    return v;
}

int32_t ReadInt32(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 4;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    int32_t v = 0;
    memcpy((char *)&v, (Byte*)readMem, sizeof(int32_t));
    return v;
}

bool ReadBool(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 1;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    Byte* buffer = (Byte*)readMem;
    return (bool)buffer[0];
}

uint8_t ReadByte(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 1;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    Byte* buffer = (Byte*)readMem;
    return (Byte)buffer[0];
}

int8_t ReadSByte(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 1;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    SignedByte* buffer = (SignedByte*)readMem;
    return (SignedByte)buffer[0];
}

int16_t ReadShort(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 2;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    int16_t v = 0;
    memcpy((char *)&v, (Byte*)readMem, sizeof(int16_t));
    return v;
}

uint16_t ReadUShort(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 2;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    uint16_t v = 0;
    memcpy((char *)&v, (Byte*)readMem, sizeof(uint16_t));
    return v;
}

float ReadFloat(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 4;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    Byte* buffer = (Byte*)readMem;
    return ToFloat((Byte*)buffer);
}

double ReadDouble(HANDLE task, mach_vm_address_t address) {
    vm_offset_t readMem;
    vm_size_t size = 8;
    mach_msg_type_number_t data_read;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&data_read);
    if (err != KERN_SUCCESS) return 0;
    
    Byte* buffer = (Byte*)readMem;
    return ToDouble((Byte*)buffer);
}

bool ReadBytes(HANDLE task, proc_address buf, uint32_t size, mach_vm_address_t address) {
    vm_offset_t readMem;
    kern_return_t err = mach_vm_read(task,address,size,&readMem,&size);
    if (err != KERN_SUCCESS) return 0;
    
    memcpy((void*)buf, (void*)readMem, size);
    
    return true;
}

char *ReadCString(HANDLE task, mach_vm_address_t pointer)
{
    assert(pointer > 0);
    int err = KERN_FAILURE;
    
    char buf[kRemoteStringBufferSize] = {0}; // too long symbol names might not fit in
    mach_vm_size_t size = sizeof(buf);
    err = mach_vm_read_overwrite(task, pointer, size,
                                 (mach_vm_address_t)&buf, &size);
    if (err != KERN_SUCCESS) return 0;
    
    // add ending
    buf[kRemoteStringBufferSize-1] = '\0';
    
    char *result = strdup(buf);
    return result;
}
