#pragma once

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <iostream>
#include <cmath>
#include <stdint.h>

const std::string save_png = "@";
const std::string save_btn = "_BTN";
const std::string save_edit = "_EDIT";
const std::string save_coord = "!";

const std::string xmlLayerGroup = "LayerGroup";
const std::string xmlLayer = "Layer";
const std::string xmlLayerName = "name";
const std::string xmlLayerHasFile = "hasFile";
const std::string xmlLayerTop = "top";
const std::string xmlLayerLeft = "left";
const std::string xmlLayerBottom = "bottom";
const std::string xmlLayerRight = "right";
const std::string xmlLayerIndex = "index";

const std::string xmlLayerAtlasOffsetX = "atlasOffsetX";
const std::string xmlLayerAtlasOffsetY = "atlasOffsetY";
const std::string xmlLayerAtlasWidth = "atlasWidth";
const std::string xmlLayerAtlasHeight = "atlasHeight";
// -----------------------------------------------------------------------------------------------------------
enum class WDGroupMarker { content = 0, start, finish, error };

// -----------------------------------------------------------------------------------------------------------
class WDLayerStruct
{
	bool hasEnding (const std::string &fullString, const std::string &ending) const;
public:
	static WDLayerStruct *finishGroup()
    {
        static WDLayerStruct *res = new WDLayerStruct();
        res->grMarker = WDGroupMarker::finish;
        return res;
    }

	WDGroupMarker			grMarker;

	std::u16string		nameUTF16;
	std::string			nameUTF8;

	double top;
	double left;
	double bottom;
	double right;

	int atlasWidth;
	int atlasHeight;

	int atlasOffsetX;
	int atlasOffsetY;

	int index;

	bool visible;

	std::string			fileNamePNG;

	WDLayerStruct			*parent;
	std::vector<WDLayerStruct *> children;

	WDLayerStruct() : grMarker(WDGroupMarker::content), nameUTF8(""), top(0), left(0), bottom(0), right(0), index(0), atlasWidth(0), atlasHeight(0), atlasOffsetX(0), atlasOffsetY(0), fileNamePNG(""), parent(nullptr), visible(true)
	{}

	bool hasAnyEnding() const		{ return hasEnding(nameUTF8, save_png) || hasEnding(nameUTF8, save_btn) || hasEnding(nameUTF8, save_edit) || hasEnding(nameUTF8, save_coord); }
	bool hasPngEnding() const		{ return hasEnding(nameUTF8, save_png); }
	bool hasBtnEnding() const		{ return hasEnding(nameUTF8, save_btn); }
	bool hasEditEnding() const		{ return hasEnding(nameUTF8, save_edit); }

	bool isGroup() const			{ return grMarker == WDGroupMarker::start; }
	bool isContent() const			{ return grMarker == WDGroupMarker::content; }

	bool hasActiveParent() const { return parent && (parent->hasBtnEnding() || parent->hasEditEnding()); }
	bool shouldIgnoreInvisible() const { return hasActiveParent(); }

	bool shouldSavePNG() const		
	{ 
		if (!visible) return false;	// not visible - not exporting
		if (grMarker == WDGroupMarker::finish) return false; // just marker, not a layer
		if (hasActiveParent()) return true;	// button/editbox state
		return (grMarker == WDGroupMarker::content || grMarker == WDGroupMarker::start) && hasPngEnding();
	}
	
	bool shouldSaveCoords() const	{ return true; }
	bool shouldSaveAtlas() const { return atlasOffsetX != 0 || atlasOffsetY != 0 || atlasWidth != width() || atlasHeight != height(); }
	bool isCoordsEmpty() const { return top == 0 && bottom == 0 && left == 0 && right == 0; }


	static bool shouldSkip(const WDLayerStruct *layer)		
	{ 
		if (layer->grMarker == WDGroupMarker::error || layer->nameUTF8.empty()) 
			return true;
		if (layer->grMarker == WDGroupMarker::finish)
			return true;
		if (layer->hasActiveParent()) return false;
		if (layer->visible == false)
			return true;
		return !layer->hasAnyEnding();
	}

	bool shouldSkip() const		
	{ 
        return WDLayerStruct::shouldSkip(this);
	}
    
    std::string getUniqPathName() const
    {
        std::string result = nameUTF8;
        for (auto p = this->parent; p != nullptr; p = p->parent)
            result = p->nameUTF8 + "_" + result;
        return result;
    }

	bool hasAnyEndingAtChildren() const
	{
		if (hasAnyEnding()) 
			return true;
		for (auto child : children)
			if (child->hasAnyEndingAtChildren())
				return true;
		return false;
	}

	bool isInVisibleGroup() const
	{
		if (visible == false) return false;
		
		if (parent)
			return parent->isInVisibleGroup();

		return true;
	}
	
	std::string toXML()
	{
		if (grMarker == WDGroupMarker::error) return "";
		
		std::string result = "";
		if (grMarker == WDGroupMarker::finish) return result + "</"+xmlLayerGroup+">";
		if (grMarker == WDGroupMarker::start)		result += "<"+xmlLayerGroup+" "+xmlLayerName+"=\"" + nameUTF8 + "\"";
		if (grMarker == WDGroupMarker::content)	result += "<"+xmlLayer+" "+xmlLayerName+"=\"" + nameUTF8 + "\"";
		
		if (shouldSaveCoords() && isCoordsEmpty() == false)
		{
			result += " "+xmlLayerTop+"=\""		+ std::to_string(top)		+ "\"";
			result += " "+xmlLayerLeft+"=\""	+ std::to_string(left)		+ "\"";
			result += " "+xmlLayerBottom+"=\""	+ std::to_string(bottom)	+ "\"";
			result += " "+xmlLayerRight+"=\""	+ std::to_string(right)		+ "\"";

			if (shouldSaveAtlas())
			{
				result += " "+xmlLayerAtlasOffsetX+"=\""	+ std::to_string(atlasOffsetX)		+ "\"";
				result += " "+xmlLayerAtlasOffsetY+"=\""	+ std::to_string(atlasOffsetY)		+ "\"";
				result += " "+xmlLayerAtlasWidth+"=\""		+ std::to_string(atlasWidth)		+ "\"";
				result += " "+xmlLayerAtlasHeight+"=\""		+ std::to_string(atlasHeight)		+ "\"";
			}
		}
		
        if (shouldSavePNG() && !fileNamePNG.empty())
        {
            result += " "+xmlLayerHasFile+"=\""	+ fileNamePNG + "\"";
        }
        
		result += " "+xmlLayerIndex+"=\""	+ std::to_string(index) + "\"";
        
//		std::string visibleStr = (visible) ? "true" : "false";
//		result += " visible=\""	+ visibleStr + "\"";

		if (grMarker == WDGroupMarker::content) result += "/";
		result += ">";
		return result;
	}

	int width() const { return (int) (fabs(right - left) + 0.5); }
	int height() const { return (int) (fabs(top - bottom) + 0.5); }
};

// -----------------------------------------------------------------------------------------------------------
class WDLayerHierarchy
{
	friend class WDLayerHierarchyManager;

	int startNodes;
	int finishNodes;
	int contentNodes;

public:

	std::vector<WDLayerStruct *> vectorLayers;
	std::vector<WDLayerStruct *> vectorTopLayers;

	WDLayerHierarchy() : startNodes(0), finishNodes(0), contentNodes(0) {}
    WDLayerStruct *layerById(const std::string &idLayer);
    WDLayerHierarchy *extractSubHierarchy(const std::string &idLayer);
    WDLayerHierarchy *extractSubHierarchy(WDLayerStruct *layer);
};

// -----------------------------------------------------------------------------------------------------------
class WDLayerHierarchyManager
{
	std::ostream &errorStream;
	std::deque<WDLayerStruct *> layerBuildingStack;
	WDLayerHierarchy *building;
public:
	WDLayerHierarchyManager(std::ostream &errStream) : errorStream(errStream), building(nullptr) {} 
	WDLayerHierarchyManager() : errorStream(std::cout), building(nullptr) {} 

	WDLayerStruct *currentGroup() { if (layerBuildingStack.empty()) return nullptr; else return layerBuildingStack.back(); }

	void saveToXML(const WDLayerHierarchy &hierarchy, std::ostream &stream);
	WDLayerHierarchy *loadFromXML(const std::string &filename);

	void beginBuilding();
	void addLayer(WDLayerStruct *layer);
	WDLayerHierarchy *endBuilding();
};
