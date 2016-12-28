//
//  MonoReader.m
//  MonoReader
//
//  Created by Istvan Fehervari on 04/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#import "MonoReader.h"
#import <AppKit/AppKit.h>

#import "Mirror.hpp"

@interface NSString (cppstring_additions)
+(NSString*) stringWithu16string:(const std::u16string&)string;
@end

@implementation NSString (cppstring_additions)

+(NSString*) stringWithu16string:(const std::u16string&)ws
{
    char* data = (char*)ws.data();
    
    NSString* result = [[NSString alloc] initWithBytes:data length:ws.size() encoding:NSUTF16LittleEndianStringEncoding];
    return result;
}

@end

@implementation MonoReader

using namespace HearthMirror;

Mirror* _mirror = NULL;

-(int) initTask {

    if (acquireTaskportRight()) {
        NSLog(@"Error: Access refused");
        return false;
    }
    
    // get Hearthstone app
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
        return 1;
    }
    
    // get rights to attach
    if (acquireTaskportRight() != 0)
    {
        printf("acquireTaskportRight() failed!\n");
        return 2;
    }
    
    _mirror = new Mirror([hsapp processIdentifier]);

    return 0;
}

-(NSString*) getBattleTag {
    
    if (_mirror) {
        BattleTag tag = _mirror->getBattleTag();
        NSString* battlename = [NSString stringWithu16string:tag.name];
        return [NSString stringWithFormat:@"%@#%d",battlename,tag.number];
    }
    
    return 0;
}

-(void)dealloc {
    delete _mirror;
}

@end










