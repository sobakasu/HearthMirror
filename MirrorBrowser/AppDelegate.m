//
//  AppDelegate.m
//  MirrorBrowser
//
//  Created by Istvan Fehervari on 14/06/2017.
//  Copyright © 2017 com.ifehervari. All rights reserved.
//

#import "AppDelegate.h"
#import "security.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    // get rights to attach
    if (acquireTaskportRight() != 0) {
        printf("acquireTaskportRight() failed!\n");
        
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        [alert setMessageText:@"Error"];
        [alert setInformativeText:@"Failed to acquire taskport rights!"];
        [alert setAlertStyle:NSAlertStyleCritical];
        [alert runModal];
        
        [NSApp terminate:self];
        
        return;
    }
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

// Terminate application when the last window is closed
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}

@end
