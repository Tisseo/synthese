
/** AddressablePlace class implementation.
	@file AddressablePlace.cpp

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

#include "AddressablePlace.h"
#include "Address.h"
#include "VertexAccessMap.h"
#include "Exception.h"
#include "RoadModule.h"

#include <boost/foreach.hpp>
#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace geometry;
	using namespace util;
	using namespace graph;
	using namespace env;
	using namespace time;

	namespace road
	{
		AddressablePlace::AddressablePlace(
			bool allowedConnection/*= CONNECTION_TYPE_FORBIDDEN */,
			boost::posix_time::time_duration defaultTransferDelay /*= FORBIDDEN_TRANSFER_DELAY  */
		):	Place(),
			_allowedConnection(allowedConnection),
			_defaultTransferDelay(defaultTransferDelay)
		{
		}


		boost::posix_time::time_duration AddressablePlace::getDefaultTransferDelay(
			) const {
				return _defaultTransferDelay;
		}

		boost::posix_time::time_duration AddressablePlace::getMinTransferDelay() const
		{
			if (_minTransferDelay.is_not_a_date_time())
			{
				_minTransferDelay = _defaultTransferDelay;
				for (TransferDelaysMap::const_iterator it(_transferDelays.begin()); it != _transferDelays.end(); ++it)
					if (it->second < _minTransferDelay)
						_minTransferDelay = it->second;
			}
			return _minTransferDelay;
		}



		void AddressablePlace::setDefaultTransferDelay(
			boost::posix_time::time_duration defaultTransferDelay
		){
			assert(defaultTransferDelay >= posix_time::minutes(0) && !defaultTransferDelay.is_not_a_date_time());

			_defaultTransferDelay = defaultTransferDelay;
			_minTransferDelay = boost::posix_time::time_duration();
		}

		void AddressablePlace::setAllowedConnection(
			bool value
			){
				_allowedConnection = value;
		}



		void AddressablePlace::addTransferDelay(
			AddressablePlace::TransferDelaysMap::key_type::first_type fromVertex,
			AddressablePlace::TransferDelaysMap::key_type::second_type toVertex,
			posix_time::time_duration transferDelay
		){
			assert(transferDelay >= posix_time::minutes(0) && !transferDelay.is_not_a_date_time());

			_transferDelays[std::make_pair (fromVertex, toVertex)] = transferDelay;
			_minTransferDelay = posix_time::time_duration();
		}


		void AddressablePlace::addForbiddenTransferDelay(
			AddressablePlace::TransferDelaysMap::key_type::first_type fromVertex,
			AddressablePlace::TransferDelaysMap::key_type::second_type toVertex
		){
			_transferDelays[std::make_pair (fromVertex, toVertex)] = posix_time::time_duration();
			_minTransferDelay = posix_time::time_duration();
		}



		void AddressablePlace::clearTransferDelays()
		{
			_transferDelays.clear ();
			_defaultTransferDelay = posix_time::time_duration();
			_minTransferDelay = posix_time::time_duration();
		}



		bool AddressablePlace::isConnectionAllowed(
			const Vertex& fromVertex,
			const Vertex& toVertex
		) const {
			return getTransferDelay(fromVertex, toVertex) != posix_time::time_duration();
		}



		boost::posix_time::time_duration AddressablePlace::getTransferDelay(
			const Vertex& fromVertex,
			const Vertex& toVertex
		) const {
			TransferDelaysMap::const_iterator it(
				_transferDelays.find(make_pair(fromVertex.getKey(), toVertex.getKey()))
			);

			// If not defined in map, return default transfer delay
			return (it == _transferDelays.end ()) ? _defaultTransferDelay : it->second;
		}




		AddressablePlace::~AddressablePlace ()
		{

		}



		const AddressablePlace::Addresses& AddressablePlace::getAddresses () const
		{
			return _addresses;
		}



		void AddressablePlace::addAddress (const Address* address)
		{
			_isoBarycentreToUpdate = true;
			_addresses.push_back (address);
		}



		void AddressablePlace::getVertexAccessMap(
			VertexAccessMap& result, 
			const AccessDirection& accessDirection,
			GraphIdType whatToSearch,
			const Vertex& origin
		) const {
			if (whatToSearch != RoadModule::GRAPH_ID) return;

			if(accessDirection == DEPARTURE_TO_ARRIVAL)
			{
				BOOST_FOREACH(const Address* address, _addresses)
				{
					if(!isConnectionAllowed(origin, *address)) continue;

					result.insert(
						address,
						VertexAccess(getTransferDelay(origin, *address))
					);
				}
			} else {
				BOOST_FOREACH(const Address* address, _addresses)
				{
					if(!isConnectionAllowed(*address, origin)) continue;

					result.insert(
						address,
						VertexAccess(getTransferDelay(*address, origin))
					);
				}
			}
		}



		void AddressablePlace::getVertexAccessMap(
			VertexAccessMap& result, 
			const AccessDirection& accessDirection,
			const AccessParameters& accessParameters,
			const geography::Place::GraphTypes& whatToSearch
		) const {
			if (whatToSearch.find(RoadModule::GRAPH_ID) == whatToSearch.end()) return;
			
			BOOST_FOREACH(const Address* address, _addresses)
			{
				result.insert(
					address,
					VertexAccess()
				);
		}	}


		const geometry::Point2D& AddressablePlace::getPoint() const
		{
			if (_isoBarycentreToUpdate)
			{
				_isoBarycentre.clear();
				for (Addresses::const_iterator it(_addresses.begin()); it != _addresses.end(); ++it)
					_isoBarycentre.add(**it);
				_isoBarycentreToUpdate = false;
			}
			return _isoBarycentre;
		}



		const AddressablePlace* AddressablePlace::GetPlace(const graph::Hub* hub)
		{
			const AddressablePlace* place(dynamic_cast<const AddressablePlace*>(hub));
			if(place == NULL) throw util::Exception("bad conversion");
			return place;
		}



		bool AddressablePlace::containsAnyVertex( graph::GraphIdType graphType ) const
		{
			if(graphType == RoadModule::GRAPH_ID)
			{
				return !_addresses.empty();
			}
			return false;
		}



		graph::HubScore AddressablePlace::getScore() const
		{
			return MIN_HUB_SCORE;
		}



		bool AddressablePlace::getAllowedConnection() const
		{
			return _allowedConnection;
		}

		const AddressablePlace::TransferDelaysMap& AddressablePlace::getTransferDelays() const
		{
			return _transferDelays;
		}
	}
}
