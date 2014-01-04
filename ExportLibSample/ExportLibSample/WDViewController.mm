//
//  WDViewController.m
//  ExportLibSample
//
//  Created by Yurii Zakipnyi on 1/4/14.
//  Copyright (c) 2014 Plawius. All rights reserved.
//

#import "WDViewController.h"
#import "ExportStaticLib/ExportStaticLib.h"

@interface WDViewController ()

@end

@implementation WDViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    ExportLib lib;
    lib.init("wda");
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
