//
//  WDExportCocos.h
//  frost
//
//  Created by Yurii Zakipnyi on 8/12/13.
//
//

#pragma once

#include <string>

class WDLayerHierarchy;

class WDExportCocos
{
    std::string folderPath;
    WDLayerHierarchy *hierarchy;
public:
    WDExportCocos(std::string wdExportFolder);
    ~WDExportCocos();
    
    CCLabelTTF *labelFrom(std::string layerName, std::string fontName, float fontSize);
    CCSprite *spriteFrom(std::string layerName);
    CCSprite *spriteFrom(std::string layerName, std::string imageFromLayerName);

};