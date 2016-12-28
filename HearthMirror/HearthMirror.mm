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
    
    NSString* result = [[NSString alloc] initWithBytes:data length:ws.size() encoding:NSUTF16LittleEndianStringEncoding];
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

-(void)dealloc {
    delete _mirror;
}

@end
