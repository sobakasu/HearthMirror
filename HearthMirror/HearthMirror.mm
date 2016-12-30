/*
 *  HearthMirror.mm
 *  HearthMirror
 *
 *  Created by Istvan Fehervari on 26/12/2016.
 *  Copyright © 2016 com.ifehervari. All rights reserved.
 *
 */

#import "HearthMirror.h"
#import "Mirror.hpp"

@interface NSString (cppstring_additions)
+(NSString*) stringWithu16string:(const std::u16string&)string;
@end

@implementation NSString (cppstring_additions)

+(NSString*) stringWithu16string:(const std::u16string&)ws
{
    char* data = (char*)ws.data();
    
    NSString* result = [[NSString alloc] initWithBytes:data length:ws.size()*2 encoding:NSUTF16LittleEndianStringEncoding];
    return result;
}

@end

using namespace hearthmirror;

@implementation HearthMirror {
    Mirror* _mirror;
}

-(id) initWithPID:(pid_t)pid {
    
    self = [super init];
    if (self)
    {
        _mirror = new Mirror(pid);
    }
    return self;
}

-(NSString*) getBattleTag {
    if (_mirror == NULL) return @"";

    BattleTag tag = _mirror->getBattleTag();
    NSString* battlename = [NSString stringWithu16string:tag.name];
    return [NSString stringWithFormat:@"%@#%d",battlename,tag.number];
}

-(NSArray*) getCardCollection {
    NSMutableArray* result = [NSMutableArray new];
    if (_mirror == NULL) return result;
    
    _mirror->getCardCollection();
    
    return result;
}

-(MirrorGameServerInfo*) getGameServerInfo {
    MirrorGameServerInfo *result = [MirrorGameServerInfo new];
    if (_mirror == NULL) return result;

    InternalGameServerInfo _serverInfo = _mirror->getGameServerInfo();
    result.address = [NSString stringWithu16string:_serverInfo.address];
    result.auroraPassword = [NSString stringWithu16string:_serverInfo.auroraPassword];
    result.clientHandle = @(_serverInfo.clientHandle);
    result.gameHandle = @(_serverInfo.gameHandle);
    result.mission = @(_serverInfo.mission);
    result.port = @(_serverInfo.port);
    result.resumable = _serverInfo.resumable;
    result.spectatorMode = _serverInfo.spectatorMode;
    result.spectatorPassword = [NSString stringWithu16string:_serverInfo.spectatorPassword];
    result.version = [NSString stringWithu16string:_serverInfo.version];

    return result;
}

-(NSNumber *) getGameType {
    if (_mirror == NULL) return nil;

    return @(_mirror->getGameType());
}

-(NSNumber *) getFormat {
    if (_mirror == NULL) return nil;

    return @(_mirror->getFormat());
}

-(MirrorMatchInfo *) getMatchInfo {
    MirrorMatchInfo *result = [MirrorMatchInfo new];
    if (_mirror == NULL) return result;

    InternalMatchInfo _matchInfo = _mirror->getMatchInfo();
    MirrorPlayer *localPlayer = [MirrorPlayer new];
    localPlayer.name = [NSString stringWithu16string:_matchInfo.localPlayer.name];
    localPlayer.playerId = @(_matchInfo.localPlayer.id);
    localPlayer.standardRank = @(_matchInfo.localPlayer.standardRank);
    localPlayer.standardLegendRank = @(_matchInfo.localPlayer.standardLegendRank);
    localPlayer.standardStars = @(_matchInfo.localPlayer.standardStars);
    localPlayer.wildRank = @(_matchInfo.localPlayer.wildRank);
    localPlayer.wildLegendRank = @(_matchInfo.localPlayer.wildLegendRank);
    localPlayer.wildStars = @(_matchInfo.localPlayer.wildStars);
    localPlayer.cardBackId = @(_matchInfo.localPlayer.cardBackId);
    result.localPlayer = localPlayer;

    MirrorPlayer *opposingPlayer = [MirrorPlayer new];
    opposingPlayer.name = [NSString stringWithu16string:_matchInfo.opposingPlayer.name];
    opposingPlayer.playerId = @(_matchInfo.opposingPlayer.id);
    opposingPlayer.standardRank = @(_matchInfo.opposingPlayer.standardRank);
    opposingPlayer.standardLegendRank = @(_matchInfo.opposingPlayer.standardLegendRank);
    opposingPlayer.standardStars = @(_matchInfo.opposingPlayer.standardStars);
    opposingPlayer.wildRank = @(_matchInfo.opposingPlayer.wildRank);
    opposingPlayer.wildLegendRank = @(_matchInfo.opposingPlayer.wildLegendRank);
    opposingPlayer.wildStars = @(_matchInfo.opposingPlayer.wildStars);
    opposingPlayer.cardBackId = @(_matchInfo.opposingPlayer.cardBackId);
    result.opposingPlayer = opposingPlayer;

    result.brawlSeasonId = @(_matchInfo.brawlSeasonId);
    result.missionId = @(_matchInfo.missionId);
    result.rankedSeasonId = @(_matchInfo.rankedSeasonId);

    return result;
}

-(MirrorAccountId *) getAccountId {
    MirrorAccountId *result = [MirrorAccountId new];
    if (_mirror == NULL) return result;

    AccountId _account = _mirror->getAccountId();
    result.lo = @(_account.lo);
    result.hi = @(_account.hi);

    return result;
}

-(BOOL) isSpectating {
    if (_mirror == NULL) return NO;

    return _mirror->isSpectating();
}

-(void)dealloc {
    delete _mirror;
}

@end

@implementation MirrorGameServerInfo
@end

@implementation MirrorPlayer
@end

@implementation MirrorMatchInfo
@end

@implementation MirrorAccountId
@end
