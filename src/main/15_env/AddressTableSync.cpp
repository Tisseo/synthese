#include "AddressTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/Address.h"

#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;

namespace synthese
{
	using namespace db;

	namespace env
	{

			AddressTableSync::AddressTableSync ()
				: ComponentTableSync (ADDRESSES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (ADDRESSES_TABLE_COL_PLACEID, "INTEGER", false);
			addTableColumn (ADDRESSES_TABLE_COL_ROADID, "INTEGER", false);
			addTableColumn (ADDRESSES_TABLE_COL_METRICOFFSET, "DOUBLE", false);
			addTableColumn (ADDRESSES_TABLE_COL_X, "DOUBLE", true);
			addTableColumn (ADDRESSES_TABLE_COL_Y, "DOUBLE", true);

		}



		AddressTableSync::~AddressTableSync ()
		{

		}

		    


		void 
		AddressTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
					synthese::env::Environment& environment)
		{
			uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, db::TABLE_COL_ID)));
		    
			if (environment.getAddresses ().contains (id)) return;

			synthese::env::Address* address = new synthese::env::Address (
			id,
			environment.getConnectionPlaces ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_PLACEID))),
			environment.getRoads ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_ROADID))),
			Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_METRICOFFSET)),
			Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_X)),
			Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_Y))
			);
		    

			environment.getAddresses ().add (address);
		}



		void 
		AddressTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
					synthese::env::Environment& environment)
		{
			uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
			synthese::env::Address* address = environment.getAddresses ().get (id);
			address->setX (Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_X)));
			address->setY (Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_Y)));
		}



		void 
		AddressTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
					synthese::env::Environment& environment)
		{
			uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
			environment.getAddresses ().remove (id);
		}













	}

}

