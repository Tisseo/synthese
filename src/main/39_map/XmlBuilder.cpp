#include "XmlBuilder.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"
#include "01_util/Log.h"

#include "DrawableLine.h"
#include "Map.h"
#include "MapBackgroundManager.h"


using namespace synthese::util::XmlToolkit;

using namespace synthese::util;



namespace synthese
{
namespace map
{


DrawableLine* 
XmlBuilder::CreateDrawableLine (XMLNode& node, 
				const synthese::env::Line::Registry& lines)
{
    // assert ("drawableLine" == node.getName ());
    
    uid lineId (GetLongLongAttr (node, "lineId"));
    
    const synthese::env::Line* line = lines.get (lineId);
    const std::vector<synthese::env::Edge*>& lineStops = line->getEdges ();
    
    assert (lineStops.size () >= 2);

    int fromLineStopIndex (GetIntAttr (node, "fromLineStopId", 0));
    int toLineStopIndex (GetIntAttr (node, "toLineStopId", (int) lineStops.size () - 1));

    bool withPhysicalStops (GetBoolAttr (node, "withPhysicalStops", false));

    return new DrawableLine (line, 
			     fromLineStopIndex, 
			     toLineStopIndex,
			     withPhysicalStops);
}




Map* 
XmlBuilder::CreateMap (XMLNode& node, const synthese::env::Line::Registry& lines)
{
    // assert ("map" == node.getName ());
    
    
    int outputWidth (GetIntAttr (node, "outputWidth", -1));
    int outputHeight (GetIntAttr (node, "outputHeight", -1));

    // Drawable lines
    std::set<DrawableLine*> selectedLines;
    int nbDrawableLines = node.nChildNode ("drawableLine");
    for (int i=0; i<nbDrawableLines; ++i) 
    {
	XMLNode drawableLineNode = node.getChildNode ("drawableLine", i);
	selectedLines.insert (CreateDrawableLine (drawableLineNode, lines));
    }

    const MapBackgroundManager* mbm = 0;

    std::string backgroundId (GetStringAttr (node, "backgroundId", ""));
    if (backgroundId != "")
    {
	try 
	{
	    mbm = MapBackgroundManager::GetMapBackgroundManager (backgroundId);
	}
	catch (synthese::util::Exception& ex)
	{
	    Log::GetInstance ().warn ("Cannot find background", ex);
	}
    }

    std::string urlPattern (GetStringAttr (node, "urlPattern", ""));

    Map* map = 0;

    bool preserveRatio (GetBoolAttr (node, "preserveRatio", true));

    double neighborhood (GetDoubleAttr (node, "neighborhood", 0.0));

    // If one of the 4 coordinates is missing, let the autofit 
    // feature process the right rectangle
    if ( 
	(HasAttr (node, "lowerLeftLatitude") == false) ||
	(HasAttr (node, "lowerLeftLongitude") == false) ||
	(HasAttr (node, "upperRightLatitude") == false) ||
	(HasAttr (node, "upperRightLongitude") == false) 
	)
    {
	map = new Map (selectedLines,
		       outputWidth, 
		       outputHeight, 
		       neighborhood,
		       preserveRatio,
		       mbm, urlPattern); 
	
    }
    else 
    {
	double lowerLeftLatitude (GetDoubleAttr (node, "lowerLeftLatitude"));
	double lowerLeftLongitude (GetDoubleAttr (node, "lowerLeftLongitude"));
	double upperRightLatitude (GetDoubleAttr (node, "upperRightLatitude"));
	double upperRightLongitude (GetDoubleAttr (node, "upperRightLongitude"));

        map = new Map (selectedLines,
		       Rectangle (lowerLeftLatitude,
				  lowerLeftLongitude,
				  upperRightLatitude - lowerLeftLatitude,
				  upperRightLongitude - lowerLeftLongitude),
		       outputWidth, 
		       outputHeight, 
		       preserveRatio,
		       mbm, urlPattern); 
	

    }


    bool lineGrouping (GetBoolAttr (node, "lineGrouping", true));
    if (lineGrouping) map->setLineGrouping (lineGrouping);

    return map;     
 }








}
}


