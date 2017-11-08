//
//  PinballNative.m
//  PorkyPinball
//
//  Created by Richard Thompson on 11/8/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import "PinballBridge.h"
#import "SoundManager.h"

@interface PinballBridge() {
    SoundManager *_soundManager;
    const char *_gameName;
    const ITimerDelegate *_timerDelegate;
}

+(CGSize)windowCurrentSize;
+(CGSize)windowSizeInOrientation:(UIInterfaceOrientation)orientation;
    
@end

@implementation PinballBridge

+(CGSize)windowCurrentSize {
    return [PinballBridge windowSizeInOrientation:[UIApplication sharedApplication].statusBarOrientation];
}

+(CGSize)windowSizeInOrientation:(UIInterfaceOrientation)orientation {
    
    CGSize size = [UIScreen mainScreen].bounds.size;
    UIApplication *application = [UIApplication sharedApplication];
    if (UIInterfaceOrientationIsLandscape(orientation)) {
        size = CGSizeMake(size.height, size.width);
    }
    if (application.statusBarHidden == NO) {
        size.height -= MIN(application.statusBarFrame.size.width, application.statusBarFrame.size.height);
        
    }
    size.width *= [UIScreen mainScreen].scale;
    size.height *= [UIScreen mainScreen].scale;
    return size;
}

PinballBridgeInterface::PinballBridgeInterface(void) {
    self = [[PinballBridge alloc] init];
}

PinballBridgeInterface::~PinballBridgeInterface(void) {
    [(id)self dealloc];
}

//bool PinballBridgeInterface::init(void) {
//    [(PinballBridge *)self initI];
//    return YES;
//}



-(void)initI {
    
#if !TARGET_IPHONE_SIMULATOR
    NSLog(@"loading sounds");
    SoundManager *s = [[SoundManager alloc] init];
    
    [s loadSoundWithKey:@"flip" musicFile:@"flip.caf"];
    
    _soundManager = s;
#endif
    
}

-(void)dealloc {
    
    [_soundManager release];
    [super dealloc];
}

void PinballBridgeInterface::setGameName(const char *gameName) {
    [(id)self setGameName:gameName];
}

const char *PinballBridgeInterface::getGameName() {
    return [(id)self getGameName];
}

const char * PinballBridgeInterface::getScriptPath(const char *scriptName) {
    return [(id)self getScriptPath:scriptName];
}

const char * PinballBridgeInterface::getTexturePath(const char * textureName) {
    return [(id)self getTexturePath:textureName];
}

GLTexture * PinballBridgeInterface::createRGBATexture(void *textureFileName) {
    return [(id)self createRGBATexture:textureFileName];
}

const HostProperties * PinballBridgeInterface::getHostProperties() {
    return [(id)self getHostProperties];
}

void PinballBridgeInterface::playSound(const char * soundName) {
    [(id)self playSound:soundName];
}

void PinballBridgeInterface::addTimer(float duration, int timerId, const ITimerDelegate *timerDelegate) {
    [(id)self addTimer:timerId duration:duration delegate:timerDelegate];
}

-(void)setGameName:(const char *)gameName {
    
    delete(_gameName);
    _gameName = gameName;
    
}

-(const char *)getGameName {
    return _gameName;
}

-(const char *)getScriptPath:(const char *)scriptFileName {
    NSString *prefix = [[NSString stringWithUTF8String:scriptFileName] stringByDeletingPathExtension];
    NSString *suffix = [[NSString stringWithUTF8String:scriptFileName] pathExtension];
    NSString *dir = [@"resource" stringByAppendingPathComponent:[NSString stringWithUTF8String:_gameName]];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:prefix ofType:suffix inDirectory:dir];
    const char *path = [filePath UTF8String];
    return path;
}



-(const char *)getTexturePath:(const char *)textureFileName {
    NSString *prefix = [[NSString stringWithUTF8String:(const char *)textureFileName] stringByDeletingPathExtension];
    NSString *suffix = [[NSString stringWithUTF8String:(const char *)textureFileName] pathExtension];
    NSString *dir = [@"resource" stringByAppendingPathComponent:[[NSString stringWithUTF8String:_gameName] stringByAppendingPathComponent:@"textures"]];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:prefix ofType:suffix inDirectory:dir];
    const char *path = [filePath UTF8String];
    return path;
}

-(GLTexture *)createRGBATexture:(void *)textureFileName {
    
    // TODO: "inject" this into a something instead;
    GLTexture *tex = new GLTexture();
    
    NSString *path = [NSString stringWithUTF8String:[self getTexturePath:(const char *)textureFileName]];
    NSData *data = [NSData dataWithContentsOfFile:path];
    UIImage *image = [UIImage imageWithData:data];
    
    GLuint width = CGImageGetWidth(image.CGImage);
    GLuint height = CGImageGetHeight(image.CGImage);
    GLuint bpp = 8;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    void *imageData = malloc(height * width * 4);
    CGContextRef context = CGBitmapContextCreate(imageData, width, height, bpp, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);
    CGContextClearRect(context, CGRectMake(0, 0, width, height));
    CGContextTranslateCTM(context, 0, height - height);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image.CGImage);
    
    CGContextRelease(context);
    
    tex->width = width;
    tex->height = height;
    tex->bpp = 4; // TODO: nah...
    tex->data = imageData;
    
    return tex;
    
}

-(const HostProperties *)getHostProperties {
    HostProperties *props = new HostProperties();
    
    props->viewportX = 0;
    props->viewportY = 0;
    
    CGSize size = [PinballBridge windowCurrentSize];
    
    props->viewportHeight = size.height;
    props->viewportWidth = size.width;
    
    float scale = size.width / 800.0f;
    props->fontScale = scale;
    
    props->overlayScale = 0.5;
    
    return props;
}

-(void)playSound:(const char *)soundName {
    
    //NSLog(@"playSound:%@", [NSString stringWithUTF8String:soundName]);
    
    NSString *name = [NSString stringWithCString:(const char *)soundName encoding:NSUTF8StringEncoding];
    
    NSUInteger result = [_soundManager playSoundWithKey:name gain:1.0f pitch:1.0f location:CGPointMake(0, 0) shouldLoop:NO sourceID:-1];
    
    NSLog(@"result: %d\n", result);
    
}

-(void)timerCallback:(NSNumber *)timerId {
    NSLog(@"timer: %d\n", [timerId intValue]);
    _timerDelegate->timerCallback([timerId intValue]);
}

-(void)addTimer:(int)timerId duration:(float)duration delegate:(const ITimerDelegate *)timerDelegate {
    _timerDelegate = timerDelegate;

    [self performSelector:@selector(timerCallback:) withObject:[NSNumber numberWithInt:timerId] afterDelay:duration];
}

@end
