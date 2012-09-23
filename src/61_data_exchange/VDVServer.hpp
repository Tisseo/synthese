  
/** VDVServer class header.
	@file VDVServer.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_cms_VDVClient_hpp__
#define SYNTHESE_cms_VDVClient_hpp__

#include "Object.hpp"

#include "DataSource.h"
#include "StandardFields.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace data_exchange
	{
		class VDVServerSubscription;

		FIELD_TYPE(ServerAddress, std::string)
		FIELD_TYPE(ServerPort, std::string)
		FIELD_TYPE(DataSourcePointer, boost::optional<impex::DataSource&>)
		FIELD_TYPE(ClientControlCentreCode, std::string)
		FIELD_TYPE(ServerControlCentreCode, std::string)
		FIELD_TYPE(ServiceCode, std::string)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(ServerAddress),
			FIELD(ServerPort),
			FIELD(ServerControlCentreCode),
			FIELD(ClientControlCentreCode),
			FIELD(ServiceCode),
			FIELD(DataSourcePointer)
		> VDVServerRecord;



		//////////////////////////////////////////////////////////////////////////
		/// VDV 3rd party server.
		///	@ingroup m36
		/// @author Hugues Romain
		/// @since 3.5.0
		class VDVServer:
			public Object<VDVServer, VDVServerRecord>
		{
		public:
		
			/// Chosen registry class.
			typedef util::Registry<VDVServer>	Registry;

			typedef std::set<VDVServerSubscription*> Subscriptions;

		private:
			Subscriptions _subscriptions;
			boost::posix_time::ptime _startServiceTimeStamp;
			mutable bool _online;

			std::string _getURL( const std::string& request ) const;
			
		public:
			VDVServer(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				void addSubscription(VDVServerSubscription* subscription);
				const Subscriptions& getSubscriptions() const { return _subscriptions; }
				void removeSubscription(VDVServerSubscription* subscription);
				void connect() const;
				bool getOnline() const { return _online; }

				void updateSYNTHESEFromServer() const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_cms_VDVClient_hpp__