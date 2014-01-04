//
//  ExportLib.m
//  ExportLib
//
//  Created by Yurii Zakipnyi on 1/3/14.
//  Copyright (c) 2014 Plawius. All rights reserved.
//

#include "ExportStaticLib.h"

#include <iostream>
#include "WDLayersHierarchy.h"

void ExportLib::init(const std::string &dirname)
{
    std::cout << "HELLO" << std::endl;
    
    WDLayerHierarchyManager manager;
    
    WDLayerHierarchy *hierarchy = manager.loadFromXML(dirname + "/struct.xml");
    
    
}
