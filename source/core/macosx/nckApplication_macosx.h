
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */
 
#ifndef NCK_APPLICATION_MACOSX_H
#define NCK_APPLICATION_MACOSX_H

#include "../../nckConfig.h"

#if defined(NCK_MACOSX)

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import <Foundation/Foundation.h>

@interface NCKApplication : NSApplication
{
	bool shouldKeepRunning;
}
- (void)run;
- (void)terminate:(id)sender;
- (BOOL)processEvents;
+ (NCKApplication*)singleton;
+ (void)appMain:(NSArray*)arguments;
@end

@interface NCKAppDelegate : NSObject <NSApplicationDelegate,NSWindowDelegate>
+(NCKAppDelegate*)singleton;
@end

#endif // #if defined(NCK_MACOSX)

#endif // NCK_APPLICATION_MACOSX_H
