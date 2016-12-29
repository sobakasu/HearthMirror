/*
 *  HearthMirror.h
 *  HearthMirror wrapper class
 *
 *  Created by Istvan Fehervari on 26/12/2016.
 *  Copyright © 2016 com.ifehervari. All rights reserved.
 *
 */

#import <Foundation/Foundation.h>

@interface MirrorGameServerInfo: NSObject
    @property NSString *address;
    @property NSString *auroraPassword;
    @property NSNumber *clientHandle;
    @property NSNumber *gameHandle;
    @property NSNumber *mission;
    @property NSNumber *port;
    @property BOOL resumable;
    @property BOOL spectatorMode;
    @property NSString *spectatorPassword;
    @property NSString *version;
@end

@interface MirrorPlayer: NSObject
    @property NSString *name;
    @property NSNumber *playerId;
    @property NSNumber *standardRank;
    @property NSNumber *standardLegendRank;
    @property NSNumber *standardStars;
    @property NSNumber *wildRank;
    @property NSNumber *wildLegendRank;
    @property NSNumber *wildStars;
    @property NSNumber *cardBackId;
@end

@interface MirrorMatchInfo: NSObject
    @property MirrorPlayer *localPlayer;
    @property MirrorPlayer *opposingPlayer;
    @property NSNumber *brawlSeasonId;
    @property NSNumber *missionId;
    @property NSNumber *rankedSeasonId;
@end

@interface HearthMirror : NSObject

-(id) initWithPID:(pid_t)pid;

/** Returns the BattleTag as a single string in the following format: name#number. */
-(NSString*) getBattleTag;

-(NSArray*) getCardCollection;

-(MirrorGameServerInfo*) getGameServerInfo;

-(NSNumber *) getGameType;

-(MirrorMatchInfo *) getMatchInfo;

-(NSNumber *) getFormat;

@end
