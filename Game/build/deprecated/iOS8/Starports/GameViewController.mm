//
//  GameViewController.m
//  Starports
//
//  Created by Amine Rehioui on 2015-03-15.
//  Copyright (c) 2015 Frantic. All rights reserved.
//

#import "GameViewController.h"
#import <OpenGLES/ES2/glext.h>

#include "Mutex.h"
#include "File.h"
#include "MemoryManager.h"
#include "InputManager.h"
#include "OpenGLDriverES2.h"
#include "InputRecorder.h"

#include "GameEngine.h"
#include "GameData.h"

#import "GameIAPHelper.h"

#include <iAd/iAd.h>

extern void RegisterObjects();

float _touchTimer = -1.0f;
bool _pinch = true;
NSArray* _products = nil;
std::vector<std::wstring> IAPPreviouslyPurchasedProducts;

CGSize _finalScreenSize;
void FinalRenderTargetSetter()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 1);
	glViewport(0, 0, _finalScreenSize.width, _finalScreenSize.height);
}

@interface GameViewController ()
{
	shoot::Engine* _engine;
	shoot::Mutex* _inputMutex;
	std::list<shoot::InputManager::TouchState> _touchStates;
    UITouch *_currentTouch;	
	CGSize _engineScreenSize;
	CGFloat _screenScale;
	NSArray *_products;
    
    BOOL _adVisible;
    ADBannerView* _adBanner;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)tearDownGL;
@end

@implementation GameViewController

- (void)viewDidLoad
{
    [super viewDidLoad];	
	
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
	_inputMutex = shoot::Mutex::Create();
	const char* strResourcePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	const char* strWritablePath = [documentsDirectory UTF8String];
	shoot::File::SetBasePath(strResourcePath);
	shoot::File::SetWritablePath(strWritablePath);
	
   [EAGLContext setCurrentContext:self.context];
   _engine = new shoot::GameEngine(&RegisterObjects, shoot::InputManager::IMT_Touch);
   
   CGRect screenBounds = [[UIScreen mainScreen] bounds];
   _screenScale = [[UIScreen mainScreen] scale];
   _engineScreenSize = screenBounds.size;
   _finalScreenSize = CGSizeMake(_engineScreenSize.width * _screenScale, _engineScreenSize.height * _screenScale);
   //_engineScreenSize = _finalScreenSize;
   _screenScale = 1.0f;
   _engine->ResizeScreen(shoot::Vector2::Create(_engineScreenSize.width, _engineScreenSize.height));
   _engine->Init();
   
   shoot::OpenGLDriverES2* es2Driver = (shoot::OpenGLDriverES2*)shoot::GraphicsDriver::Instance();
   es2Driver->SetFinalRenderTargetSetter(&FinalRenderTargetSetter);

   _touchTimer = -1.0f;
   _pinch = false;
    
    _adVisible = false;
	for (auto& id : IAPPreviouslyPurchasedProducts)
		shoot::GameEngine::Instance()->GetGameMgr()->GetGameData()->OnProductPurchased(id);
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];   

    //_adBanner = [[ADBannerView alloc] initWithFrame:CGRectMake(0, self.view.frame.size.height - 50, 320, 50)];
    //_adBanner.delegate = self;
}

/*-(void)viewWillAppear:(BOOL)animated
{
	[[NSNotificationCenter defaultCenter] addObserver:self selector : @selector(productPurchased:) name:IAPHelperProductPurchasedNotification object : nil];
}

-(void)viewWillDisappear : (BOOL)animated
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}*/

- (void)dealloc
{    
    [self tearDownGL];
	
	delete _inputMutex;
	delete _engine;
	
    if ([EAGLContext currentContext] == self.context) 
	{
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) 
	{
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) 
		{
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];   
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
	if(_pinch)
	{
		if(_touchTimer < 0.0f)
		{
            shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Left, false);
			shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Right, false);
			_pinch = false;
		}
        else
            _touchTimer -= shoot::g_fDeltaTime;
        
        shoot::InputManager::TouchState state;
        state.bPressed = false;
        shoot::InputManager::Instance()->SetTouchState(state);
	}
	else
	{
		//_inputMutex->Lock();
		if(_touchStates.size())
		{
			if(_touchTimer < 0.0f)
			{
				shoot::InputManager::Instance()->SetTouchState(_touchStates.front());
				_touchStates.pop_front();
			}
			else
				_touchTimer -= shoot::g_fDeltaTime;
		}
		//_inputMutex->Unlock();
	}
	
	shoot::InputManager::Instance()->Update();
	
	_engine->Update(self.timeSinceLastUpdate);	
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    _engine->Render();
}

#pragma mark -  Touch Input

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* touch = [[event allTouches] anyObject];
    
	// if in replay mode, go back to start screen
	auto gameMgr = shoot::GameEngine::Instance()->GetGameMgr();
	if (gameMgr->GetState() == shoot::GameManager::S_Replay)
	{		
		shoot::InputRecorder::Instance()->StopReplay();
		gameMgr->ResetMenuTimer();		
	}
    else if (gameMgr->GetState() == shoot::GameManager::S_Menu)
	{
		gameMgr->ResetMenuTimer();
	}
	
    if(_currentTouch) 
		return;
    
	_currentTouch = touch;
	CGPoint location = [_currentTouch locationInView:_currentTouch.view];
	
	shoot::InputManager::TouchState state;
	state.bPressed = true;
	state.vPosition.Set(location.x*_screenScale, location.y*_screenScale);
	//_inputMutex->Lock();
    _touchStates.clear();
	_touchStates.push_back(state);
	//_inputMutex->Unlock();

    shoot::GameEngine* engine = (shoot::GameEngine*)shoot::Engine::Instance();
    if(!engine)
        return;
    shoot::GameManager* mgr = engine->GetGameMgr();
    if(mgr->GetState() != shoot::GameManager::S_InGame)
        return;
	_touchTimer = 0.05f;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if(_currentTouch)
    {        
        CGPoint location = [_currentTouch locationInView:_currentTouch.view];
		_currentTouch = nil;
		
		shoot::InputManager::TouchState state;        
        state.bPressed = false;
        state.vPosition.Set(location.x*_screenScale, location.y*_screenScale);
		//_inputMutex->Lock();
		_touchStates.push_back(state);
		//_inputMutex->Unlock();
    }   
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	_currentTouch = nil;
	
	//if(!_touchStates.empty())
	//{
		//shoot::InputManager::TouchState lastState = _touchStates.back();
		//lastState.bPressed = false;
		_touchStates.clear();
		//_touchStates.push_back(lastState);
	//}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* touch = [[event allTouches] anyObject];
    if(!_currentTouch || touch != _currentTouch) return;
	
	CGPoint location = [_currentTouch locationInView:_currentTouch.view];	
	
	shoot::InputManager::TouchState state;        
    state.bPressed = true;
    state.vPosition.Set(location.x*_screenScale, location.y*_screenScale);
	//_inputMutex->Lock();
	_touchStates.clear();
	_touchStates.push_back(state);
	//_inputMutex->Unlock()
}

- (void)bannerViewDidLoadAd:(ADBannerView*)banner
{
    if(!_adVisible)
    {
        if(_adBanner.superview == nil)
        {
            [self.view addSubview:_adBanner];
        }
        
        [UIView beginAnimations:@"animateAdBannerOn" context:NULL];
        banner.frame = CGRectOffset(banner.frame, 0, -banner.frame.size.height);
        
        [UIView commitAnimations];
        
        _adVisible = YES;
    }
}

- (void)bannerView:(ADBannerView*)banner didFailToReceiveAdWithError:(NSError*)error
{
    NSLog(@"Failed to retreive ad");
    
    if(_adVisible)
    {
        [UIView beginAnimations:@"animateAdBannerOff" context:NULL];
        banner.frame = CGRectOffset(banner.frame, 0, banner.frame.size.height);
        [UIView commitAnimations];
        _adVisible = NO;
    }
}

- (IBAction)pinch:(UIPinchGestureRecognizer *) sender
{
    float scale = [sender scale];
	bool zoomIn = scale < 1.0f;
	shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Left, !zoomIn);
	shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Right, zoomIn);
	_pinch = true;
	_touchTimer = 0.05f;
    _touchStates.clear();
}

std::wstring NSStringToStringW(NSString* Str)
{
	NSStringEncoding pEncode = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
	NSData* pSData = [Str dataUsingEncoding : pEncode];
	return std::wstring((wchar_t*)[pSData bytes], [pSData length] / sizeof(wchar_t));
}

void iOSRequestProducts()
{
	_products = nil;	
	[[GameIAPHelper sharedInstance] requestProductsWithCompletionHandler:^ (BOOL success, NSArray *products)
	{
		if (success)
		{
			NSNumberFormatter * _priceFormatter = [[NSNumberFormatter alloc] init];
			[_priceFormatter setFormatterBehavior : NSNumberFormatterBehavior10_4];
			[_priceFormatter setNumberStyle : NSNumberFormatterCurrencyStyle];

			_products = products;
			for (SKProduct * skProduct in _products)
			{
				std::wstring id = NSStringToStringW(skProduct.productIdentifier);
				std::wstring name = NSStringToStringW(skProduct.localizedTitle);				
				[_priceFormatter setLocale : skProduct.priceLocale];
				std::wstring price = NSStringToStringW([_priceFormatter stringFromNumber : skProduct.price]);
				shoot::GameEngine::Instance()->GetGameMgr()->GetGameData()->AddProductInfo(id, name, price);
			}
		}

		shoot::GameEngine::Instance()->GetGameMgr()->OnProductsRetrieved(success == YES);
	}];
}

void iOSBuyProduct(const std::wstring& id)
{
	if (_products == nil)
	{
		shoot::Log << "iOSBuyProduct faild product list is NULL!\n";
		return;
	}

	for (SKProduct * skProduct in _products)
	{
		std::wstring _id = NSStringToStringW(skProduct.productIdentifier);
		if (_id == id)
		{
			NSLog(@"Buying %@...", skProduct.productIdentifier);
			[[GameIAPHelper sharedInstance] buyProduct:skProduct];
			break;
		}
	}
}

void iOSRestorePurchases()
{
	[[GameIAPHelper sharedInstance] restoreCompletedTransactions];
}

-(void)productPurchased:(NSNotification *)notification
{
	NSString * productIdentifier = notification.object;
	[_products enumerateObjectsUsingBlock : ^ (SKProduct * product, NSUInteger idx, BOOL *stop)
	{
		if ([product.productIdentifier isEqualToString : productIdentifier])
		{
			shoot::GameEngine::Instance()->GetGameMgr()->GetGameData()->OnProductPurchased(NSStringToStringW(productIdentifier));
			*stop = YES;
		}
	}];
}

@end
