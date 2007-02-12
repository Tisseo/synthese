
/** DisplayScreenTableSync class header.
	@file DisplayScreenTableSync.h

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

#ifndef SYNTHESE_DisplayScreenTableSync_H__
#define SYNTHESE_DisplayScreenTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;

		/** DisplayScreen table synchronizer.
			@ingroup m34
		*/
		class DisplayScreenTableSync : public db::SQLiteTableSyncTemplate<DisplayScreen>
		{
		public:
			//! \name Columns
			//@{
			static const std::string COL_BROADCAST_POINT_ID;
			static const std::string COL_BROADCAST_POINT_COMMENT;
			static const std::string COL_TYPE_ID;
			static const std::string COL_WIRING_CODE;
			static const std::string COL_TITLE;
			static const std::string COL_BLINKING_DELAY;
			static const std::string COL_TRACK_NUMBER_DISPLAY;
			static const std::string COL_SERVICE_NUMBER_DISPLAY;
			static const std::string COL_PHYSICAL_STOPS_IDS;	//!< List of physical stops uids, separated by comas
			static const std::string COL_ALL_PHYSICAL_DISPLAYED;
			static const std::string COL_FORBIDDEN_ARRIVAL_PLACES_IDS;	//!< List of forbidden connection places uids, separated by comas
			static const std::string COL_FORBIDDEN_LINES_IDS;	//!< List of forbidden lines uids, separated by comas
			static const std::string COL_DIRECTION;
			static const std::string COL_ORIGINS_ONLY;
			static const std::string COL_DISPLAYED_PLACES_IDS;	//!< List of displayed places uids, separated by comas
			static const std::string COL_MAX_DELAY;
			static const std::string COL_CLEARING_DELAY;
			static const std::string COL_FIRST_ROW;
			static const std::string COL_GENERATION_METHOD;
			static const std::string COL_FORCED_DESTINATIONS_IDS;	//!< List of forced destination uids in preselection, separated by comas
			static const std::string COL_DESTINATION_FORCE_DELAY;
			static const std::string COL_MAINTENANCE_CHECKS_PER_DAY;
			static const std::string COL_MAINTENANCE_IS_ONLINE;
			static const std::string COL_MAINTENANCE_MESSAGE;
			//@}
			


			DisplayScreenTableSync();
			~DisplayScreenTableSync();


			/** DisplayScreen search.
				(other search parameters)
				@param first First DisplayScreen object to answer
				@param number Number of DisplayScreen objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<DisplayScreen*> Founded DisplayScreen objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<DisplayScreen*> search(
				std::string duid=""
				, uid localizationid = UNKNOWN_VALUE
				, uid lineid = UNKNOWN_VALUE
				, uid typeuid = UNKNOWN_VALUE
				, int state = UNKNOWN_VALUE
				, int message = UNKNOWN_VALUE
				, int first = 0, int number = 0);


		protected:

			/** Action to do on DisplayScreen creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on DisplayScreen creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on DisplayScreen deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};
	}
}

#endif // SYNTHESE_DisplayScreenTableSync_H__
