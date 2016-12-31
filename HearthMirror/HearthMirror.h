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

@interface MirrorAccountId: NSObject
    @property NSNumber *lo;
    @property NSNumber *hi;
@end

@interface MirrorCard: NSObject
    @property NSString *cardId;
    @property NSNumber *count;
    @property BOOL premium;
@end

@interface MirrorDeck: NSObject
    @property NSNumber *id;
    @property NSString *name;
    @property NSString *hero;
    @property BOOL isWild;
    @property NSNumber *type;
    @property NSNumber *seasonId;
    @property NSNumber *cardBackId;
    @property NSNumber *heroPremium;
    @property NSArray *cards;
@end

@interface HearthMirror : NSObject

-(instancetype) initWithPID:(pid_t)pid;

/** Returns the BattleTag as a single string in the following format: name#number. */
-(nullable NSString*) getBattleTag;

-(nonnull NSArray*) getCardCollection;

-(nonnull MirrorGameServerInfo*) getGameServerInfo;

-(nullable NSNumber*) getGameType;

-(nonnull MirrorMatchInfo*) getMatchInfo;

-(nullable NSNumber*) getFormat;

-(BOOL) isSpectating;

-(nonnull MirrorAccountId*) getAccountId;

-(nonnull NSArray*) getDecks;

-(nullable NSNumber*) getSelectedDeck;

@end
