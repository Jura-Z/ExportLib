//
//  WDExportCocos.cpp
//  frost
//
//  Created by Yurii Zakipnyi on 8/12/13.
//
//

#include "WDExportCocos.h"
#include "WDLayersHierarchy.h"

WDExportCocos::WDExportCocos(std::string wdExportFolder) : folderPath(wdExportFolder)
{
    WDLayerHierarchyManager manager;
    hierarchy = manager.loadFromXML(folderPath + "/struct.xml");
    if (hierarchy == nullptr)
    {
        std::cout << "Folder not found: " << wdExportFolder << std::endl;
    }
}

WDExportCocos::~WDExportCocos()
{
    delete hierarchy;
}

cocos2d::CCLabelTTF *WDExportCocos::labelFrom(std::string layerName, std::string fontName, float fontSize)
{
    WDLayerStruct *layer = hierarchy->layerById(layerName);
    
    CCLabelTTF *label = cocos2d::CCLabelTTF::create(layerName.c_str(), fontName.c_str(), fontSize);

    label->setAnchorPoint(ccp(0, 0));
    label->setPosition(ccp(layer->left * 0.5, layer->top * 0.5));
    
    return label;
}

cocos2d::CCSprite *WDExportCocos::spriteFrom(std::string layerName, std::string imageFromLayerName)
{
    WDLayerStruct *layer2 = hierarchy->layerById(layerName);
    WDLayerStruct *layer = hierarchy->layerById(imageFromLayerName);
    
    CCRect rect(layer->atlasOffsetX * 0.5, layer->atlasOffsetY * 0.5, layer->width() * 0.5, layer->height() * 0.5);
    cocos2d::CCSprite *spr = cocos2d::CCSprite::create((folderPath + "/" + layer->fileNamePNG).c_str(), rect);
    
    spr->setAnchorPoint(ccp(0, 0));
    spr->setPosition(ccp(layer2->left * 0.5, layer2->top * 0.5));
    
    return spr;
}

cocos2d::CCSprite *WDExportCocos::spriteFrom(std::string layerName)
{
    WDLayerStruct *layer = hierarchy->layerById(layerName);
    
    CCRect rect(layer->atlasOffsetX * 0.5, layer->atlasOffsetY * 0.5, layer->width() * 0.5, layer->height() * 0.5);
    cocos2d::CCSprite *spr = cocos2d::CCSprite::create((folderPath + "/" + layer->fileNamePNG).c_str(), rect);
    
    spr->setAnchorPoint(ccp(0, 0));
    spr->setPosition(ccp(layer->left * 0.5, layer->top * 0.5));
    
    return spr;
}
