
/** TimetableAdmin class implementation.
	@file TimetableAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "TimetableAdmin.h"
#include "TimetableModule.h"
#include "TimetableGenerateFunction.h"
#include "PropertiesHTMLTable.h"
#include "ActionResultHTMLTable.h"
#include "HTMLModule.h"
#include "CalendarModule.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"
#include "Calendar.h"
#include "Line.h"
#include "CommercialLine.h"
#include "CalendarTemplateTableSync.h"
#include "Timetable.h"
#include "TimetableRight.h"
#include "TimetableTableSync.h"
#include "TimetableRow.h"
#include "TimetableRowTableSync.h"
#include "TimetableUpdateAction.h"
#include "TimetableRowAddAction.h"
#include "TimetableRowDeleteAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "InterfaceTableSync.h"
#include "TimetableInterfacePage.h"
#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace timetables;
	using namespace html;
	using namespace db;
	using namespace calendar;
	using namespace security;
	using namespace env;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TimetableAdmin>::FACTORY_KEY("TimetableAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TimetableAdmin>::ICON("table.png");
		template<> const string AdminInterfaceElementTemplate<TimetableAdmin>::DEFAULT_TITLE("Fiche horaire inconnue");
	}

	namespace timetables
	{
		const string TimetableAdmin::PARAMETER_RANK("rk");
		const string TimetableAdmin::TAB_CONTENT("co");
		const string TimetableAdmin::TAB_PROPERTIES("pr");
		const string TimetableAdmin::TAB_RESULT("re");

		TimetableAdmin::TimetableAdmin()
			: AdminInterfaceElementTemplate<TimetableAdmin>()
		{ }
		
		void TimetableAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			if(objectWillBeCreatedLater) return;
			
			try
			{
				_timetable = TimetableTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv()
				);
			}
			catch(...)
			{
				throw AdminParametersException("No such timetable");
			}
			if (_timetable->getIsBook())
				throw AdminParametersException("Timetable is document");
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_RANK);
		}
		
		
		
		server::ParametersMap TimetableAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_timetable.get()) m.insert(Request::PARAMETER_OBJECT_ID, _timetable->getKey());
			return m;
		}
		
		
		
		void TimetableAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				// Requests
				AdminActionFunctionRequest<TimetableUpdateAction,TimetableAdmin> updateRequest(_request);
				updateRequest.getAction()->setTimetable(const_pointer_cast<Timetable>(_timetable));

				// Display
				stream << "<h1>Propri�t�s</h1>";

				PropertiesHTMLTable pt(updateRequest.getHTMLForm("update"));
				stream << pt.open();
				stream << pt.cell(
					"Calendrier",
					pt.getForm().getSelectInput(
						TimetableUpdateAction::PARAMETER_BASE_CALENDAR_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList(),
						_timetable->getBaseCalendar() ? _timetable->getBaseCalendar()->getKey() : RegistryKeyType(0)
				)	);
				stream << pt.cell(
					"Saut de page avant",
					pt.getForm().getOuiNonRadioInput(
						TimetableUpdateAction::PARAMETER_MUST_BEGIN_A_PAGE,
						_timetable->getMustBeginAPage()
				)	);
				stream << pt.cell(
					"Titre",
					pt.getForm().getTextInput(
						TimetableUpdateAction::PARAMETER_TITLE,
						_timetable->getTitle()
				)	);
				stream << pt.cell(
					"Interface",
					pt.getForm().getSelectInput(
						TimetableUpdateAction::PARAMETER_INTERFACE_ID,
						InterfaceTableSync::GetInterfaceLabels<TimetableInterfacePage>(optional<string>()),
						_timetable->getInterface() ? _timetable->getInterface()->getKey() : RegistryKeyType(0)
				)	);
				stream << pt.cell(
					"Format",
					pt.getForm().getRadioInputCollection(
						TimetableUpdateAction::PARAMETER_FORMAT,
						Timetable::GetFormatsList(),
						_timetable->getFormat(),
						true
				)	);

				stream << pt.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB CONTENT
			if (openTabContent(stream, TAB_CONTENT))
			{
				AdminActionFunctionRequest<TimetableRowAddAction,TimetableAdmin> addRowRequest(_request);
				addRowRequest.getAction()->setTimetable(_timetable);

				AdminActionFunctionRequest<TimetableRowDeleteAction,TimetableAdmin> deleteRowRequest(_request);

				AdminFunctionRequest<TimetableAdmin> searchRequest(_request);

				stream << "<h1>Arr�ts</h1>";

				// Search
				TimetableRowTableSync::SearchResult rows(
					TimetableRowTableSync::Search(
						_getEnv(),
						_timetable->getKey()
						, _requestParameters.orderField == PARAMETER_RANK
						, _requestParameters.raisingOrder
						, _requestParameters.first
						, _requestParameters.maxSize,
						UP_LINKS_LOAD_LEVEL
				)	);
				
				ActionResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(string(), HTMLModule::getHTMLImage("arrow_up.png", "^")));
				h.push_back(make_pair(string(), HTMLModule::getHTMLImage("arrow_down.png", "V")));
				h.push_back(make_pair(PARAMETER_RANK, "Rang"));
				h.push_back(make_pair(string(), "Commune"));
				h.push_back(make_pair(string(), "Arr�t"));
				h.push_back(make_pair(string(), "Arr"));
				h.push_back(make_pair(string(), "Dep"));
				h.push_back(make_pair(string(), "Obl"));
				h.push_back(make_pair(string(), "Sel"));
				h.push_back(make_pair(string(), "Aff"));
				h.push_back(make_pair(string(), "Action"));
				ActionResultHTMLTable t(
					h,
					searchRequest.getHTMLForm(),
					_requestParameters,
					rows,
					addRowRequest.getHTMLForm("addrow"),
					TimetableRowAddAction::PARAMETER_RANK
				);

				stream << t.open();

				int maxRank(TimetableRowTableSync::GetMaxRank(_timetable->getKey()));
				int lastRank(UNKNOWN_VALUE);
				BOOST_FOREACH(shared_ptr<TimetableRow> row, rows)
				{
					lastRank = row->getRank();
					deleteRowRequest.getAction()->setElement(const_pointer_cast<const TimetableRow>(row));

					stream << t.row(Conversion::ToString(lastRank));
					stream << t.col();
					if (lastRank > 0)
						stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("arrow_up.png", "^"));
					stream << t.col();
					if (lastRank < maxRank)
						stream << HTMLModule::getHTMLLink(string(), HTMLModule::getHTMLImage("arrow_down.png", "V"));;
					stream << t.col() << lastRank;
					stream << t.col() << row->getPlace()->getCity()->getName();
					stream << t.col() << row->getPlace()->getName();
					stream <<
						t.col() <<
						(	row->getIsArrival() ?
							HTMLModule::getHTMLImage("bullet_green.png","Arriv�e possible") :
							HTMLModule::getHTMLImage("bullet_white.png", "Arriv�e impossible")
						)
					;
					stream <<
						t.col() <<
						(	row->getIsDeparture() ?
							HTMLModule::getHTMLImage("bullet_green.png", "D�part possible") :
							HTMLModule::getHTMLImage("bullet_white.png", "D�part impossible")
						)
					;
					stream <<
						t.col() <<
						(	(row->getCompulsory() == PassageObligatoire) ?
							HTMLModule::getHTMLImage("bullet_green.png", "Obligatoire") :
							HTMLModule::getHTMLImage("bullet_white.png", "Non obligatoire")
						)
					;
					stream <<
						t.col() <<
						(	(row->getCompulsory() == PassageSuffisant) ?
							HTMLModule::getHTMLImage("bullet_green.png", "Suffisant") :
							HTMLModule::getHTMLImage("bullet_white.png", "Non suffisant")
						)
					;
					stream <<
						t.col() <<
						(	(false) ?
							HTMLModule::getHTMLImage("bullet_green.png", "Affich�") :
							HTMLModule::getHTMLImage("bullet_white.png", "Non affich�")
						)
					;
					stream << t.col() << HTMLModule::getLinkButton(deleteRowRequest.getURL(), "Supprimer", "Etes-vous s�r de vouloir supprimer l'arr�t ?");
				}
				stream << t.row(Conversion::ToString(UNKNOWN_VALUE));
				stream << t.col();
				stream << t.col();
				stream << t.col() << ++lastRank;
				stream << t.col() << t.getActionForm().getTextInput(TimetableRowAddAction::PARAMETER_CITY_NAME, string(), "(commune)");
				stream << t.col() << t.getActionForm().getTextInput(TimetableRowAddAction::PARAMETER_PLACE_NAME, string(), "(arr�t)");
				stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_ARRIVAL, Conversion::ToString(true), true);
				stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_DEPARTURE, Conversion::ToString(true), true);
				stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_COMPULSORY, Conversion::ToString(true), false);
				stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_SUFFICIENT, Conversion::ToString(true), true);
				stream << t.col() << t.getActionForm().getCheckBox(TimetableRowAddAction::PARAMETER_IS_DISPLAYED, Conversion::ToString(true), true);
				stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");

				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB RESULT
			if (openTabContent(stream, TAB_RESULT))
			{
				if(_timetable->getInterface())
				{
					FunctionRequest<TimetableGenerateFunction> viewRequest(&_request);
					viewRequest.getFunction()->setTimetable(_timetable);
					if(	!_timetable->getInterface()->getDefaultClientURL().empty()
					){
						viewRequest.setClientURL(_timetable->getInterface()->getDefaultClientURL());
					}

					stream << "<h1>R�sultat final</h1>";
					stream << "<p>" << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), ICON) << "</p>";				}

				stream << "<h1>Tableau</h1>";

				auto_ptr<TimetableGenerator> g(_timetable->getGenerator(Env::GetOfficialEnv()));
				g->build();

				HTMLTable tf(0, ResultHTMLTable::CSS_CLASS);
				stream << tf.open();
				stream << tf.row();
				stream << tf.col(1, string(), true) << "Lignes";

				BOOST_FOREACH(const Line* line, g->getLines())
				{
					stream <<
						tf.col(1, line->getCommercialLine()->getStyle()) <<
						line->getCommercialLine()->getShortName()
						;
				}

				const TimetableGenerator::Rows& grows(g->getRows());
				for (TimetableGenerator::Rows::const_iterator it(grows.begin()); it !=grows.end(); ++it)
				{
					stream << tf.row();
					stream << tf.col(1, string(), true) << it->getPlace()->getFullName();
					vector<Schedule> cols(g->getSchedulesByRow(it));
					for (vector<Schedule>::const_iterator its(cols.begin()); its != cols.end(); ++its)
					{
						stream << tf.col();
						if (!its->getHour().isUnknown())
							stream << its->toString();
					}
				}

				// Notes
				stream << tf.row();
				stream << tf.col(1, string(), true) << "Renvois";
				BOOST_FOREACH(const TimetableGenerator::ColumnWarnings::value_type& warn, g->getColumnsWarnings())
				{
					stream << tf.col();
					if(warn != 0)
					{
						stream << warn;
					}
				}

				stream << tf.close();

				if(	!g->getWarnings().empty()
				){
					stream << "<h1>Renvois</h1>";

					HTMLTable::ColsVector v;
					v.push_back("Num");
					v.push_back("Texte");
					HTMLTable tw(v, ResultHTMLTable::CSS_CLASS);
					stream << tw.open();

					BOOST_FOREACH(const TimetableGenerator::Warnings::value_type& warn, g->getWarnings())
					{
						stream << tw.row();
						stream << tw.col() << warn.first;
						stream << tw.col() << warn.second.getText();
					}

					stream << tw.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}

		bool TimetableAdmin::isAuthorized(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{
			return _request.isAuthorized<TimetableRight>(READ);
		}
		


		std::string TimetableAdmin::getTitle() const
		{
			return _timetable.get() ? _timetable->getTitle() : DEFAULT_TITLE;
		}
		
		void TimetableAdmin::setTimetable(boost::shared_ptr<Timetable> timetable)
		{
			_timetable = const_pointer_cast<const Timetable, Timetable>(timetable);
		}
	
		boost::shared_ptr<const Timetable> TimetableAdmin::getTimetable() const
		{
			return _timetable;
		}


		bool TimetableAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _timetable == static_cast<const TimetableAdmin&>(other)._timetable;
		}



		void TimetableAdmin::_buildTabs( const server::FunctionRequest<admin::AdminRequest>& request ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Propri�t�s", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("Contenu", TAB_CONTENT, true));
			if(_timetable->isGenerable()) _tabs.push_back(Tab("R�sultat", TAB_RESULT, true));

			_tabBuilded = true;
		}

	}
}
