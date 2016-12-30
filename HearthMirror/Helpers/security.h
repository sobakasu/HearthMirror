//
//  security.h
//  HearthMirror
//
//  Created by Istvan Fehervari on 26/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef security_h
#define security_h

#ifdef __APPLE__
/** Requests access to attach to another process. Required for HearthMirror to function properly. */
int acquireTaskportRight();
#endif

#endif /* security_h */
