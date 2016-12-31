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
    @property NSString *_Nonnull address;
    @property NSString *_Nonnull auroraPassword;
    @property NSNumber *_Nonnull clientHandle;
    @property NSNumber *_Nonnull gameHandle;
    @property NSNumber *_Nonnull mission;
    @property NSNumber *_Nonnull port;
    @property BOOL resumable;
    @property BOOL spectatorMode;
    @property NSString *_Nonnull spectatorPassword;
    @property NSString *_Nonnull version;
@end

@interface MirrorPlayer: NSObject
    @property NSString *_Nonnull name;
    @property NSNumber *_Nonnull playerId;
    @property NSNumber *_Nonnull standardRank;
    @property NSNumber *_Nonnull standardLegendRank;
    @property NSNumber *_Nonnull standardStars;
    @property NSNumber *_Nonnull wildRank;
    @property NSNumber *_Nonnull wildLegendRank;
    @property NSNumber *_Nonnull wildStars;
    @property NSNumber *_Nonnull cardBackId;
@end

@interface MirrorMatchInfo: NSObject
    @property MirrorPlayer *_Nonnull localPlayer;
    @property MirrorPlayer *_Nonnull opposingPlayer;
    @property NSNumber *_Nonnull brawlSeasonId;
    @property NSNumber *_Nonnull missionId;
    @property NSNumber *_Nonnull rankedSeasonId;
@end

@interface MirrorAccountId: NSObject
    @property NSNumber *_Nonnull lo;
    @property NSNumber *_Nonnull hi;
@end

@interface MirrorCard: NSObject
    @property NSString *_Nonnull cardId;
    @property NSNumber *_Nonnull count;
    @property BOOL premium;
@end

@interface MirrorDeck: NSObject
    @property NSNumber *_Nonnull id;
    @property NSString *_Nonnull name;
    @property NSString *_Nonnull hero;
    @property BOOL isWild;
    @property NSNumber *_Nonnull type;
    @property NSNumber *_Nonnull seasonId;
    @property NSNumber *_Nonnull cardBackId;
    @property NSNumber *_Nonnull heroPremium;
    @property NSArray *_Nonnull cards;
@end

@interface HearthMirror : NSObject

-(nonnull instancetype) initWithPID:(pid_t)pid;

/** Returns the BattleTag as a single string in the following format: name#number. */
-(nullable NSString*) getBattleTag;

-(nonnull NSArray*) getCardCollection;

-(nullable MirrorGameServerInfo*) getGameServerInfo;

-(nullable NSNumber*) getGameType;

-(nullable MirrorMatchInfo*) getMatchInfo;

-(nullable NSNumber*) getFormat;

-(BOOL) isSpectating;

-(nonnull MirrorAccountId*) getAccountId;

-(nonnull NSArray*) getDecks;

-(nullable NSNumber*) getSelectedDeck;

@end
