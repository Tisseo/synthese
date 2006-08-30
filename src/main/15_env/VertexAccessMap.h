#ifndef SYNTHESE_ENV_VERTEXACCESSMAP_H
#define SYNTHESE_ENV_VERTEXACCESSMAP_H


#include "module.h"

#include "Point.h"


#include <vector>
#include <map>



namespace synthese
{
namespace env
{


 class Vertex;
 class Line;


 typedef std::vector<const Vertex*> AccessPath;
 typedef enum { FROM_ORIGIN, TO_DESTINATION } AccessDirection ;
 

 typedef struct {
     double maxApproachDistance;
     double maxApproachTime;
     double approachSpeed;
     
 } AccessParameters;
 
 
 typedef struct {
     AccessPath path;
     double approachTime;
     double approachDistance;
 } VertexAccess;
 


/** 


 @ingroup m15
*/
class VertexAccessMap
{
public:


private:

    std::map<const Vertex*, VertexAccess> _map;
    std::map<const Line*, bool> _nonLineConnectableDepartureVertex;
    std::map<const Line*, bool> _nonLineConnectableArrivalVertex;
    
    Point _isobarycenter;   //!< Isobarycenter of all points contained in this map.
    double _isobarycenterMaxDistance;   //!< Maximum distance of one map point with the isobarycenter.



// une fonction qui verifie pour une ligne donn�e si elle passe par l'un des vertex
// de la vam qui n'est pas de correspondance transport.
// VertexAccessMap vam, 

 public:

    VertexAccessMap ();
    ~VertexAccessMap ();


    const std::map<const Vertex*, VertexAccess>& getMap () const;

    bool hasNonLineConnectableDepartureVertex (const Line* line) const;
    bool hasNonLineConnectableArrivalVertex (const Line* line) const;
    
    void insert (const Vertex* vertex, const VertexAccess& vertexAccess);
    
    // TODO : rajouter a l'init le calcul de l'isobarycentre et la distance max de cet iso
    // avec chacun des vertex.

 private:

    void updateNonLineConnectableVertexMap ();
    void updateIsobarycenter ();
    void updateIsobarycenterMaxDistance ();


};


}
}

#endif 	    




