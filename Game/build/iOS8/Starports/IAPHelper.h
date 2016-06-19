
// Add to the top of the file
#import <StoreKit/StoreKit.h>
UIKIT_EXTERN NSString *const IAPHelperProductPurchasedNotification;

typedef void(^RequestProductsCompletionHandler)(BOOL success, NSArray * products);

@interface IAPHelper : NSObject

- (id)initWithProductIdentifiers : (NSSet *)productIdentifiers;
-(void)requestProductsWithCompletionHandler:(RequestProductsCompletionHandler)completionHandler;
-(void)restoreCompletedTransactions;

// Add two new method declarations
-(void)buyProduct:(SKProduct *)product;
-(BOOL)productPurchased:(NSString *)productIdentifier;

@end

