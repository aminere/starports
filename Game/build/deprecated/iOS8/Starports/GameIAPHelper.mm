#import "GameIAPHelper.h"

@implementation GameIAPHelper

+ (GameIAPHelper *)sharedInstance{
	static dispatch_once_t once;
	static GameIAPHelper * sharedInstance;
	dispatch_once(&once, ^{
		NSSet * productIdentifiers = [NSSet setWithObjects :
		@"com.frantic.starports.extramode1",
			nil];
		sharedInstance = [[self alloc] initWithProductIdentifiers:productIdentifiers];
	});
	return sharedInstance;
}

@end

