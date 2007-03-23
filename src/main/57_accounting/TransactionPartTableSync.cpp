
/** TransactionPartTableSync class implementation.
	@file TransactionPartTableSync.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Rapha�l Murat - Vinci Park <rmurat@vincipark.com>

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

#include "01_util/Conversion.h"
#include "01_util/RegistryKeyException.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "12_security/User.h"

#include "57_accounting/AccountingModule.h"
#include "57_accounting/Account.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace accounts;
	using namespace security;
	using namespace time;
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TransactionPart>::TABLE_NAME = "t030_transaction_parts";
		template<> const int SQLiteTableSyncTemplate<TransactionPart>::TABLE_ID = 30;
		template<> const bool SQLiteTableSyncTemplate<TransactionPart>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<TransactionPart>::load(TransactionPart* tp, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			tp->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			tp->setTransactionId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_TRANSACTION_ID)));
			tp->setAccountId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_ACCOUNT_ID)));
			tp->setComment(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_COMMENT));
			tp->setLeftCurrencyAmount(Conversion::ToDouble(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT)));
			tp->setRightCurrencyAmount(Conversion::ToDouble(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_RIGHT_CURRENCY_AMOUNT)));
			tp->setRateId(Conversion::ToLongLong(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_RATE_ID)));
			tp->setTradedObjectId(rows.getColumn(rowId, TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID));
		}

		template<> void SQLiteTableSyncTemplate<TransactionPart>::save(TransactionPart* tp)
		{
			try
			{
				const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
				if (tp->getKey() > 0)
				{
					/// @todo UPDATE implementation
				}
				else
				{
					tp->setKey(getId(1,1)); /// @todo Handle grid
					stringstream query;
					query << "INSERT INTO " << TABLE_NAME << " VALUES("
						<< Conversion::ToString(tp->getKey())
						<< "," << Conversion::ToString(tp->getTransactionId())
						<< "," << Conversion::ToString(tp->getLeftCurrencyAmount())
						<< "," << Conversion::ToString(tp->getRightCurrencyAmount())
						<< "," << Conversion::ToString(tp->getAccountId())
						<< "," << Conversion::ToString(tp->getRateId())
						<< "," << Conversion::ToSQLiteString(tp->getTradedObjectId())
						<< "," << Conversion::ToSQLiteString(tp->getComment())
						<< ')';
					sqlite->execUpdate(query.str());
				}
			}
			catch (SQLiteException& e)
			{

			}
		}


	}

	namespace accounts
	{
		const std::string TransactionPartTableSync::TABLE_COL_ID = "id";
		const std::string TransactionPartTableSync::TABLE_COL_TRANSACTION_ID = "transaction_id";
		const std::string TransactionPartTableSync::TABLE_COL_LEFT_CURRENCY_AMOUNT = "left_currency_amount";
		const std::string TransactionPartTableSync::TABLE_COL_RIGHT_CURRENCY_AMOUNT = "right_currency_amount";
		const std::string TransactionPartTableSync::TABLE_COL_ACCOUNT_ID = "account_id";
		const std::string TransactionPartTableSync::TABLE_COL_RATE_ID = "rate_id";
		const std::string TransactionPartTableSync::TABLE_COL_TRADED_OBJECT_ID = "traded_object";
		const std::string TransactionPartTableSync::TABLE_COL_COMMENT = "comment";

		TransactionPartTableSync::TransactionPartTableSync()
			: SQLiteTableSyncTemplate<TransactionPart>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE, true)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_TRANSACTION_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_LEFT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_RIGHT_CURRENCY_AMOUNT, "REAL", true);
			addTableColumn(TABLE_COL_ACCOUNT_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_RATE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_TRADED_OBJECT_ID, "TEXT", true);
			addTableColumn(TABLE_COL_COMMENT, "TEXT", true);

		}

		void TransactionPartTableSync::rowsAdded( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionPartTableSync::rowsUpdated( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		void TransactionPartTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{

		}

		vector<TransactionPart*> TransactionPartTableSync::search(
				const Transaction* transaction, const Account* account
				, int first, int number)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT * "
				<< " FROM " << TABLE_NAME << " AS p "
				<< " WHERE "
				<< " p." << TABLE_COL_TRANSACTION_ID << "=" << Conversion::ToString(transaction->getKey());
			if (account != NULL)
				query << " AND p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey());
			query << " LIMIT " << number << " OFFSET " << first;
			
			SQLiteResult result = sqlite->execQuery(query.str());
			vector<TransactionPart*> tps;
			for (int i=0; i<result.getNbRows(); ++i)
			{
//				try
//				{
					TransactionPart* tp = new TransactionPart;
					load(tp, result, i);
					tps.push_back(tp);
//				}
			}
			return tps;
		}

		vector<TransactionPart*> TransactionPartTableSync::search(Account* account, User* user, bool order, int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT * "
				<< " FROM " << TABLE_NAME << " AS p "
				<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " WHERE "
				<< " p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
				;
			if (user != NULL)
				query << " AND t." << TransactionTableSync::TABLE_COL_LEFT_USER_ID << "=" << Conversion::ToString(user->getKey());
			query << " ORDER BY " << TransactionTableSync::TABLE_COL_START_DATE_TIME;
			if (!order)
				query << " DESC ";
			query << " LIMIT " << number << " OFFSET " << first;
			
			SQLiteResult result = sqlite->execQuery(query.str());
			vector<TransactionPart*> tps;
			for (int i=0; i<result.getNbRows(); ++i)
			{
//				try
//				{
					TransactionPart* tp = new TransactionPart;
					load(tp, result, i);
					tps.push_back(tp);
//				}
			}
			return tps;
		}

		map<int, int> TransactionPartTableSync::count(Account* account, Date startDate, Date endDate, int first, int number)
		{
			int transactionsNumber;
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT strftime('%H', t.start_date_time) AS hours,"
					<< " count(t." << TABLE_COL_ID << ") AS numbers "
				<< " FROM " << TABLE_NAME << " AS p "
					<< " INNER JOIN " << TransactionTableSync::TABLE_NAME << " AS t ON t." << TABLE_COL_ID << "=p." << TABLE_COL_TRANSACTION_ID
				<< " WHERE "
					<< " p." << TABLE_COL_ACCOUNT_ID << "=" << Conversion::ToString(account->getKey())
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << "<=" << endDate.toSQLString()
					<< " AND t." << TransactionTableSync::TABLE_COL_START_DATE_TIME << ">=" << startDate.toSQLString()
				<< " GROUP BY strftime('%H', t.start_date_time) "
				<< " LIMIT " << number << " OFFSET " << first
			;
			
			SQLiteResult result = sqlite->execQuery(query.str());
			map<int, int> mapii;
			for (int i=0; i<result.getNbRows(); ++i)
			{
//				try
//				{
				mapii.insert(make_pair(Conversion::ToInt(result.getColumn(i, "hours")), Conversion::ToInt(result.getColumn(i, "numbers"))));
//				}
			}
			return mapii;
		}

	}
}
