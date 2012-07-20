////////////////////////////////////////////////////////////////////////////////
/// TransportNetworkAdmin class implementation.
///	@file TransportNetworkAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "TransportNetworkAdmin.h"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"
#include "CommercialLine.h"
#include "CommercialLineAdmin.h"
#include "CommercialLineTableSync.h"
#include "ModuleAdmin.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "PTModule.h"
#include "PTNetworksAdmin.h"
#include "RemoveObjectAction.hpp"
#include "TransportNetworkRight.h"
#include "TreeFolder.hpp"
#include "ScheduledService.h"
#include "SearchFormHTMLTable.h"
#include "CommercialLineUpdateAction.h"
#include "ImportableAdmin.hpp"
#include "TransportNetworkUpdateAction.hpp"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace security;
	using namespace html;
	using namespace impex;
	using namespace calendar;
	using namespace tree;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TransportNetworkAdmin>::FACTORY_KEY("TransportNetworkAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TransportNetworkAdmin>::ICON("chart_organisation.png");
		template<> const string AdminInterfaceElementTemplate<TransportNetworkAdmin>::DEFAULT_TITLE("Réseau inconnu");
	}

	namespace pt
	{
		const string TransportNetworkAdmin::PARAMETER_SEARCH_NAME("sn");
		const string TransportNetworkAdmin::PARAMETER_SEARCH_NETWORK_ID("sw");


		void TransportNetworkAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Search parameters
			_searchName = map.getDefault<string>(PARAMETER_SEARCH_NAME);
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 100);

			// Node
			_loadNodeFromMainEnv(map);
		}



		util::ParametersMap TransportNetworkAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_SEARCH_NAME,_searchName);

			// Node
			_getNodeParametersMap(m);

			return m;
		}



		void TransportNetworkAdmin::display(
			ostream& stream,
			const admin::AdminRequest& _request
		) const	{

			// Search form
			stream << "<h1>Recherche</h1>";
			AdminFunctionRequest<TransportNetworkAdmin> searchRequest(_request);
			HTMLForm sortedForm(searchRequest.getHTMLForm("search"));

			getHTMLLineSearchForm(
				stream,
				sortedForm,
				optional<RegistryKeyType>(),
				_searchName
			);

			// Results display
			stream << "<h1>Lignes du ";
			if(getRoot())
			{
				stream << "réseau";
			}
			if(getFolder())
			{
				stream << "répertoire";
			}
			stream << "</h1>";

			// Lines search
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					Env::GetOfficialEnv(),
					getNode()->getKey(),
					string("%"+_searchName+"%"),
					optional<string>(),
					_requestParameters.first,
					_requestParameters.maxSize,
					false,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);

			// Requests
			AdminFunctionRequest<CommercialLineAdmin> lineOpenRequest(_request);
			AdminActionFunctionRequest<RemoveObjectAction, TransportNetworkAdmin> lineRemoveRequest(_request);
			AdminActionFunctionRequest<CommercialLineUpdateAction, CommercialLineAdmin> creationRequest(_request);
			creationRequest.getFunction()->setActionFailedPage(getNewCopiedPage());
			creationRequest.setActionWillCreateObject();
			creationRequest.getAction()->setNetwork(
				Env::GetOfficialEnv().getEditableSPtr(const_cast<TransportNetwork*>(getNodeRoot()))
			);
			
			// Table
			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), string()));
			h.push_back(make_pair(string(), "N°"));
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), string()));
			ResultHTMLTable t(h,sortedForm,_requestParameters, lines);
			stream << t.open();

			// Rows
			BOOST_FOREACH(const shared_ptr<CommercialLine>& line, lines)
			{
				// Row opening
				stream << t.row();

				// Open button
				stream << t.col();
				lineOpenRequest.getPage()->setCommercialLine(line);
				stream << HTMLModule::getLinkButton(lineOpenRequest.getURL(), "Ouvrir", string(), "chart_line_edit.png");

				// Short name
				stream << t.col(1, line->getStyle(), true);
				stream << line->getShortName();

				// Long name
				stream << t.col();
				stream << line->getName();

				// Remove action
				stream << t.col();
				if(line->getPaths().empty())
				{
					lineRemoveRequest.getAction()->setObjectId(line->getKey());
					stream << HTMLModule::getLinkButton(
						lineRemoveRequest.getURL(),
						"Supprimer",
						"Etes-vous sûr de vouloir supprimer la ligne "+ line->getShortName() +" ?"
					);
				}
			}

			// Creation row
			stream << t.row();
			stream << t.col(3) << "Création de ligne";
			stream << t.col() << HTMLModule::getLinkButton(creationRequest.getURL(), "Créer");
			stream << t.close();

			// Network properties
			if(getRoot())
			{
				const TransportNetwork& network(*getRoot());

				// Update request
				AdminActionFunctionRequest<TransportNetworkUpdateAction,TransportNetworkAdmin> updateRequest(_request);
				updateRequest.getAction()->setNetwork(
					Env::GetOfficialEnv().getEditableSPtr(const_cast<TransportNetwork*>(getRoot()))
				);

				// Section title
				stream << "<h1>Propriétés</h1>";

				// The form
				PropertiesHTMLTable p(updateRequest.getHTMLForm("update"));
				stream << p.open();
				stream << p.cell("ID", lexical_cast<string>(network.getKey()));
				stream << p.cell("Nom", p.getForm().getTextInput(TransportNetworkUpdateAction::PARAMETER_NAME, network.getName()));
				stream << p.cell(
					"Parent des calendriers de jours",
					p.getForm().getSelectInput(
						TransportNetworkUpdateAction::PARAMETER_DAYS_CALENDARS_PARENT_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
						optional<RegistryKeyType>(
							network.getDaysCalendarsParent() ? network.getDaysCalendarsParent()->getKey() : RegistryKeyType(0)
						)
				)	);
				stream << p.cell(
					"Parent des calendriers de périodes",
					p.getForm().getSelectInput(
						TransportNetworkUpdateAction::PARAMETER_PERIODS_CALENDARS_PARENT_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
						optional<RegistryKeyType>(
							network.getPeriodsCalendarsParent() ? network.getPeriodsCalendarsParent()->getKey() : RegistryKeyType(0)
						)
				)	);

				stream << p.close();

				// Source id
				StaticActionRequest<TransportNetworkUpdateAction> updateOnlyRequest(_request);
				updateOnlyRequest.getAction()->setNetwork(
					Env::GetOfficialEnv().getEditableSPtr(const_cast<TransportNetwork*>(getRoot()))
				);
				ImportableAdmin::DisplayDataSourcesTab(stream, network, updateOnlyRequest);
			}

			// Sub-folders
			_displaySubFoldersList(stream, _request);

			// Folder properties
			_displayFolderProperties(stream, _request);
		}



		bool TransportNetworkAdmin::isAuthorized(
			const security::User& user
		) const	{

			// Read network
			const TransportNetwork* network(getNodeRoot());

			// Reject page if no network is readable
			if(!network)
			{
				return false;
			}

			// Network right
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(network->getKey()));
		}



		std::string TransportNetworkAdmin::getTitle() const
		{
			if(getRoot())
			{
				return getRoot()->getName(); 
			}
			if(getFolder())
			{
				return getFolder()->get<Name>();
			}
			return DEFAULT_TITLE;
		}



		AdminInterfaceElement::PageLinks TransportNetworkAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				const TreeFolderUpNode* node(getNode());

				// Folders
				vector<TreeFolder*> folders(node->getChildren<TreeFolder>());
				BOOST_FOREACH(TreeFolder* folder, folders)
				{
					shared_ptr<TransportNetworkAdmin> p(
						getNewPage<TransportNetworkAdmin>()
					);
					p->setFolder(*folder);
					links.push_back(p);
				}

				// Lines
				vector<CommercialLine*> lines(node->getChildren<CommercialLine>());
				BOOST_FOREACH(CommercialLine* line, lines)
				{
					shared_ptr<CommercialLineAdmin> p(
						getNewPage<CommercialLineAdmin>()
					);
					p->setCommercialLine(Env::GetOfficialEnv().getSPtr(line));
					links.push_back(p);
				}
			}
			return links;
		}



		bool TransportNetworkAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return
				static_cast<const TransportNetworkAdmin&>(other).getNode() &&
				getNode()->getKey() == static_cast<const TransportNetworkAdmin&>(other).getNode()->getKey()
			;
		}



		void TransportNetworkAdmin::getHTMLLineSearchForm(
			std::ostream& stream,
			const html::HTMLForm& form,
			boost::optional<util::RegistryKeyType> networkId,
			boost::optional<const std::string&> lineName
		){

			SearchFormHTMLTable s(form);
			stream << s.open();
			if(networkId)
			{
				stream << s.cell("Réseau", s.getForm().getTextInput(PARAMETER_SEARCH_NETWORK_ID, lexical_cast<string>(*networkId)));
			}
			if(lineName)
			{
				stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, *lineName));
			}
			HTMLForm sortedForm(s.getForm());
			stream << s.close();
		}



		AdminInterfaceElement::PageLinks TransportNetworkAdmin::_getCurrentTreeBranch() const
		{
			if(getFolder())
			{
				shared_ptr<TransportNetworkAdmin> p(
					getNewPage<TransportNetworkAdmin>()
				);
				p->setNode(*getFolder()->_getParent());

				PageLinks links(p->_getCurrentTreeBranch());
				links.push_back(getNewCopiedPage());
				return links;
			}
			else
			{
				shared_ptr<PTNetworksAdmin> p(
					getNewPage<PTNetworksAdmin>()
				);
				
				PageLinks links(p->_getCurrentTreeBranch());
				links.push_back(getNewCopiedPage());
				return links;
			}
			return PageLinks();
		}
}	}
