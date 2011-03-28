
/** CurrencyTableSync class implementation.
	@file CurrencyTableSync.cpp

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

#include <sstream>

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLiteException.h"

#include "AccountingModule.h"
#include "Currency.h"
#include "CurrencyTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<SQLiteTableSync,CurrencyTableSync>::FACTORY_KEY("57.00 Currency");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CurrencyTableSync>::TABLE.NAME = "t029_currencies";
		template<> const int SQLiteTableSyncTemplate<CurrencyTableSync>::TABLE.ID = 29;
		template<> const bool SQLiteTableSyncTemplate<CurrencyTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<CurrencyTableSync,Currency>::load(Currency* currency, const db::SQLiteResultSPtr& rows )
		{
			currency->setKey(rows->getLongLong (TABLE_COL_ID));
			currency->setName(rows->getText ( CurrencyTableSync::TABLE_COL_NAME));
			currency->setSymbol(rows->getText ( CurrencyTableSync::TABLE_COL_SYMBOL));
		}

		template<> void SQLiteDirectTableSyncTemplate<CurrencyTableSync,Currency>::_link(Currency* currency, const db::SQLiteResultSPtr& rows, GetSource temporary )
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<CurrencyTableSync,Currency>::_unlink(Currency* currency)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<CurrencyTableSync,Currency>::Save(Currency* currency)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (currency->getKey() <= 0)
				currency->setKey(getId());
            query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(currency->getKey())
				<< "," << Conversion::ToSQLiteString(currency->getName())
				<< "," << Conversion::ToSQLiteString(currency->getSymbol())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace accounts
	{
		const std::string CurrencyTableSync::TABLE_COL_NAME = "name";
		const std::string CurrencyTableSync::TABLE_COL_SYMBOL = "symbol";


		CurrencyTableSync::CurrencyTableSync()
			: SQLiteRegistryTableSyncTemplate<CurrencyTableSync,Currency>()
		{
			addTableColumn(TABLE_COL_ID, "SQL_INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "SQL_TEXT", true);
			addTableColumn(TABLE_COL_SYMBOL, "SQL_TEXT", true);
		}



		std::vector<shared_ptr<Currency> > CurrencyTableSync::search(const std::string& name, const std::string& symbol , int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				<< " AND " << TABLE_COL_SYMBOL << " LIKE '%" << Conversion::ToSQLiteString(symbol, false) << "%'";
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<Currency> > currencies;
				while (rows->next ())
				{
					shared_ptr<Currency> currency(new Currency);
					load(currency.get(), rows);
					currencies.push_back(currency);
				}
				return currencies;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}

		}
	}
}
