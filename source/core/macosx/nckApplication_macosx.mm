
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#import "nckApplication_macosx.h"

#if defined(NCK_MACOSX)

#import "nckWindow_macosx.h"

static NCKApplication * appSingleton = nil;
static NCKAppDelegate * appDelegateSingleton = nil;
static NSArray * appRunArguments = nil;

@implementation NCKApplication

-(id)init{
    self = [super init];
    if(self){
        appSingleton = self;
    }
    return self;
}

- (void)dealloc{
    [super dealloc];
}

- (void)run
{
	[[NSNotificationCenter defaultCenter]
     postNotificationName:NSApplicationWillFinishLaunchingNotification
     object:NSApp];
    
	[[NSNotificationCenter defaultCenter]
     postNotificationName:NSApplicationDidFinishLaunchingNotification
     object:NSApp];
	
    std::vector<std::string> args;
    for(NSString * a : appRunArguments)
        args.push_back(std::string([a UTF8String]));
                       
    Core::Application_Main_MacOSX(args);
}

- (BOOL)processEvents{
    NSEvent *event =
    [self nextEventMatchingMask:NSAnyEventMask
                      untilDate:[NSDate distantFuture]
                         inMode:NSDefaultRunLoopMode
                        dequeue:YES];
    
    [self sendEvent:event];
    [self updateWindows];
    
    return shouldKeepRunning;
}

- (void)terminate:(id)sender
{
	shouldKeepRunning = NO;
}

+ (NCKApplication*)singleton{
    return appSingleton;
}

+ (void)appMain:(NSArray*)arguments{
    appRunArguments = arguments;
    
	NSDictionary * infoDictionary = [[NSBundle mainBundle] infoDictionary];
	
    Class principalClass = NSClassFromString([infoDictionary objectForKey:@"NSPrincipalClass"]);
    
	NSApplication * applicationObject = [principalClass sharedApplication];
    
	NSString * mainNibName = [infoDictionary objectForKey:@"NSMainNibFile"];
    
	NSNib * mainNib = [[NSNib alloc] initWithNibNamed:mainNibName bundle:[NSBundle mainBundle]];
    
	[mainNib instantiateWithOwner:applicationObject topLevelObjects:nil];
    
	if ([applicationObject respondsToSelector:@selector(run)])
	{
		[applicationObject performSelectorOnMainThread:@selector(run) withObject:nil waitUntilDone:YES];
	}

	[mainNib release];
}

@end

@implementation NCKAppDelegate

-(id)init{
    self = [super init];
    if(self){
        appDelegateSingleton = self;
    }
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification{

}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender{
    return TRUE;
}

- (void)dealloc
{
    [super dealloc];
}

+(NCKAppDelegate*)singleton{
    return appDelegateSingleton;
}

@end

#endif // #if defined(NCK_MACOSX)