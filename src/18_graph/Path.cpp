
/** Path class implementation.
	@file Path.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Path.h"
#include "PathGroup.h"
#include "Edge.h"
#include "Vertex.h"
#include "Service.h"
#include "Exception.h"
#include "Conversion.h"
#include "Hub.h"

#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace geometry;
	using namespace calendar;

	namespace graph
	{

		Path::Path():
			RuleUser(),
			Calendar(),
			_pathGroup(NULL),
			_pathClass(NULL),
			_allDays(false)
		{
		}
		    

		Path::~Path ()
		{
		}



		const RuleUser* Path::_getParentRuleUser() const
		{
			return _pathGroup;
		}


// 		const Service* Path::getService(
// 			int serviceIndex
// 		) const {
// 			ServiceSet::const_iterator it(_services.begin ());
// 			advance (it, serviceIndex);
// 			return (*it);
// 		}




		void Path::addService(
			Service* service,
			bool ensureLineTheory
		){
			if (_services.find(service) != _services.end())
				throw Exception("The service already exists.");

			std::pair<ServiceSet::iterator, bool> result = _services.insert (service);
			if (result.second == false)
			{
				throw Exception(
					"Service number " + Conversion::ToString (service->getServiceNumber ())
					+ " is already defined in path " + Conversion::ToString (getKey())
				);
			}
			markScheduleIndexesUpdateNeeded ();
			service->setPath(this);
		}



		void Path::removeService (Service* service)
		{
			_services.erase(service);

			markScheduleIndexesUpdateNeeded ();
		}




		bool Path::isActive(const date& d) const
		{
			return _allDays || Calendar::isActive(d);
		}



		const Edge* Path::getEdge (int index) const
		{
			return _edges[index];
		}



		std::vector<const Point2D*> 
		Path::getPoints (int fromEdgeIndex,
				int toEdgeIndex) const
		{
			if (toEdgeIndex == -1) toEdgeIndex = _edges.size () - 1;
			std::vector<const Point2D*> points;
		    
			for (int i=fromEdgeIndex; i<=toEdgeIndex; ++i)
			{
			points.push_back (_edges[i]->getFromVertex ());
			
			// Adds all the via points of the line stop
			const std::vector<const Point2D*>& viaPoints = _edges[i]->getViaPoints ();
			for (std::vector<const Point2D*>::const_iterator it = viaPoints.begin (); 
				it != viaPoints.end (); 
				++it)
			{
				points.push_back (*it);
			}
			}
			return points;
		}



		void Path::addEdge(
			Edge* edge,
			optional<double> autoShift
		){
			// Empty path : just put the edge in the vector

			if (_edges.empty())
			{
				_edges.push_back(edge);
				return;
			}

			// Non empty path : determinate the good position of the edge
			Edges::iterator insertionPosition;
			for(insertionPosition = _edges.begin();
				insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() < edge->getRankInPath();
				++insertionPosition) ;

			// If an edge with the same rank exists, then throw an exception or shift all ranks after it, depending on the autoshift parameter
			if(insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() == edge->getRankInPath())
			{
				// If the edge is the same, do nothing
				if((*insertionPosition) == edge) return;

				if(!autoShift) throw Exception("An edge with the rank "+ lexical_cast<string>(edge->getRankInPath()) + " already exists in the path " + lexical_cast<string>(getKey()));

				for(Edges::iterator it(insertionPosition); it != _edges.end(); ++it)
				{
					(*it)->setRankInPath((*it)->getRankInPath() + 1);
					if(insertionPosition == _edges.begin())
					{
						(*it)->setMetricOffset((*it)->getMetricOffset() + *autoShift);
					}
				}
			}

			// Next arrival and next in path of the previous
			if(insertionPosition != _edges.begin())
			{
				Edge* previousEdge(*(insertionPosition - 1));
				Edge* nextArrival(previousEdge->getFollowingArrivalForFineSteppingOnly());
				Edge* nextConnectingArrival(previousEdge->getFollowingConnectionArrival());

				// Next arrival of previous edges
				if(edge->isArrivalAllowed())
				{
					for(Edges::iterator it(insertionPosition-1); 
						(*it)->getFollowingArrivalForFineSteppingOnly() == nextArrival;
						--it
					){
						(*it)->setFollowingArrivalForFineSteppingOnly(edge);
						if (it == _edges.begin()) break;
					}

					if(edge->isConnectingEdge())
					{
						for(Edges::iterator it(insertionPosition-1); 
							(*it)->getFollowingConnectionArrival() == nextConnectingArrival;
							--it
						){
							(*it)->setFollowingConnectionArrival(edge);
							if (it == _edges.begin()) break;
						}
					}
				}

				edge->setFollowingArrivalForFineSteppingOnly(nextArrival);
				edge->setFollowingConnectionArrival(nextConnectingArrival);
			}
			else
			{
				// First edge : next arrival is the old first one or its next arrival
				Edge* firstEdge(*insertionPosition);
				if(firstEdge->isArrivalAllowed())
				{
					edge->setFollowingArrivalForFineSteppingOnly(firstEdge);
				}
				else
				{
					edge->setFollowingArrivalForFineSteppingOnly(firstEdge->getFollowingArrivalForFineSteppingOnly());
				}
				if(firstEdge->isArrivalAllowed() && firstEdge->isConnectingEdge())
				{
					edge->setFollowingConnectionArrival(firstEdge);
				}
				else
				{
					edge->setFollowingConnectionArrival(firstEdge->getFollowingConnectionArrival());
				}
			}
		
			// Previous departure
			if(insertionPosition != _edges.end())
			{
				Edge* nextEdge(*insertionPosition);
				Edge* previousDeparture(nextEdge->getPreviousDepartureForFineSteppingOnly());
				Edge* previousConnectingDeparture(nextEdge->getPreviousConnectionDeparture());

				// Previous departure of next edges
				if(edge->isDepartureAllowed())
				{
					for(Edges::iterator it(insertionPosition); 
						it != _edges.end() && (*it)->getPreviousDepartureForFineSteppingOnly() == previousDeparture;
						++it
					){
						(*it)->setPreviousDepartureForFineSteppingOnly(edge);
					}

					if(edge->isConnectingEdge())
					{
						for(Edges::iterator it(insertionPosition); 
							it != _edges.end() && (*it)->getPreviousConnectionDeparture() == previousConnectingDeparture;
							++it
						){
							(*it)->setPreviousConnectionDeparture(edge);
						}
					}
				}

				edge->setPreviousDepartureForFineSteppingOnly(previousDeparture);
				edge->setPreviousConnectionDeparture(previousConnectingDeparture);
			}
			else
			{
				// Last edge : previous departure is the old last one or its previous departure
				Edge* lastEdge(*(insertionPosition - 1));
				if(lastEdge->isDepartureAllowed())
				{
					edge->setPreviousDepartureForFineSteppingOnly(lastEdge);
				}
				else
				{
					edge->setPreviousDepartureForFineSteppingOnly(lastEdge->getPreviousDepartureForFineSteppingOnly());
				}
				if(lastEdge->isDepartureAllowed() && lastEdge->isConnectingEdge())
				{
					edge->setPreviousConnectionDeparture(lastEdge);
				}
				else
				{
					edge->setPreviousConnectionDeparture(lastEdge->getPreviousConnectionDeparture());
				}
			}

			// Insertion of the new edges
			_edges.insert(insertionPosition, edge);
		}


		Edge* Path::getLastEdge() const
		{
			if(_edges.empty()) return NULL;
			return *(_edges.end() - 1);
		}



	    void 
	    Path::markScheduleIndexesUpdateNeeded()
	    {
		for (Edges::const_iterator it = _edges.begin(); it != _edges.end(); ++it)
		    (*it)->markServiceIndexUpdateNeeded (false);
	    }



		void Path::merge(Path& other )
		{
			if(	other._pathGroup != _pathGroup ||
				other._edges.empty() ||
				_edges.empty() ||
				other.getEdge(0)->getFromVertex() != getLastEdge()->getFromVertex()
			){
				throw util::Exception("The two roads cannot be merged");
			}

			Edge* lastEdge(getLastEdge());
			Vertex* vertex(const_cast<Vertex*>(lastEdge->getFromVertex()));
			double metricOffset(lastEdge->getMetricOffset());
			int rankInPath(lastEdge->getRankInPath());

			_edges.pop_back();
			vertex->removeArrivalEdge(lastEdge);
			other._edges[0]->setPreviousConnectionDeparture(lastEdge->getPreviousConnectionDeparture());
			other._edges[0]->setPreviousDepartureForFineSteppingOnly(lastEdge->getPreviousDepartureForFineSteppingOnly());

			BOOST_FOREACH(Edge* edge, _edges)
			{
				if(edge->getFollowingConnectionArrival() == lastEdge)
				{
					edge->setFollowingConnectionArrival(other._edges[0]);
				}
				if(edge->getFollowingArrivalForFineSteppingOnly() == lastEdge)
				{
					edge->setFollowingArrivalForFineSteppingOnly(other._edges[0]);
				}
			}

			BOOST_FOREACH(Edge* edge, other._edges)
			{
				Vertex* vertex(const_cast<Vertex*>(edge->getFromVertex()));
				if(edge->isArrivalAllowed())
				{
					vertex->removeArrivalEdge(edge);
				}
				if(edge->isDepartureAllowed())
				{
					vertex->removeDepartureEdge(edge);
				}
				edge->setRankInPath(edge->getRankInPath()+rankInPath);
				edge->setMetricOffset(edge->getMetricOffset()+metricOffset);
				_edges.push_back(edge);
				edge->setParentPath(this);
				if(edge->isArrivalAllowed())
				{
					vertex->addArrivalEdge(edge);
				}
				if(edge->isDepartureAllowed())
				{
					vertex->addDepartureEdge(edge);
				}
			}

			other._edges.clear();

			_pathGroup->removePath(&other);
		}



		bool Path::isRoad() const
		{
			return false;
		}



		bool Path::sameContent( const Path& other, bool considerVertices, bool considerDepartureArrival) const
		{
			if(_edges.size() != other._edges.size())
			{
				return false;
			}

			Edges::const_iterator otherEdgeIt(other._edges.begin());
			BOOST_FOREACH(const Edge* edge, _edges)
			{
				const Edge& otherEdge(**otherEdgeIt);

				if(	(considerVertices && edge->getFromVertex() != otherEdge.getFromVertex()) ||
					edge->getFromVertex()->getHub() != otherEdge.getFromVertex()->getHub() ||
					(considerDepartureArrival && edge->isArrival() != otherEdge.isArrival()) ||
					(considerDepartureArrival && edge->isDeparture() != otherEdge.isDeparture())
				){
					return false;
				}

				++otherEdgeIt;
			}

			return true;
		}



		bool Path::includes( const Path& other, bool considerVertices ) const
		{
			Edges::const_iterator edgeIt(_edges.begin());
			BOOST_FOREACH(const Edge* edge, other._edges)
			{
				for(;
					edgeIt != _edges.end() &&
					(
						(considerVertices && edge->getFromVertex() != (*edgeIt)->getFromVertex()) ||
						edge->getFromVertex()->getHub() != (*edgeIt)->getFromVertex()->getHub() ||
						edge->isArrival() && !(*edgeIt)->isArrival() ||
						edge->isDeparture() && !(*edgeIt)->isDeparture()
					);
					++edgeIt) ;
				if(edgeIt == _edges.end())
				{
					return false;
				}
			}
			return true;
		}


		bool cmpService::operator ()(const Service *s1, const Service *s2) const
		{
			return (s1->getDepartureSchedule (false,0) < s2->getDepartureSchedule (false,0))
				|| (s1->getDepartureSchedule (false,0) == s2->getDepartureSchedule (false,0)
				&& s1 < s2)						
				;
		}
	}
}
