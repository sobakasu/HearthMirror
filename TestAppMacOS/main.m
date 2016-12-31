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

        // Get Account id
        MirrorAccountId *accountId = [mirror getAccountId];
        NSLog(@"AccountId: %@,%@", accountId.hi, accountId.lo);
        
        // Get card collection
        NSArray* collection = [mirror getCardCollection];
        NSLog(@"Collection : %@", collection);

        // Get Game server info
        MirrorGameServerInfo *serverInfo = [mirror getGameServerInfo];
        NSLog(@"GameServerInfo: address : %@, auroraPassword: %@, clientHandle: %@, gameHandle: %@, mission: %@, port: %@, resumable: %d, spectatorMode: %d, spectatorPassword: %@, version: %@", serverInfo.address, serverInfo.auroraPassword, serverInfo.clientHandle, serverInfo.gameHandle, serverInfo.mission, serverInfo.port, serverInfo.resumable, serverInfo.spectatorMode, serverInfo.spectatorPassword, serverInfo.version);

        // Get the game type
        // GT_UNKNOWN = 0,
        // GT_VS_AI = 1,
        // GT_VS_FRIEND = 2,
        // GT_TUTORIAL = 4,
        // GT_ARENA = 5,
        // GT_TEST = 6,
        // GT_RANKED = 7,
        // GT_CASUAL = 8,
        // GT_TAVERNBRAWL = 16,
        // GT_TB_1P_VS_AI = 17,
        // GT_TB_2P_COOP = 18,
        // GT_LAST = 19,
        NSNumber* gameType = [mirror getGameType];
        NSLog(@"GameType: %@", gameType);

        // Get the format
        // FT_UNKNOWN = 0,
        // FT_WILD = 1,
        // FT_STANDARD = 2,
        NSNumber* format = [mirror getFormat];
        NSLog(@"Format: %@", format);

        // Get the match info
        MirrorMatchInfo *matchInfo = [mirror getMatchInfo];
        NSLog(@"MatchInto: brawlSeasonId: %@, missionId: %@, rankedSessionId: %@", matchInfo.brawlSeasonId, matchInfo.missionId, matchInfo.rankedSeasonId);
        NSLog(@"MatchInto.localPlayer: name: %@, playerId: %@, standardRank: %@, standardLegendRank: %@, standardStars: %@, wildRank: %@, wildLegendRank: %@, wildStars: %@, cardBackId: %@", matchInfo.localPlayer.name, matchInfo.localPlayer.playerId, matchInfo.localPlayer.standardRank, matchInfo.localPlayer.standardLegendRank, matchInfo.localPlayer.standardStars, matchInfo.localPlayer.wildRank, matchInfo.localPlayer.wildLegendRank, matchInfo.localPlayer.wildStars, matchInfo.localPlayer.cardBackId);
        NSLog(@"MatchInto.opposingPlayer: name: %@, playerId: %@, standardRank: %@, standardLegendRank: %@, standardStars: %@, wildRank: %@, wildLegendRank: %@, wildStars: %@, cardBackId: %@", matchInfo.opposingPlayer.name, matchInfo.opposingPlayer.playerId, matchInfo.opposingPlayer.standardRank, matchInfo.opposingPlayer.standardLegendRank, matchInfo.opposingPlayer.standardStars, matchInfo.opposingPlayer.wildRank, matchInfo.opposingPlayer.wildLegendRank, matchInfo.opposingPlayer.wildStars, matchInfo.opposingPlayer.cardBackId);
        // Get the decks
        NSArray *decks = [mirror getDecks];
        NSLog(@"Decks: %@", decks);

        // Get the selected deck
        NSNumber *deckId = [mirror getSelectedDeck];
        NSLog(@"Selected deck: %@", deckId);
        if (deckId.longValue > 0) {

        }
    }
    return 0;
}
