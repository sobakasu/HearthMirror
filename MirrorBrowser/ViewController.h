//
//  ViewController.h
//  MirrorBrowser
//
//  Created by Istvan Fehervari on 14/06/2017.
//  Copyright © 2017 com.ifehervari. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface TreeElement: NSObject

@property NSString *name;
@property NSString *type;
@property NSString *value;
@property NSMutableArray *children;

@end

@interface ViewController : NSViewController<NSOutlineViewDataSource, NSOutlineViewDelegate>

@property (nonatomic, weak) IBOutlet NSOutlineView *tableView;
@property NSMutableArray *treeElements;

- (IBAction)reload:(id)sender;

@end

