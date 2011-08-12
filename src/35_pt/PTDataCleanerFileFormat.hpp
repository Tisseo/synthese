
/** PTDataCleanerFileFormat class header.
	@file PTDataCleanerFileFormat.hpp

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

#ifndef SYNTHESE_pt_PTDataCleanerFileFormat_hpp__
#define SYNTHESE_pt_PTDataCleanerFileFormat_hpp__

#include "Importer.hpp"
#include "Calendar.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class JourneyPattern;
		class ContinuousService;
		class ScheduledService;
		class DesignatedLinePhysicalStop;
		class StopPoint;
		class StopArea;
		class LineStop;

		/** PTDataCleanerFileFormat class.
			@ingroup m35
		*/
		class PTDataCleanerFileFormat:
			public virtual impex::Importer
		{
		public:
			static const std::string PARAMETER_CALENDAR_ID;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_FROM_TODAY;
			static const std::string PARAMETER_CLEAN_OLD_DATA;
			static const std::string PARAMETER_CLEAN_UNUSED_STOPS;

			PTDataCleanerFileFormat(
				util::Env& env,
				const impex::DataSource& dataSource
			);

			virtual bool beforeParsing();
			virtual bool afterParsing();

		protected:
			boost::shared_ptr<const calendar::CalendarTemplate> _calendarTemplate;
			calendar::Calendar _calendar;
			boost::optional<boost::gregorian::date> _startDate;
			boost::optional<boost::gregorian::date> _endDate;
			bool _fromToday;
			bool _cleanOldData;
			bool _cleanUnusedStops;

		private:
			mutable std::set<boost::shared_ptr<JourneyPattern> > _journeyPatternsToRemove;
			mutable std::set<boost::shared_ptr<ScheduledService> > _scheduledServicesToRemove;
			mutable std::set<boost::shared_ptr<ContinuousService> > _continuousServicesToRemove;
			mutable std::set<boost::shared_ptr<StopPoint> > _stopsToRemove;
			mutable std::set<boost::shared_ptr<StopArea> > _stopAreasToRemove;
			mutable std::set<boost::shared_ptr<const LineStop> > _edgesToRemove;

		public:
			void setCalendar(boost::shared_ptr<const calendar::CalendarTemplate> value){ _calendarTemplate = value; }
			void setStartDate(boost::optional<boost::gregorian::date> value){ _startDate = value; }
			void setEndDate(boost::optional<boost::gregorian::date> value){ _endDate = value; }
			void setFromToday(bool value){ _fromToday = value; }
			virtual void cleanObsoleteData(const boost::gregorian::date& firstDayToKeep) const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// @return Generated parameters map
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			virtual util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			virtual void _setFromParametersMap(const util::ParametersMap& map);

		protected:
			void _cleanCalendars() const;
			void _selectObjectsToRemove() const;
			void _addRemoveQueries(db::DBTransaction& transaction) const;
		};
	}
}

#endif // SYNTHESE_pt_PTDataCleanerFileFormat_hpp__
