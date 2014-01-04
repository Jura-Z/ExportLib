#include "WDLayersHierarchy.h"

#include <functional>

#include "tinyxml.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------
bool WDLayerStruct::hasEnding (const std::string &fullString, const std::string &ending) const
{
	if (fullString.length() < ending.length()) return false;
	return fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

WDLayerStruct *WDLayerHierarchy::layerById(const std::string &idLayer)
{
    for (auto layer : vectorLayers)
        if (layer->getUniqPathName() == idLayer)
            return layer;
    return nullptr;
}

WDLayerHierarchy *WDLayerHierarchy::extractSubHierarchy(WDLayerStruct *layer)
{
    WDLayerHierarchy *result = new WDLayerHierarchy();
    
    result->vectorTopLayers.push_back(layer);
    
    std::function<void(WDLayerStruct *)> recurcivelyAdd;
    recurcivelyAdd = [&result, &recurcivelyAdd](WDLayerStruct *layer)
    {
        result->vectorLayers.push_back(layer);
        
        if (layer->isGroup())
        {
            for(auto l : layer->children)
                recurcivelyAdd(l);
            result->vectorLayers.push_back(WDLayerStruct::finishGroup());
        }
    };
    
    recurcivelyAdd(layer);
    
    return result;
}

WDLayerHierarchy *WDLayerHierarchy::extractSubHierarchy(const std::string &idLayer)
{
    return extractSubHierarchy(layerById(idLayer));
}

// -----------------------------------------------------------------------------------------------------------
//std::string WDLayerHierarchy::genUniqName(const std::string &layerName)
//{
//	std::string result = layerName;
//	int indx = 1;
//	for (auto it = layerNameMap.find(result); it != layerNameMap.end(); it = layerNameMap.find(result))
//	{
//		indx++;
//		result = layerName.substr(0, layerName.size() - 1) + '(' + std::to_string(indx) + ')' + layerName.back();
//	}
//	return result;
//}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------
std::string createShift(int shift)
{
	std::string res = "";
	for (int j = 1; j <= shift; ++j)
		res += '\t';
	return res;
}

// -----------------------------------------------------------------------------------------------------------
void WDLayerHierarchyManager::saveToXML(const WDLayerHierarchy &hierarchy, std::ostream &stream)
{
	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	
	std::function<void(WDLayerStruct *, int)> recurcivelyPrint = [&stream, &recurcivelyPrint](WDLayerStruct *layer, int ident)
    {
		if (layer->visible == false) return;

		if (layer->isContent())
		{
			bool uselessContent = !layer->hasAnyEnding() && layer->fileNamePNG.empty();
			if (uselessContent) return;
		}

		if (layer->isGroup())
		{
			bool isButtonGroup = layer->hasActiveParent();
			bool isEmptyGroup = !layer->hasAnyEndingAtChildren();
			if (!isButtonGroup && isEmptyGroup) return;
		}

		auto identStr = createShift(ident);
		stream << identStr << layer->toXML() << std::endl;
		
		if (layer->isGroup())
		{
			for (auto it = layer->children.rbegin(); it != layer->children.rend(); ++it)
				recurcivelyPrint(*it, ident + 1);

			stream << identStr << WDLayerStruct::finishGroup()->toXML() << std::endl;
		}
    };

	for (auto it = hierarchy.vectorTopLayers.rbegin(); it != hierarchy.vectorTopLayers.rend(); ++it)
	{
		recurcivelyPrint(*it, 0);
	}
}

// -----------------------------------------------------------------------------------------------------------
WDLayerHierarchy *WDLayerHierarchyManager::loadFromXML(const std::string &filename)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile()) return nullptr;

    WDLayerHierarchyManager manager;
    
    manager.beginBuilding();
    
	std::function<WDLayerStruct *(TiXmlElement *)> parseLayerElement = [](TiXmlElement *element) -> WDLayerStruct*
	{
		auto layer = new WDLayerStruct();
        
        std::string value = element->Value();
        
        layer->grMarker = WDGroupMarker::error;
        if (value == xmlLayerGroup)
            layer->grMarker = WDGroupMarker::start;
        if (value == xmlLayer)
            layer->grMarker = WDGroupMarker::content;
        
        
        
        const std::string *attName = element->Attribute(xmlLayerName);
        if (attName)
            layer->nameUTF8 = *attName;
        
        const std::string *attHasFile = element->Attribute(xmlLayerHasFile);
        if (attHasFile)
            layer->fileNamePNG = *attHasFile;
        
		const std::string *attIndex = element->Attribute(xmlLayerIndex);
        if (attIndex)
			layer->index = ::atoi(attIndex->c_str());
        
        const std::string *attTop = element->Attribute(xmlLayerTop);
        const std::string *attLeft = element->Attribute(xmlLayerLeft);
        const std::string *attBottom = element->Attribute(xmlLayerBottom);
        const std::string *attRight = element->Attribute(xmlLayerRight);

		const std::string *attAX = element->Attribute(xmlLayerAtlasOffsetX);
        const std::string *attAY = element->Attribute(xmlLayerAtlasOffsetY);
        const std::string *attAW = element->Attribute(xmlLayerAtlasWidth);
        const std::string *attAH = element->Attribute(xmlLayerAtlasHeight);

        if (attTop && attLeft && attBottom && attRight)
        {
            layer->top = ::atof(attTop->c_str());
            layer->left = ::atof(attLeft->c_str());
            layer->bottom = ::atof(attBottom->c_str());
            layer->right = ::atof(attRight->c_str());
        }

        if (attAX && attAY && attAW && attAH)
        {
			layer->atlasOffsetX = ::atof(attAX->c_str());
            layer->atlasOffsetY = ::atof(attAY->c_str());
			layer->atlasWidth = ::atof(attAW->c_str());
			layer->atlasHeight = ::atof(attAH->c_str());
        }

		return layer;
	};
    
	std::function<void(TiXmlElement *)> recurcivelyParse = [&manager, &recurcivelyParse, &parseLayerElement](TiXmlElement *element)
    {
		auto layer = parseLayerElement(element);
		manager.addLayer(layer);

		//for each child
		for(auto child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			recurcivelyParse(child);
		}

		if (layer->grMarker == WDGroupMarker::start)
			manager.addLayer(WDLayerStruct::finishGroup());
    };


	for(auto pRoot = doc.RootElement(); pRoot != nullptr; pRoot = pRoot->NextSiblingElement())
	{
		recurcivelyParse(pRoot);
	}

	return manager.endBuilding();
}


// -----------------------------------------------------------------------------------------------------------
void WDLayerHierarchyManager::beginBuilding()
{
	layerBuildingStack.clear();
	if (building) 
		delete building;
	building = new WDLayerHierarchy();
}

// -----------------------------------------------------------------------------------------------------------
void WDLayerHierarchyManager::addLayer(WDLayerStruct *layer)
{
	if (layer->grMarker == WDGroupMarker::error)
	{
		delete layer;
		return;
	}

	if (layer->grMarker == WDGroupMarker::finish)
	{
		building->finishNodes++;
		if (!layerBuildingStack.empty()) 
			layerBuildingStack.pop_back();
	}
	else
	{
		layer->parent = currentGroup(); 
		if (layer->parent)
			layer->parent->children.push_back(layer);
		else
			building->vectorTopLayers.push_back(layer);

		if (layer->grMarker == WDGroupMarker::start)
		{
			building->startNodes++;
			layerBuildingStack.push_back(layer);
		}

		//layer->nameUTF8 = building->genUniqName(layer->nameUTF8);
		//building->layerNameMap[layer->nameUTF8] = layer;
	}
	building->vectorLayers.push_back(layer);
}

// -----------------------------------------------------------------------------------------------------------
WDLayerHierarchy *WDLayerHierarchyManager::endBuilding()
{
	auto result = building;
	
	if (layerBuildingStack.empty())
	{
		building = nullptr;
		return result;
	}

	errorStream << " layerBuildingStack is not empty";
	return nullptr;
}
