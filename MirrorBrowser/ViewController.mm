//
//  ViewController.m
//  MirrorBrowser
//
//  Created by Istvan Fehervari on 14/06/2017.
//  Copyright © 2017 com.ifehervari. All rights reserved.
//

#import "ViewController.h"
#import "Mirror.hpp"
#include "memhelper.h"
#include "offsets.h"

#include "MonoImage.hpp"
#include "MonoObject.hpp"
#include "MonoStruct.hpp"

#pragma mark - NSString extension
@interface NSString (cppstring_additions)
+(NSString*) stringWithu16string:(const std::u16string&)string;
@end

@implementation NSString (cppstring_additions)

+(NSString*) stringWithu16string:(const std::u16string&)ws
{
    if (ws.size() == 0) return nil;
    
    char* data = (char*)ws.data();
    
    NSString* result = [[NSString alloc] initWithBytes:data length:ws.size()*2 encoding:NSUTF16LittleEndianStringEncoding];
    return result;
}

@end

using namespace hearthmirror;

@implementation TreeElement

-(instancetype) init {
    
    self = [super init];
    if (self)
    {
        self.name = @"";
        self.type = @"";
        self.value = @"";
        self.children = [NSMutableArray new];
    }
    return self;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.treeElements = [NSMutableArray new];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView
  numberOfChildrenOfItem:(id)item {
    if (item) {
        TreeElement* element = (TreeElement*)item;
        return [element.children count];
    }
    
    return [self.treeElements count];
}

// Returns a Boolean value that indicates whether the a given item is expandable.
- (BOOL)outlineView:(NSOutlineView *)outlineView
   isItemExpandable:(id)item {
    
    if (item) {
        TreeElement* element = (TreeElement*)item;
        return [element.children count] > 0;
    }
    
    return NO;
}

- (id)outlineView:(NSOutlineView *)outlineView
            child:(NSInteger)index
           ofItem:(id)item {
    
    if (item) {
        TreeElement* element = (TreeElement*)item;
        return [element.children objectAtIndex:index];
    }
    
    return [self.treeElements objectAtIndex:index];
}

// Invoked by outlineView to return the data object associated with the specified item.
/*- (id)outlineView:(NSOutlineView *)outlineView
objectValueForTableColumn:(NSTableColumn *)tableColumn
           byItem:(id)item {
    return nil;
}*/

- (NSView *)outlineView:(NSOutlineView *)outlineView
     viewForTableColumn:(NSTableColumn *)tableColumn
                   item:(id)item {
    
    TreeElement* element = (TreeElement*)item;
    NSTableCellView* cellview;
    
    if ([tableColumn.identifier isEqualToString:@"ObjectColumn"]) {
        cellview = [self.tableView makeViewWithIdentifier:@"ObjectCell" owner:self];
        
        cellview.textField.stringValue = element.name;
        //[cellview.textField sizeToFit];
    } else if ([tableColumn.identifier isEqualToString:@"TypeColumn"]) {
        cellview = [self.tableView makeViewWithIdentifier:@"TypeCell" owner:self];
        
        cellview.textField.stringValue = element.type;
        //[cellview.textField sizeToFit];
    } else if ([tableColumn.identifier isEqualToString:@"ValueColumn"]) {
        cellview = [self.tableView makeViewWithIdentifier:@"ValueCell" owner:self];
        
        cellview.textField.stringValue = element.value;
        //[cellview.textField sizeToFit];
    }
    
    
    
    return cellview;
}

NSString* MonoTypeEnumToString(MonoTypeEnum type) {
    switch (type) {
        case End: return @"End";
        case Void: return @"Void";
        case hearthmirror::Boolean: return @"Boolean";
        case Char: return @"Char";
        case I1: return @"I1";
        case U1: return @"U1";
        case I2: return @"I2";
        case U2: return @"U2";
        case I4: return @"I4";
        case U4: return @"U4";
        case I8: return @"I8";
        case U8: return @"U8";
        case R4: return @"R4";
        case R8: return @"R8";
        case String: return @"String";
        case hearthmirror::Ptr: return @"Ptr";
        case ByRef: return @"ByRef";
        case ValueType: return @"ValueType";
        case hearthmirror::Class: return @"Class";
        case Var: return @"Var";
        case Array: return @"Array";
        case GenericInst: return @"GenericInst";
        case TypedByRef: return @"TypedByRef";
        case I: return @"I";
        case U: return @"U";
        case FnPtr: return @"FnPtr";
        case Object: return @"Object";
        case Szarray: return @"Szarray";
        case Mvar: return @"Mvar";
        case Cmod_reqd: return @"Cmod_reqd";
        case Cmod_opt: return @"Cmod_opt";
        case Internal: return @"Internal";
        case Modifier: return @"Modifier";
        case Sentinel: return @"Sentinel";
        case Pinned: return @"Pinned";
        case Enum: return @"Enum";
        default: return @"Unknown";
    }
    return @"Unknown";
}

NSString* MonoTypeToString(MonoType* type) {
    return MonoTypeEnumToString(type->getType());
}

NSString* MonoValueToString(MonoValue value) {
    switch (value.type) {
        case MonoTypeEnum::Boolean: {
            return [NSString stringWithFormat:@"%@",value.value.b ? @"true" : @"false"];
        }
        case MonoTypeEnum::U1: {
            return [NSString stringWithFormat:@"%d",value.value.u8];
        }
        case MonoTypeEnum::I1: {
            return [NSString stringWithFormat:@"%d",value.value.i8];
        }
        case MonoTypeEnum::I2: {
            return [NSString stringWithFormat:@"%d",value.value.i16];
        }
        case MonoTypeEnum::U2: {
            return [NSString stringWithFormat:@"%d",value.value.u16];
        }
        case MonoTypeEnum::Char: {
            return [NSString stringWithFormat:@"%c",value.value.c];
        }
        case MonoTypeEnum::I:
        case MonoTypeEnum::I4: {
            return [NSString stringWithFormat:@"%d",value.value.i32];
        }
        case MonoTypeEnum::U:
        case MonoTypeEnum::U4: {
            return [NSString stringWithFormat:@"%d",value.value.u32];
        }
        case MonoTypeEnum::I8: {
            return [NSString stringWithFormat:@"%lld",value.value.i64];
        }
        case MonoTypeEnum::U8: {
            return [NSString stringWithFormat:@"%lld",value.value.u64];
        }
        case MonoTypeEnum::R4: {
            return [NSString stringWithFormat:@"%f",value.value.f];
        }
        case MonoTypeEnum::R8:{
            return [NSString stringWithFormat:@"%f",value.value.d];
        }
        case MonoTypeEnum::String:{
            return [NSString stringWithu16string:value.str];
        }
        default: return @"";
    }
    return @"";
}

NSMutableArray* getChildren(MonoValue value) {
    NSMutableArray* result = [NSMutableArray new];
    switch (value.type) {
        case hearthmirror::Object:
        case hearthmirror::GenericInst:
        case hearthmirror::Var:
        case hearthmirror::Class: {
            MonoObject* o = value.value.obj.o;
            auto fields = o->getFields();
            for (auto it = fields.begin(); it != fields.end(); it++) {
                TreeElement* child = [TreeElement new];
                auto mv = it->second;
                child.name = [NSString stringWithCString:it->first.c_str()
                                                           encoding:[NSString defaultCStringEncoding]];
                child.type = MonoTypeEnumToString(mv.type);
                child.value = MonoValueToString(mv);
                
                if (mv.type == hearthmirror::Object ||
                    mv.type == hearthmirror::ValueType ||
                    mv.type == hearthmirror::GenericInst ||
                    mv.type == hearthmirror::Var ||
                    mv.type == hearthmirror::Class) {
                    
                    // get children
                    child.children = getChildren(mv);
                }
                
                [result addObject:child];
                DeleteMonoValue(mv);
            }
            
            break;
        }
        case hearthmirror::ValueType: {
            MonoStruct* s = value.value.obj.s;
            break;
        }
        default: break;
    }
    return result;
}

- (IBAction)reload:(id)sender {
    
    NSArray* apps = [[NSWorkspace sharedWorkspace] runningApplications]; // NSRunningApplication
    
    NSRunningApplication* hsapp = NULL;
    for (NSRunningApplication* app in apps) {
        if ([[app bundleIdentifier] isEqualToString:@"unity.Blizzard Entertainment.Hearthstone"]) {
            hsapp = app;
            break;
        }
    }
    
    if (hsapp == NULL) {
        printf("Hearthstone is not running!\n");
        return;
    }
    
    MonoImage* _image;
    HANDLE task;
    try {
        if (MonoImage::getMonoImage([hsapp processIdentifier],false, &task, &_image) != 0) {
            return;
        }
    }  catch (const std::exception &e) {
        NSLog(@"Error while initializing Mirror: %s", e.what());
        return;
    }
    
    [self.treeElements removeAllObjects];
    
    NSMutableArray* queue = [NSMutableArray new];
    
    const std::map<std::string,MonoClass*> classes = _image->getClasses();
    for (auto it = classes.begin(); it != classes.end(); it++) {
        TreeElement* element = [TreeElement new];
        element.name = [NSString stringWithCString:it->first.c_str()
                                          encoding:[NSString defaultCStringEncoding]];
        element.type = @"Class";
        element.value = @"";
        
        MonoClass* c = it->second;
        auto fields = c->getFields();
        for (auto it = fields.begin(); it != fields.end(); it++) {
            auto ctype = (*it)->getType();
            
            
            TreeElement* classfieldElement = [TreeElement new];
            classfieldElement.name = [NSString stringWithCString:(*it)->getName().c_str()
                                                        encoding:[NSString defaultCStringEncoding]];
            
            
            classfieldElement.type = MonoTypeToString(ctype);
            MonoValue mv = (*it)->getStaticValue();
            
            auto cct = ctype->getType();
            if (cct == hearthmirror::Object ||
                cct == hearthmirror::ValueType ||
                cct == hearthmirror::GenericInst ||
                cct == hearthmirror::Var ||
                cct == hearthmirror::Class) {
                
                // get children
                classfieldElement.children = getChildren(mv);
                [element.children addObject:classfieldElement];
            } else if (ctype->isStatic()) {
                classfieldElement.value = MonoValueToString(mv);
                [element.children addObject:classfieldElement];
            }
            DeleteMonoValue(mv);
            
            delete ctype;
            delete *it;
        }
        
        
        [self.treeElements addObject:element];
        [queue addObject:element];
    }

    [self.tableView reloadData];
}

@end
