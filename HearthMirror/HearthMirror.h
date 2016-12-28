/*
 *  HearthMirror.h
 *  HearthMirror wrapper class
 *
 *  Created by Istvan Fehervari on 26/12/2016.
 *  Copyright © 2016 com.ifehervari. All rights reserved.
 *
 */

#import <Foundation/Foundation.h>

@interface HearthMirror : NSObject

-(id) initWithPID:(pid_t)pid;

/** Returns the BattleTag as a single string in the following format: name#number. */
-(NSString*) getBattleTag;

-(NSArray*) getCardCollection;

@end
