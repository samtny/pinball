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

PinballBridgeInterface::PinballBridgeInterface(void) : self (NULL) {
    
}

PinballBridgeInterface::~PinballBridgeInterface(void) {
    [(id)self dealloc];
}

bool PinballBridgeInterface::init(void) {
    self = [[PinballBridge alloc] init];
    [(PinballBridge *)self initI];
    return YES;
}

-(void)initI {
    
#ifndef TARGET_IPHONE_SIMULATOR
    SoundManager *s = [[SoundManager alloc] init];
    
    [s loadSoundWithKey:@"flip" musicFile:@"flip.caf"];
    
    _soundManager = s;
#endif
    
}

-(void)dealloc {
    
    [_soundManager release];
    
}

void PinballBridgeInterface::setGameName(const char *gameName) {
    [(id)self setGameName:gameName];
}

const char * PinballBridgeInterface::getPathForScriptFileName(void * scriptFileName) {
    return [(id)self getPathForScriptFileName:scriptFileName];
}

const char * PinballBridgeInterface::getPathForTextureFileName(void * textureFileName) {
    return [(id)self getPathForTextureFileName:textureFileName];
}

Texture * PinballBridgeInterface::createRGBATexture(void *textureFileName) {
    return [(id)self createRGBATexture:textureFileName];
}

HostProperties * PinballBridgeInterface::getHostProperties() {
    return [(id)self getHostProperties];
}

void PinballBridgeInterface::playSound(void * soundName) {
    [(id)self playSound:(const char *)soundName];
}

void PinballBridgeInterface::addTimer(float duration, int id) {
    // TODO: something
}

void PinballBridgeInterface::setTimerDelegate(ITimerDelegate *timerDelegate) {
    // TODO: something
}

-(void)setGameName:(const char *)gameName {
    
    delete(_gameName);
    _gameName = gameName;
    
}

-(const char *)getPathForScriptFileName:(void *)scriptFileName {
    NSString *prefix = [[NSString stringWithUTF8String:(const char *)scriptFileName] stringByDeletingPathExtension];
    NSString *suffix = [[NSString stringWithUTF8String:(const char *)scriptFileName] pathExtension];
    NSString *dir = [NSString stringWithUTF8String:_gameName];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:prefix ofType:suffix inDirectory:dir];
    const char *path = [filePath UTF8String];
    return path;
}

-(const char *)getPathForTextureFileName:(void *)textureFileName {
    NSString *prefix = [[NSString stringWithUTF8String:(const char *)textureFileName] stringByDeletingPathExtension];
    NSString *suffix = [[NSString stringWithUTF8String:(const char *)textureFileName] pathExtension];
    NSString *dir = [[NSString stringWithUTF8String:_gameName] stringByAppendingPathComponent:@"textures"];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:prefix ofType:suffix inDirectory:dir];
    const char *path = [filePath UTF8String];
    return path;
}

-(Texture *)createRGBATexture:(void *)textureFileName {
    
    Texture *tex = new Texture();
    
    NSString *path = [NSString stringWithUTF8String:[self getPathForTextureFileName:textureFileName]];
    NSData *data = [[NSData data] initWithContentsOfFile:path];
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

-(HostProperties *)getHostProperties {
    HostProperties *props = new HostProperties();
    
    props->viewportX = 0;
    props->viewportY = 0;
    
    CGSize size = [PinballBridge windowCurrentSize];
    
    props->viewportHeight = size.height;
    props->viewportWidth = size.width;
    
    float scale = size.width / 800.0f;
    props->fontScale = scale;
    
    return props;
}

-(void)playSound:(const char *)soundName {
    
    NSLog(@"playSound:%@", [NSString stringWithUTF8String:soundName]);
    
    NSString *name = [NSString stringWithCString:(const char *)soundName encoding:NSUTF8StringEncoding];
    
    [_soundManager playSoundWithKey:name gain:0 pitch:0 location:CGPointMake(0, 0) shouldLoop:NO sourceID:0];
    
}

@end
