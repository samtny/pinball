//
//  ViewController.m
//  PorkyPinball
//
//  Created by Richard Thompson on 11/7/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

#import "GameViewController.h"

#import "PinballBridgeInterface.h"

#import "Playfield.h"

#import "Physics.h"

#import "Camera.h"

#import "Editor.h"

#import "Renderer.h"

#import "Game.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Uniform index.
enum
{
    UNIFORM_MODELVIEWPROJECTION_MATRIX,
    UNIFORM_NORMAL_MATRIX,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum
{
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    NUM_ATTRIBUTES
};

@interface GameViewController () {
    GLKMatrix4 _modelViewProjectionMatrix;
    GLKMatrix3 _normalMatrix;
    float _rotation;
    
    GLuint _vertexArray;
    GLuint _vertexBuffer;
    
@private
    PinballBridgeInterface *_bridgeInterface;
    Physics *_physics;
    Renderer *_renderer;
    Game *_game;
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GameViewController

@synthesize context = _context;
@synthesize effect = _effect;
@synthesize game = game;

- (void)dealloc
{
    delete _game;
    delete _renderer;
    delete _physics;
    delete _bridgeInterface;
    [_context release];
    [_effect release];
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    //self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1] autorelease];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [self setPreferredFramesPerSecond:60];
    
    [self setupGL];
    
    PinballBridgeInterface *bi = new PinballBridgeInterface();
    bi->setGameName([[self.game identifier] UTF8String]);
    //bi->init();
    
    Playfield *f = new Playfield();
    f->setBridgeInterface(bi);
    f->init();
    
    Physics *p = new Physics();
    p->setBridgeInterface(bi);
    p->setPlayfield(f);
    p->init();
    
    Camera *c = new Camera();
    c->setBridgeInterface(bi);
    c->setPlayfield(f);
    c->init();
    
    Editor *e = new Editor();
    e->setBridgeInterface(bi);
    e->setPlayfield(f);
    e->setCamera(c);
    e->setPhysics(p);
    e->init();
    
    Renderer *r = new Renderer();
    r->setBridgeInterface(bi);
    r->setPlayfield(f);
    r->setPhysics(p);
    r->setCamera(c);
    r->setEditor(e);
    r->setDebug(YES);
    r->init();
    
    Game *g = new Game();
    g->setBridgeInterface(bi);
    g->setPhysics(p);
    g->setRenderer(r);
    g->init();
    
    _bridgeInterface = bi;
    _physics = p;
    _renderer = r;
    _game = g;
    
}

- (void)viewDidUnload
{    
    [super viewDidUnload];
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
}

-(void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    [self.navigationController setNavigationBarHidden:YES animated:animated];
}

-(void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    [self.navigationController setNavigationBarHidden:NO animated:animated];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    glDeleteBuffers(1, &_vertexBuffer);
    glDeleteVertexArraysOES(1, &_vertexArray);
    
    self.effect = nil;
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    _physics->updatePhysics();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    _renderer->draw();
}

#pragma mark UI Callbacks

-(IBAction)userDidTapStartButton:(id)sender {
    // TODO: use dynamically set startButton id;
    _game->closeSwitch(0);
}

-(IBAction)userDidTapMenuButton:(id)sender {
    [self.navigationController popViewControllerAnimated:YES];
}

-(IBAction)userDidTapLeftFlipperButton:(id)sender {
    _game->switchClosed("lbutton", NULL);
}

-(IBAction)userDidReleaseLeftFlipperButton:(id)sender {
    _game->switchOpened("lbutton", NULL);
}

-(IBAction)userDidTapRightFlipperButton:(id)sender {
    _game->switchClosed("rbutton", NULL);
}

-(IBAction)userDidReleaseRightFlipperButton:(id)sender {
    _game->switchOpened("rbutton", NULL);
}

@end
