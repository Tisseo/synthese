
/** MessageAdmin class implementation.
	@file MessageAdmin.cpp

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

#include "01_util/Html.h"

#include "17_messages/MessageAdmin.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/Types.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/UpdateAlarmAction.h"
#include "17_messages/UpdateAlarmMessagesFromTemplateAction.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace messages
	{
		/// @todo Verify the parent constructor parameters
		MessageAdmin::MessageAdmin()
			: AdminInterfaceElement("messages", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}

		string MessageAdmin::getTitle() const
		{
			return _alarm->getShortMessage();
		}

		void MessageAdmin::setFromParametersMap( const AdminRequest::ParametersMap& map )
		{
			Request::ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw AdminParametersException("Missing message ID");

			if (Conversion::ToLongLong(it->second) == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			if (!MessagesModule::getAlarms().contains(Conversion::ToLongLong(it->second)))
				throw AdminParametersException("Invalid message ID");
			
			_alarm = MessagesModule::getAlarms().get(Conversion::ToLongLong(it->second));
		}

		void MessageAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
			updateRequest->setAction(Factory<Action>::create<UpdateAlarmAction>());
			updateRequest->setObjectId(request->getObjectId());

			AdminRequest* templateRequest = Factory<Request>::create<AdminRequest>();
			templateRequest->copy(request);
			templateRequest->setPage(Factory<AdminInterfaceElement>::create<MessageAdmin>());
			templateRequest->setAction(Factory<Action>::create<UpdateAlarmMessagesFromTemplateAction>());
			templateRequest->setObjectId(request->getObjectId());


			// Alarm level map
			map<int, string> lmap;
			lmap.insert(make_pair((int) ALARM_LEVEL_WARNING, "Compl�mentaire"));
			lmap.insert(make_pair((int) ALARM_LEVEL_ERROR, "Prioritaire"));

			// Templates map
			map<int, string> tmap;

			stream
				<< "<h1>Param�tres</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Type</td><td>" << Html::getRadioInput(UpdateAlarmAction::PARAMETER_TYPE, lmap, (int) _alarm->getLevel()) << "</td></tr>"
				<< "<tr><td>D�but diffusion</td><td>Date " << Html::getTextInput(UpdateAlarmAction::PARAMETER_START_DATE, _alarm->getPeriodStart().getDate().toSQLString(false)) 
				<< " Heure " << Html::getTextInput(UpdateAlarmAction::PARAMETER_START_HOUR, _alarm->getPeriodStart().getHour().toSQLString(false)) << "</td></tr>"
				<< "<tr><td>Fin diffusion</td><td>Date " << Html::getTextInput(UpdateAlarmAction::PARAMETER_END_DATE, _alarm->getPeriodStart().getDate().toSQLString(false)) 
				<< " Heure " << Html::getTextInput(UpdateAlarmAction::PARAMETER_END_HOUR, _alarm->getPeriodStart().getHour().toSQLString(false)) << "</td></tr>"
				<< "<tr><td colspan=\"2\">" << Html::getSubmitButton("Enregistrer") << "</td></tr>"
				<< "</table></form>"

				<< "<h1>Contenu</h1>"
				<< "<table>"
				<< "<tr><td>Mod�le</td><td>" 
				<< templateRequest->getHTMLFormHeader("template")
				<< Html::getSelectInput(UpdateAlarmMessagesFromTemplateAction::PARAMETER_TEMPLATE_ID, tmap, 0) 
				<< Html::getSubmitButton("Copier contenu")
                << "</form></td></tr>"
				
				<< "<tr><td>Message court</td><td>" << Html::getTextAreaInput("", _alarm->getShortMessage(), 2, 20) << "</td></tr>"
				<< "<tr><td>Message long</td><td>" << Html::getTextAreaInput("", _alarm->getLongMessage(), 4, 30) << "</td></tr>"
				<< "</table>";

			// Alarm messages destinations loop
			for (Factory<AlarmRecipient>::Iterator arit = Factory<AlarmRecipient>::begin(); arit != Factory<AlarmRecipient>::end(); ++arit)
			{
				stream << "<h1>Diffusion sur " << arit->getTitle() << "</h1>";

				arit->displayBroadcastListEditor(stream, _alarm, request);
			}

			delete updateRequest;
			delete templateRequest;
		}
	}
}
