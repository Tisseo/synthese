#ifndef SYNTHESE_DB_SQLITESTATEMENT_H
#define SYNTHESE_DB_SQLITESTATEMENT_H

#include "01_util/threads/ThreadAware.h"


#include <sqlite3.h>

#include <boost/shared_ptr.hpp>

#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>



namespace synthese
{
	namespace db
	{
	    class SQLite;
	    class SQLiteHandle;

	    typedef std::string SQLData;

		/** SQLite  statement. 
		    Natively an SQLite statement is a  SQL command.
		    This class reflects this model and cannot contain a batch of commands.

		    @ingroup m10
		*/
	    class SQLiteStatement : public util::ThreadAware
	    {

		private:
		
		sqlite3_stmt* _statement;
		const SQLData _sql;

		
		SQLiteStatement (sqlite3_stmt* statement, const SQLData& sql);
		SQLiteStatement (const SQLiteStatement&);
		
		SQLiteStatement& operator = (const SQLiteStatement&);

		public:

		    ~SQLiteStatement ();

		    const SQLData& getSQL () const { return _sql; }

		    int getParameterIndex (const std::string& parameterName) const;
		    
		    void reset ();
		    void clearBindings ();

		    void bindParameterBlob (int index, const std::string& param);
		    void bindParameterBlob (const std::string& name, const std::string& param);

		    void bindParameterInt (int index, int param);
		    void bindParameterInt (const std::string& name, int param);

		    void bindParameterDouble (int index, double param);
		    void bindParameterDouble (const std::string& name, double param);

		    void bindParameterTimestamp (int index, const boost::posix_time::ptime& param);
		    void bindParameterTimestamp (const std::string& name, const boost::posix_time::ptime& param);

		    void bindParameterText (int index, const std::string& param);
		    void bindParameterText (const std::string& name, const std::string& param);

		    void bindParameterBool (int index, bool param);
		    void bindParameterBool (const std::string& name, bool param);

		    void bindParameterLong (int index, long param);
		    void bindParameterLong (const std::string& name, long param);

		    void bindParameterLongLong (int index, long long param);
		    void bindParameterLongLong (const std::string& name, long long param);


		private:

		    sqlite3_stmt* getStatement () const;

		    friend class SQLiteLazyResult;
		    friend class SQLiteBatchStatement;
		    friend class SQLiteHandle;

		};

	    typedef boost::shared_ptr<SQLiteStatement> SQLiteStatementSPtr;

	}
}

#endif

