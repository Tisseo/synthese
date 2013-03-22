
/** AlarmObjectLinkTableSync class implementation.
	@file AlarmObjectLinkTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "AlarmObjectLinkTableSync.h"

#include "SentScenario.h"
#include "AlarmObjectLinkException.h"
#include "AlarmRecipient.h"
#include "AlarmObjectLink.h"
#include "Profile.h"
#include "SentAlarm.h"
#include "Session.h"
#include "User.h"
#include "Registry.h"
#include "AlarmTableSync.h"
#include "LoadException.h"
#include "LinkException.h"
#include "DeleteQuery.hpp"
#include "ReplaceQuery.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"
#include "AlarmTemplate.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,AlarmObjectLinkTableSync>::FACTORY_KEY("99.00.01 Alarm links");
	}

	namespace messages
	{
		const string AlarmObjectLinkTableSync::COL_RECIPIENT_KEY("recipient_key");
		const string AlarmObjectLinkTableSync::COL_OBJECT_ID("object_id");
		const string AlarmObjectLinkTableSync::COL_ALARM_ID("alarm_id");
		const string AlarmObjectLinkTableSync::COL_PARAMETER = "parameter";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<AlarmObjectLinkTableSync>::TABLE(
			"t040_alarm_object_links"
		);

		template<> const Field DBTableSyncTemplate<AlarmObjectLinkTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(AlarmObjectLinkTableSync::COL_RECIPIENT_KEY, SQL_TEXT),
			Field(AlarmObjectLinkTableSync::COL_OBJECT_ID, SQL_INTEGER),
			Field(AlarmObjectLinkTableSync::COL_ALARM_ID, SQL_INTEGER),
			Field(AlarmObjectLinkTableSync::COL_PARAMETER, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<AlarmObjectLinkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(AlarmObjectLinkTableSync::COL_OBJECT_ID.c_str(),	AlarmObjectLinkTableSync::COL_ALARM_ID.c_str(), ""));
			r.push_back(DBTableSync::Index(AlarmObjectLinkTableSync::COL_ALARM_ID.c_str(), ""));
			return r;
		}

		template<>
		void OldLoadSavePolicy<AlarmObjectLinkTableSync,AlarmObjectLink>::Load(
			AlarmObjectLink* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// It makes no sense to load such an object without the up links
			assert(linkLevel > FIELDS_ONLY_LOAD_LEVEL);

			object->setRecipientKey(rows->getText ( AlarmObjectLinkTableSync::COL_RECIPIENT_KEY));
			object->setParameter(rows->getText(AlarmObjectLinkTableSync::COL_PARAMETER));

			// Object
			try
			{
				if(rows->getLongLong(AlarmObjectLinkTableSync::COL_OBJECT_ID) > 0)
				{
					object->setObject(
						DBModule::GetEditableObject(
							rows->getLongLong(AlarmObjectLinkTableSync::COL_OBJECT_ID),
							env
						).get()
					);
				}
		
				// Alarm
				Alarm& alarm(
					*AlarmTableSync::GetEditable(
						rows->getLongLong(AlarmObjectLinkTableSync::COL_ALARM_ID),
						env,
						linkLevel
				)	);
				object->setAlarm(&alarm);

				// link the object in the alarm (only if the linked object was found)
				Alarm::LinkedObjects linkedObjects(alarm.getLinkedObjects());
				Alarm::LinkedObjects::iterator it(
					linkedObjects.find(object->getRecipientKey())
				);
				if(it == linkedObjects.end())
				{
					it = linkedObjects.insert(
						make_pair(
							object->getRecipientKey(),
							Alarm::LinkedObjects::mapped_type()
					)	).first;
				}
				it->second.insert(object);
				alarm.setLinkedObjects(linkedObjects);

				// link the alarm in the object
				if(	linkLevel >= RECURSIVE_LINKS_LOAD_LEVEL &&
					dynamic_cast<SentAlarm*>(&alarm) &&
					static_cast<SentAlarm&>(alarm).getScenario()
				){
					shared_ptr<AlarmRecipient> ar(Factory<AlarmRecipient>::create(object->getRecipientKey()));
					ar->addObject(*object);
				}
			}
			catch(ObjectNotFoundException<Registrable>& e)
			{
				Log::GetInstance().warn("Data corrupted in " + AlarmObjectLinkTableSync::TABLE.NAME + "/" + AlarmObjectLinkTableSync::COL_OBJECT_ID, e);
			}
			catch(FactoryException<AlarmRecipient> e)
			{
				throw LoadException<AlarmObjectLinkTableSync>(rows, AlarmObjectLinkTableSync::COL_RECIPIENT_KEY, "Inconsistent recipient type");
			}
			catch(ObjectNotFoundException<Alarm> e)
			{
				throw LinkException<AlarmObjectLinkTableSync>(rows, AlarmObjectLinkTableSync::COL_ALARM_ID, e);
			}
			catch (AlarmObjectLinkException e)
			{
				throw LoadException<AlarmObjectLinkTableSync>(rows, AlarmObjectLinkTableSync::COL_OBJECT_ID, e.getMessage());
			}
		}



		template<>
		void OldLoadSavePolicy<AlarmObjectLinkTableSync,AlarmObjectLink>::Unlink(
			AlarmObjectLink* object
		){
			if(object->getAlarm())
			{
				Alarm& alarm(*object->getAlarm());
				Alarm::LinkedObjects linkedObjects(alarm.getLinkedObjects());
				Alarm::LinkedObjects::iterator it(
					linkedObjects.find(object->getRecipientKey())
				);
				if(it != linkedObjects.end())
				{
					it->second.erase(object);
					alarm.setLinkedObjects(linkedObjects);
				}
			}

			// Registration in the "recipient to message" map, not for template messages
			if(dynamic_cast<SentAlarm*>(object->getAlarm()))
			{
				try
				{
					shared_ptr<AlarmRecipient> ar(Factory<AlarmRecipient>::create(object->getRecipientKey()));
					ar->removeObject(*object);
				}
				catch(FactoryException<AlarmRecipient> e)
				{
					Log::GetInstance().error("Unhanded recipient type "+ object->getRecipientKey() +" in "+ AlarmObjectLinkTableSync::TABLE.NAME +" object "+ lexical_cast<string>(object->getKey()), e);
				}
			}
		}


		template<>
		void OldLoadSavePolicy<AlarmObjectLinkTableSync,AlarmObjectLink>::Save(
			AlarmObjectLink* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<AlarmObjectLinkTableSync> query(*object);
			query.addField(object->getRecipientKey());
			query.addField(object->getObject() ? object->getObject()->getKey() : RegistryKeyType(0));
			query.addField(object->getAlarm() ? object->getAlarm()->getKey() : RegistryKeyType(0));
			query.addField(object->getParameter());
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<AlarmObjectLinkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				shared_ptr<const AlarmObjectLink> aol(AlarmObjectLinkTableSync::Get(object_id, env));
				if(dynamic_cast<AlarmTemplate*>(aol->getAlarm()))
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
				}
				else
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
				}
			}
			catch (ObjectNotFoundException<AlarmObjectLink>&)
			{
				return false;
			}
		}



		template<> void DBTableSyncTemplate<AlarmObjectLinkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlarmObjectLinkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlarmObjectLinkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			shared_ptr<const AlarmObjectLink> aol(AlarmObjectLinkTableSync::Get(id, env));
			if(aol->getObject())
			{
				if (dynamic_cast<AlarmTemplate*>(aol->getAlarm()))
				{
					MessagesLibraryLog::addUpdateEntry(
						dynamic_cast<AlarmTemplate*>(aol->getAlarm()),
						"Suppression de la destination "+ lexical_cast<string>(aol->getObject()->getKey()),
						session->getUser().get()
						);
				}
				else if(dynamic_cast<SentAlarm*>(aol->getAlarm()))
				{
					MessagesLog::addUpdateEntry(
						dynamic_cast<SentAlarm*>(aol->getAlarm()),
						"Suppression de la destination "+ lexical_cast<string>(aol->getObject()->getKey()),
						session->getUser().get()
					);
				}
			}
		}
	}

	namespace messages
	{
		void AlarmObjectLinkTableSync::Remove(
			RegistryKeyType alarmId,
			optional<RegistryKeyType> objectId
		){
			DeleteQuery<AlarmObjectLinkTableSync> query;
			query.addWhereField(COL_ALARM_ID, alarmId);
			if(objectId)
			{
				query.addWhereField(COL_OBJECT_ID, *objectId);
			}
			query.execute();
		}



		void AlarmObjectLinkTableSync::CopyRecipients(
			RegistryKeyType sourceId,
			Alarm& destAlarm,
			optional<DBTransaction&> transaction
		){
			Env lenv;
			SearchResult links(
				Search(lenv, sourceId)
			);
			BOOST_FOREACH(const shared_ptr<AlarmObjectLink>& aol, links)
			{
				AlarmObjectLink naol;
				naol.setAlarm(&destAlarm);
				naol.setObject(aol->getObject());
				naol.setRecipientKey(aol->getRecipientKey());
				naol.setParameter(aol->getParameter());
				Save(&naol, transaction);
			}
		}



		AlarmObjectLinkTableSync::SearchResult AlarmObjectLinkTableSync::Search(
			Env& env,
			util::RegistryKeyType alarmId,
			int first /*= 0*/,
			boost::optional<std::size_t> number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << alarmId;
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		void AlarmObjectLinkTableSync::RemoveByTarget(
			util::RegistryKeyType objectId,
			optional<DBTransaction&> transaction
		){
			DeleteQuery<AlarmObjectLinkTableSync> query;
			query.addWhereField(COL_OBJECT_ID, objectId);
			query.execute(transaction);
		}
	}
}
