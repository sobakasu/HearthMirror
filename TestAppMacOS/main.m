//
//  main.m
//  TestAppMacOS
//
//  Created by Istvan Fehervari on 26/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "HearthMirror.h"
#import "security.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        
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
        
        HearthMirror* mirror = [[HearthMirror alloc] initWithPID:[hsapp processIdentifier]];
        
        // Get BattleTag
        NSString* battletag = [mirror getBattleTag];
        NSLog(@"BattleTag: %@",battletag);
        
        // Get card collection
        NSArray* collection = [mirror getCardCollection];
        
    }
    return 0;
}
