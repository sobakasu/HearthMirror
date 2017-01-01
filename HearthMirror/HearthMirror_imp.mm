/*
 *  HearthMirror.mm
 *  HearthMirror
 *
 *  Created by Istvan Fehervari on 26/12/2016.
 *  Copyright © 2016 com.ifehervari. All rights reserved.
 *
 */

#import "HearthMirror_imp.h"
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

-(instancetype) initWithPID:(pid_t)pid {
    
    self = [super init];
    if (self)
    {
        _mirror = new Mirror(pid);
    }
    return self;
}

-(nullable NSString*) getBattleTag {
    if (_mirror == NULL) return nil;

    BattleTag tag = _mirror->getBattleTag();
    NSString* battlename = [NSString stringWithu16string:tag.name];
    return [NSString stringWithFormat:@"%@#%d",battlename,tag.number];
}

-(nonnull NSArray*) getCardCollection {
    if (_mirror == NULL) return [NSArray array];

    NSMutableArray  *result = [NSMutableArray array];
    std::vector<Card> cards = _mirror->getCardCollection();
    for (int i = 0; i < cards.size(); i++) {
        Card card = cards[i];

        MirrorCard *mirrorCard = [MirrorCard new];
        mirrorCard.cardId = [NSString stringWithu16string:card.id];
        mirrorCard.count = @(card.count);
        mirrorCard.premium = card.premium;

        [result addObject:mirrorCard];
    }
    
    return [NSArray arrayWithArray:result];
}

-(nullable MirrorGameServerInfo*) getGameServerInfo {
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

-(nullable NSNumber *) getGameType {
    if (_mirror == NULL) return nil;

    return @(_mirror->getGameType());
}

-(nullable NSNumber *) getFormat {
    if (_mirror == NULL) return nil;

    return @(_mirror->getFormat());
}

-(nullable MirrorMatchInfo *) getMatchInfo {
    if (_mirror == NULL) return nil;
    MirrorMatchInfo *result = [MirrorMatchInfo new];

    InternalMatchInfo _matchInfo = _mirror->getMatchInfo();

    MirrorPlayer *localPlayer = [MirrorPlayer new];
    localPlayer.name = [NSString stringWithu16string:_matchInfo.localPlayer.name];
    if (localPlayer.name == nil) return nil;
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
    if (opposingPlayer.name == nil) return nil;
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

-(nonnull MirrorAccountId *) getAccountId {
    MirrorAccountId *result = [MirrorAccountId new];
    if (_mirror == NULL) return result;

    AccountId _account = _mirror->getAccountId();
    result.lo = @(_account.lo);
    result.hi = @(_account.hi);

    return result;
}

-(nonnull NSArray *) getDecks {
    if (_mirror == NULL) return [NSArray array];

    NSMutableArray *result = [NSMutableArray array];
    std::vector<Deck> decks = _mirror->getDecks();
    for (int i = 0; i < decks.size(); i++) {
        Deck deck = decks[i];
        MirrorDeck *mirrorDeck = [MirrorDeck new];
        mirrorDeck.id = @(deck.id);
        mirrorDeck.name = [NSString stringWithu16string:deck.name];
        mirrorDeck.hero = [NSString stringWithu16string:deck.hero];
        mirrorDeck.isWild = deck.isWild;
        mirrorDeck.type = @(deck.type);
        mirrorDeck.seasonId = @(deck.seasonId);
        mirrorDeck.cardBackId = @(deck.cardBackId);
        mirrorDeck.heroPremium = @(deck.heroPremium);

        NSMutableArray *cards = [NSMutableArray array];
        for (int c = 0; c < deck.cards.size(); c++) {
            Card card = deck.cards[c];
            MirrorCard *mirrorCard = [MirrorCard new];
            mirrorCard.cardId = [NSString stringWithu16string:card.id];
            mirrorCard.count = @(card.count);
            mirrorCard.premium = card.premium;

            [cards addObject:mirrorCard];
        }
        mirrorDeck.cards = [NSArray arrayWithArray:cards];

        [result addObject:mirrorDeck];
    }

    return [NSArray arrayWithArray:result];
}

-(BOOL) isSpectating {
    if (_mirror == NULL) return NO;

    return _mirror->isSpectating();
}

-(nullable NSNumber*) getSelectedDeck {
    if (_mirror == NULL) return NULL;

    long deckId = _mirror->getSelectedDeckInMenu();
    if (deckId == 0) return nil;

    return @(deckId);
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

@implementation MirrorCard
- (NSString *)description {
    return [NSString stringWithFormat:@"cardId: %@, count: %@, premium: %@", self.cardId, self.count, @(self.premium)];
}
@end

@implementation MirrorDeck
- (NSString *)description {
    return [NSString stringWithFormat:@"id: %@, name: %@, hero: %@, isWild: %@, type: %@, seasonId: %@, cardBackId: %@, heroPremium: %@, cards: %@", self.id, self.name, self.hero, @(self.isWild), self.type, self.seasonId, self.cardBackId, self.heroPremium, self.cards];
}
@end
