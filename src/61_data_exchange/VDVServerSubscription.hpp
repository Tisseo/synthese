  
/** VDVServerSubscription class header.
	@file VDVServerSubscription.hpp

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

#ifndef SYNTHESE_cms_VDVServerSubscription_hpp__
#define SYNTHESE_cms_VDVServerSubscription_hpp__

#include "Object.hpp"

#include "StandardFields.hpp"
#include "StopArea.hpp"
#include "VDVServer.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace data_exchange
	{
		FIELD_TYPE(StopAreaPointer, boost::optional<pt::StopArea&>)
		FIELD_TYPE(SubscriptionDuration, boost::posix_time::time_duration)
		FIELD_TYPE(TimeSpan, boost::posix_time::time_duration)
			
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(VDVServer),
			FIELD(StopAreaPointer),
			FIELD(SubscriptionDuration),
			FIELD(TimeSpan)
		> VDVServerSubscriptionRecord;



		//////////////////////////////////////////////////////////////////////////
		/// Subscription to a 3rd party VDV server.
		///	@ingroup m36
		/// @author Hugues Romain
		/// @since 3.5.0
		class VDVServerSubscription:
			public Object<VDVServerSubscription, VDVServerSubscriptionRecord>
		{
		public:
		
			/// Chosen registry class.
			typedef util::Registry<VDVServerSubscription>	Registry;


		private:
		
		public:
			VDVServerSubscription(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_cms_VDVClientItem_hpp__
