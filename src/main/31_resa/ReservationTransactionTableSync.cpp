
/** ReservationTransactionTableSync class implementation.
	@file ReservationTransactionTableSync.cpp

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

#include "ReservationTransactionTableSync.h"
#include "ReservationTransaction.h"
#include "31_resa/ResaModule.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace time;

	namespace util
	{
		// template<> const std::string FactorableTemplate<SQLiteTableSync, ReservationTransactionTableSync>::FACTORY_KEY("");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ReservationTransaction>::TABLE_NAME("t046_reservation_transactions");
		template<> const int SQLiteTableSyncTemplate<ReservationTransaction>::TABLE_ID(46);
		template<> const bool SQLiteTableSyncTemplate<ReservationTransaction>::HAS_AUTO_INCREMENT(true);

		template<> void SQLiteTableSyncTemplate<ReservationTransaction>::load(
			ReservationTransaction* object
			, const db::SQLiteResultSPtr& rows
		){
			object->setKey(rows->getLongLong(TABLE_COL_ID));
			object->setLastReservation(rows->getLongLong ( ReservationTransactionTableSync::COL_LAST_RESERVATION_ID));
			object->setSeats(rows->getInt( ReservationTransactionTableSync::COL_SEATS));
			object->setBookingTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTransactionTableSync::COL_BOOKING_TIME)));
			object->setCancellationTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTransactionTableSync::COL_CANCELLATION_TIME)));
			object->setCustomerUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CUSTOMER_ID));
			object->setCustomerName(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_NAME));
			object->setCustomerPhone(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_PHONE));
			object->setBookingUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_BOOKING_USER_ID));
			object->setCancelUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CANCEL_USER_ID));
		}

		template<> void SQLiteTableSyncTemplate<ReservationTransaction>::save(ReservationTransaction* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToString(object->getLastReservation())
				<< "," << Conversion::ToString(object->getSeats())
				<< "," << object->getBookingTime().toSQLString()
				<< "," << object->getCancellationTime().toSQLString()
				<< "," << Conversion::ToString(object->getCustomerUserId())
				<< "," << Conversion::ToSQLiteString(object->getCustomerName())
				<< "," << Conversion::ToSQLiteString(object->getCustomerPhone())
				<< "," << Conversion::ToString(object->getBookingUserId())
				<< "," << Conversion::ToString(object->getCancelUserId())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace resa
	{
		// const std::string ReservationTransactionTableSync::COL_xx("xx");
		const string ReservationTransactionTableSync::COL_LAST_RESERVATION_ID = "last_reservation_id";
		const string ReservationTransactionTableSync::COL_SEATS = "seats";
		const string ReservationTransactionTableSync::COL_BOOKING_TIME = "booking_time";
		const string ReservationTransactionTableSync::COL_CANCELLATION_TIME = "cancellation_time";
		const string ReservationTransactionTableSync::COL_CUSTOMER_ID = "customer_id";
		const string ReservationTransactionTableSync::COL_CUSTOMER_NAME = "customer_name";
		const string ReservationTransactionTableSync::COL_CUSTOMER_PHONE = "customer_phone";
		const string ReservationTransactionTableSync::COL_BOOKING_USER_ID = "booking_user_id";
		const string ReservationTransactionTableSync::COL_CANCEL_USER_ID = "cancel_user_id";

		ReservationTransactionTableSync::ReservationTransactionTableSync()
			: SQLiteTableSyncTemplate<ReservationTransaction>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_LAST_RESERVATION_ID, "INTEGER");
			addTableColumn(COL_SEATS, "INTEGER");
			addTableColumn(COL_BOOKING_TIME, "TIMESTAMP");
			addTableColumn(COL_CANCELLATION_TIME, "TIMESTAMP");
			addTableColumn(COL_CUSTOMER_ID, "INTEGER");
			addTableColumn(COL_CUSTOMER_NAME, "TEXT");
			addTableColumn(COL_CUSTOMER_PHONE, "TEXT");
			addTableColumn(COL_BOOKING_USER_ID, "INTEGER");
			addTableColumn(COL_CANCEL_USER_ID, "INTEGER");
		}

		void ReservationTransactionTableSync::rowsAdded(
			SQLiteQueueThreadExec* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
			, bool isItFirstSync
		){
		}
		
		void ReservationTransactionTableSync::rowsUpdated(
			SQLiteQueueThreadExec* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
		){
		}

		void ReservationTransactionTableSync::rowsRemoved(
			SQLiteQueueThreadExec* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
		){
		}

		vector<shared_ptr<ReservationTransaction> > ReservationTransactionTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLiteHandle* sqlite(DBModule::GetSQLite());

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<ReservationTransaction> > objects;
				while (rows->next ())
				{
					shared_ptr<ReservationTransaction> object(new ReservationTransaction);
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		ReservationTransactionTableSync::~ReservationTransactionTableSync()
		{

		}
	}
}
