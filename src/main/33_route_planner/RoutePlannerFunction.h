
/** RoutePlannerFunction class header.
	@file RoutePlannerFunction.h

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

#ifndef SYNTHESE_RoutePlannerFunction_H__
#define SYNTHESE_RoutePlannerFunction_H__

#include "36_places_list/FunctionWithSite.h"
#include "36_places_list/Types.h"

#include "15_env/Types.h"

#include "04_time/DateTime.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace transportwebsite
	{
		class HourPeriod;
	}

	namespace env
	{
		class Place;
	}

	namespace routeplanner
	{
		class RoutePlannerInterfacePage;

		/** Route planning Function class.
			@ingroup m53Functions refFunctions
		*/
		class RoutePlannerFunction : public util::FactorableTemplate<transportwebsite::FunctionWithSite,RoutePlannerFunction>
		{
		public:
			static const std::string PARAMETER_SITE;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_DAY;
			static const std::string PARAMETER_PERIOD_ID;
			static const std::string PARAMETER_MAX_SOLUTIONS_NUMBER;
			static const std::string PARAMETER_DEPARTURE_CITY_TEXT;
			static const std::string PARAMETER_ARRIVAL_CITY_TEXT;
			static const std::string PARAMETER_DEPARTURE_PLACE_TEXT;
			static const std::string PARAMETER_ARRIVAL_PLACE_TEXT;
			static const std::string PARAMETER_ACCESSIBILITY;
			
		private:
			//! \name Parameters
			//@{
				const RoutePlannerInterfacePage*			_page;
				const env::Place*							_departure_place;
				const env::Place*							_arrival_place;
				std::string									_originCityText;
				std::string									_destinationCityText;
				std::string									_originPlaceText;
				std::string									_destinationPlaceText;
				time::DateTime								_startDate;
				time::DateTime								_endDate;
				AccessParameters							_accessParameters;
				int											_maxSolutionsNumber;
				int											_periodId;
				const transportwebsite::HourPeriod*			_period;
				bool										_home;
				transportwebsite::AccessibilityParameter	_accessibility;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;

			int getMaxSolutions() const;

			void setMaxSolutions(int number);

			RoutePlannerFunction();
		};
	}
}

#endif // SYNTHESE_RoutePlannerFunction_H__
