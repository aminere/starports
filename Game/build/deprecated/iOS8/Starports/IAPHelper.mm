// 1
#import "IAPHelper.h"
#import <StoreKit/StoreKit.h>

// Add to top of file
NSString *const IAPHelperProductPurchasedNotification = @"IAPHelperProductPurchasedNotification";
extern std::vector<std::wstring> IAPPreviouslyPurchasedProducts;

// 2
@interface IAPHelper() <SKProductsRequestDelegate, SKPaymentTransactionObserver>
@end

@implementation IAPHelper{
	// 3
	SKProductsRequest * _productsRequest;
	// 4
	RequestProductsCompletionHandler _completionHandler;
	NSSet * _productIdentifiers;
	NSMutableSet * _purchasedProductIdentifiers;
}

-(id)initWithProductIdentifiers:(NSSet *)productIdentifiers
{
	if ((self = [super init])) 
	{
		// Store product identifiers
		_productIdentifiers = productIdentifiers;

		// Check for previously purchased products
		IAPPreviouslyPurchasedProducts.clear();
		_purchasedProductIdentifiers = [NSMutableSet set];
		for (NSString * productIdentifier in _productIdentifiers)
		{
			BOOL productPurchased = [[NSUserDefaults standardUserDefaults] boolForKey:productIdentifier];
			if (productPurchased)
			{
				[_purchasedProductIdentifiers addObject : productIdentifier];
				NSLog(@"Previously purchased: %@", productIdentifier);

				NSStringEncoding pEncode = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
				NSData* pSData = [productIdentifier dataUsingEncoding : pEncode];
				auto wstr = std::wstring((wchar_t*)[pSData bytes], [pSData length] / sizeof(wchar_t));
				IAPPreviouslyPurchasedProducts.push_back(wstr);				
			}
			else
			{
				NSLog(@"Not purchased: %@", productIdentifier);
			}
		}
	}

	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];

	return self;
}

-(void)requestProductsWithCompletionHandler:(RequestProductsCompletionHandler)completionHandler{

	// 1
	_completionHandler = [completionHandler copy];

	// 2
	_productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:_productIdentifiers];
	_productsRequest.delegate = self;
	[_productsRequest start];

}

-(BOOL)productPurchased:(NSString *)productIdentifier
{
	return[_purchasedProductIdentifiers containsObject : productIdentifier];
}

-(void)buyProduct : (SKProduct *)product
{
	NSLog(@"Buying %@...", product.productIdentifier);

	SKPayment * payment = [SKPayment paymentWithProduct : product];
	[[SKPaymentQueue defaultQueue] addPayment:payment];
}

-(void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions : (NSArray *)transactions
{
	for (SKPaymentTransaction * transaction in transactions) 
	{
		switch (transaction.transactionState)
		{
		case SKPaymentTransactionStatePurchased:
			[self completeTransaction : transaction];
			break;
		case SKPaymentTransactionStateFailed:
			[self failedTransaction : transaction];
			break;
		case SKPaymentTransactionStateRestored:
			[self restoreTransaction : transaction];
		default:
			break;
		}
	};
}

-(void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError : (NSError *)error
{
	shoot::GameEngine::Instance()->GetGameMgr()->OnSignInCancelled();
}

-(void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
	NSLog(@"received restored transactions: %i", queue.transactions.count);
	bool restored = false;
	for (SKPaymentTransaction *transaction in queue.transactions)
	{
		NSStringEncoding pEncode = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
		NSData* pSData = [transaction.payment.productIdentifier dataUsingEncoding : pEncode];
		auto wstr = std::wstring((wchar_t*)[pSData bytes], [pSData length] / sizeof(wchar_t));
		if (shoot::GameEngine::Instance()->GetGameMgr()->GetGameData()->OnProductPurchased(wstr))
			restored = true;
	}

	if (!restored)
		shoot::GameEngine::Instance()->GetGameMgr()->OnNothingToRestore();
	else
		shoot::GameEngine::Instance()->GetGameMgr()->OnTransactionCompleted(true);
}

-(void)completeTransaction:(SKPaymentTransaction *)transaction
{
	NSLog(@"completeTransaction...");

	[self provideContentForProductIdentifier : transaction.payment.productIdentifier];

	NSStringEncoding pEncode = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
	NSData* pSData = [transaction.payment.productIdentifier dataUsingEncoding : pEncode];
	auto wstr = std::wstring((wchar_t*)[pSData bytes], [pSData length] / sizeof(wchar_t));
	shoot::GameEngine::Instance()->GetGameMgr()->GetGameData()->OnProductPurchased(wstr);
	shoot::GameEngine::Instance()->GetGameMgr()->OnTransactionCompleted(true);

	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void)restoreTransaction:(SKPaymentTransaction *)transaction
{
	NSLog(@"restoreTransaction...");

	[self provideContentForProductIdentifier : transaction.originalTransaction.payment.productIdentifier];
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void)failedTransaction:(SKPaymentTransaction *)transaction{

	NSLog(@"failedTransaction...");
	if (transaction.error.code != SKErrorPaymentCancelled)
	{
		NSLog(@"Transaction error: %@", transaction.error.localizedDescription);
	}

	shoot::GameEngine::Instance()->GetGameMgr()->OnTransactionCompleted(false);
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}
// Add new method
-(void)provideContentForProductIdentifier:(NSString *)productIdentifier
{
	[_purchasedProductIdentifiers addObject : productIdentifier];
	[[NSUserDefaults standardUserDefaults] setBool:YES forKey : productIdentifier];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[[NSNotificationCenter defaultCenter] postNotificationName:IAPHelperProductPurchasedNotification object : productIdentifier userInfo : nil];
}

#pragma mark - SKProductsRequestDelegate

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse : (SKProductsResponse *)response
{
	NSLog(@"Loaded list of products...");
	_productsRequest = nil;

	NSArray * skProducts = response.products;
	for (SKProduct * skProduct in skProducts) 
	{
		NSLog(@"Found product: %@ %@ %0.2f",
			skProduct.productIdentifier,
			skProduct.localizedTitle,
			skProduct.price.floatValue);
	}

	_completionHandler(YES, skProducts);
	_completionHandler = nil;
}

-(void)request:(SKRequest *)request didFailWithError : (NSError *)error
{
	NSLog(@"Failed to load list of products.");
	_productsRequest = nil;

	_completionHandler(NO, nil);
	_completionHandler = nil;
}

-(void)restoreCompletedTransactions
{
	[[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

@end

