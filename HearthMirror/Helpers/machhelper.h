//
//  machhelper.h
//  MonoReader
//
//  Created by Istvan Fehervari on 22/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef machhelper_h
#define machhelper_h

#include <mach-o/dyld_images.h>

typedef mach_vm_address_t vm_address;

bool ReadBytes(task_t task, uint8_t* buf, uint32_t size, mach_vm_address_t address);

uint64_t ReadUInt64(mach_port_t task, mach_vm_address_t address);
int64_t ReadInt64(mach_port_t task, mach_vm_address_t address);
uint32_t ReadUInt32(mach_port_t task, mach_vm_address_t address);
int32_t ReadInt32(mach_port_t task, mach_vm_address_t address);
char* ReadCString(task_t task, mach_vm_address_t address);
bool ReadBool(task_t task, mach_vm_address_t address);
uint8_t ReadByte(task_t task, mach_vm_address_t address);
int8_t ReadSByte(task_t task, mach_vm_address_t address);
int16_t ReadShort(task_t task, mach_vm_address_t address);
uint16_t ReadUShort(task_t task, mach_vm_address_t address);

float ReadFloat(task_t task, mach_vm_address_t address);
double ReadDouble(task_t task, mach_vm_address_t address);

vm_address getLibLoadAddress(mach_port_t task, const char* libname);
vm_address getMonoLoadAddress(mach_port_t task);
vm_address getMonoRootDomainAddr(mach_port_t task, vm_address baseAddress);

#endif /* machhelper_h */
