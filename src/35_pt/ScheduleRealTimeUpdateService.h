
/** ScheduleRealTimeUpdateService class header.
	@file ScheduleRealTimeUpdateService.h
	@author Hugues Romain
	@date 2009

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

#ifndef SYNTHESE_ScheduleRealTimeUpdateService_H__
#define SYNTHESE_ScheduleRealTimeUpdateService_H__

#include "FactorableTemplate.h"
#include "Function.h"
#include "ServicePointer.h"
#include "SchedulesBasedService.h"
#include "StopArea.hpp"
#include <boost/date_time/time_duration.hpp>
#include <boost/weak_ptr.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace pt
	{
		class ScheduledService;

		//////////////////////////////////////////////////////////////////////////
		///	Action class : real time update of service schedules.
		///	See https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_scheduled_service
		///
		/// In progress development to create an advanced version of ScheduleRealTimeUpdateAction
		/// Once complete, it will either be merger in ScheduleRealTimeUpdateAction or renamed to
		/// a relevant name.
		/// This action reschedules a service based on a given Arrival or Departure time.
		/// If the next services end up being before the current one, they are rescheduled as
		/// well.
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35Actions refActions
		class ScheduleRealTimeUpdateService:
			public util::FactorableTemplate<server::Function,ScheduleRealTimeUpdateService>
		{
		public:

			static const std::string PARAMETER_STOP_AREA_ID;
			static const std::string PARAMETER_STOP_AREA_DATASOURCE_ID;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_SERVICE_DATASOURCE_ID;
			static const std::string PARAMETER_DEPARTURE_TIME;
			static const std::string PARAMETER_ARRIVAL_TIME;


		private:
			struct Record
			{
				boost::shared_ptr<ScheduledService> service;
				boost::posix_time::time_duration arrivalTime;
				boost::posix_time::time_duration departureTime;
				std::size_t lineStopRank;
			};

			boost::shared_ptr<StopArea> _stopArea;
			std::vector<Record> _records;

			size_t _getStopRankByService(const SchedulesBasedService *service,
				 const boost::shared_ptr<StopArea> &stopArea) const;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	See https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_scheduled_service#Request
			//////////////////////////////////////////////////////////////////////////
			/// @return Generated parameters map
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	See https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_scheduled_service#Request
			//////////////////////////////////////////////////////////////////////////
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			virtual void _setFromParametersMap(const util::ParametersMap& map);

		public:
			ScheduleRealTimeUpdateService();

			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2011
			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// @todo Implement a right control on this function
			virtual bool isAuthorized(const server::Session* session) const;

			virtual bool _isSessionRequired() const { return false; }

			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2011
			virtual std::string getOutputMimeType() const;

		};
	}
}

#endif // SYNTHESE_ScheduleRealTimeUpdateService_H__
