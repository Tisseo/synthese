
#ifndef SYNTHESE_ProfileTableSync_H__
#define SYNTHESE_ProfileTableSync_H__

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Profile SQLite table synchronizer.

			The storage of rights is :
				- rights separated by |
				- parameters of rights separated by ,
			Eg: *,*,10,10|env,*,40,40

			@ingroup m12
		*/
		class ProfileTableSync : public db::SQLiteTableSyncTemplate<Profile>
		{
		private:
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_PARENT_ID;
			static const std::string TABLE_COL_RIGHTS_STRING;

			static void loadProfile(Profile* profile, const db::SQLiteResult& rows, int rowId=0);

		public:

			ProfileTableSync();
			~ProfileTableSync ();

			static void saveProfile(const db::SQLiteThreadExec* sqlite, Profile* profile);


			/** Profile search.
				@param sqlite SQLite thread
				@param name name to search (empty = no search on name)
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others users to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Profile*> Founded users. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<Profile*> searchProfiles(const db::SQLiteThreadExec* sqlite
				, const std::string name
				, int first = 0, int number = 0);

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
#endif // SYNTHESE_ProfileTableSync_H__
