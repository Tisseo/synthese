
#ifndef SYNTHESE_VinciContractTableSync_H__
#define SYNTHESE_VinciContractTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace vinci
	{
		class VinciContract;

		/** Vinci Contract SQLite table synchronizer.
			@ingroup m71
		*/

		class VinciContractTableSync : public db::SQLiteTableSyncTemplate<VinciContract>
		{
		public:
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_USER_ID;

			VinciContractTableSync();
			~VinciContractTableSync ();

			/** VinciContract search.
				@param sqlite SQLite thread
				@param name Name to search
				@param surname Surname to search
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Founded contracts. 
				@author Hugues Romain
				@date 2006	
				@warning The returned pointers must be deleted after use to avoid memory leak
			*/
			static std::vector<VinciContract*> searchVinciContracts(const db::SQLiteThreadExec* sqlite
				, std::string name = "", std::string surname = ""
				, int first = 0, int number = -1);


		protected:

			/** Action to do on user creation.
			No action because the users are not permanently loaded in ram.
			*/
			void rowsAdded (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user creation.
			Updates the users objects in the opened sessions.
			*/
			void rowsUpdated (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user deletion.
			Closes the sessions of the deleted user.
			*/
			void rowsRemoved (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}

#endif 
