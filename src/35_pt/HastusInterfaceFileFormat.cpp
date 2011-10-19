
/** HastusInterfaceFileFormat class implementation.
	@file HastusInterfaceFileFormat.cpp

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

#include "DataSource.h"
#include "DBModule.h"
#include "HastusInterfaceFileFormat.hpp"
#include "GraphConstants.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPoint.hpp"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "City.h"
#include "CityTableSync.h"
#include "Service.h"
#include "RollingStock.hpp"
#include "NonConcurrencyRule.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "PTUseRule.h"
#include "PTConstants.h"
#include "CoordinatesSystem.hpp"
#include "Conversion.h"
#include "XmlToolkit.h"
#include "DBTransaction.hpp"
#include "CityAliasTableSync.hpp"
#include "JunctionTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "ImportFunction.h"
#include "PropertiesHTMLTable.h"
#include "RequestException.h"
#include "AdminFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "PTFileFormat.hpp"
#include "ImpExModule.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "CalendarTemplateElementTableSync.h"
#include "CommercialLineTableSync.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>
#include <utility>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::filesystem;
using namespace boost::posix_time;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace geography;
	using namespace util::XmlToolkit;
	using namespace util;
	using namespace graph;
	using namespace impex;
	using namespace db;
	using namespace pt;
	using namespace server;
	using namespace admin;
	using namespace html;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,pt::HastusInterfaceFileFormat>::FACTORY_KEY("Trident");
	}

	namespace pt
	{
		const string HastusInterfaceFileFormat::Importer_::PARAMETER_TRANSPORT_NETWORK_ID = "network_id";


		//////////////////////////////////////////////////////////////////////////
		// CONSTRUCTOR
		HastusInterfaceFileFormat::Importer_::Importer_(
			Env& env,
			const DataSource& dataSource
		):	OneFileTypeImporter<Importer_>(env, dataSource),
			Importer(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource)
		{}



		//////////////////////////////////////////////////////////////////////////
		// REQUESTS HANDLING
		util::ParametersMap HastusInterfaceFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result(PTDataCleanerFileFormat::_getParametersMap());
			return result;
		}



		void HastusInterfaceFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
		}

		//////////////////////////////////////////////////////////////////////////
		// INPUT

		bool HastusInterfaceFileFormat::Importer_::_parse(
			const path& filePath,
			ostream& os,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {

			_file.open(filePath.file_string().c_str());
			if(!_file)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}

			// Record 1.1 : Lines
			ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);
			for(_loadNextRecord(); _record.recordNumber == 1.1; _loadNextRecord())
			{
				vector<string> lineNumbers(_getVectorField(21, 5));
				BOOST_FOREACH(const string& lineNumber, lineNumbers)
				{
//					PTFileFormat::CreateOrUpdateLine(
//						lines,
//						lineNumber,
				}
			}


			return true;
		}



		DBTransaction HastusInterfaceFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			_addRemoveQueries(transaction);

			// Saving of each created or altered objects
/*			if(_importStops)
			{
				BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
				{
					StopAreaTableSync::Save(cstop.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
				{
					StopPointTableSync::Save(stop.second.get(), transaction);
				}
			}
*/			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DesignatedLinePhysicalStop>::value_type lineStop, _env.getRegistry<DesignatedLinePhysicalStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Junction>::value_type& junction, _env.getRegistry<Junction>())
			{
				JunctionTableSync::Save(junction.second.get(), transaction);
			}
/*			if(_importTimetablesAsTemplates)
			{
				BOOST_FOREACH(shared_ptr<CalendarTemplateElement> element, _calendarElementsToRemove)
				{
					CalendarTemplateElementTableSync::RemoveRow(element->getKey(), transaction);
				}
				BOOST_FOREACH(const Registry<CalendarTemplate>::value_type& calendarTemplate, _env.getRegistry<CalendarTemplate>())
				{
					CalendarTemplateTableSync::Save(calendarTemplate.second.get(), transaction);
				}
				BOOST_FOREACH(const Registry<CalendarTemplateElement>::value_type& calendarTemplateElement, _env.getRegistry<CalendarTemplateElement>())
				{
					CalendarTemplateElementTableSync::Save(calendarTemplateElement.second.get(), transaction);
				}
			}
*/			return transaction;
		}



		//////////////////////////////////////////////////////////////////////////
		// HELPERS
		void HastusInterfaceFileFormat::Importer_::_loadNextRecord(
		) const {
			string line;
			if(!getline(_file, line))
			{
				_record.recordNumber = 0;
				return;
			}
			if(line.size() >= 5 && line.substr(0,5) != "     ")
			{
				_record.recordNumber = lexical_cast<size_t>(trim_copy(line.substr(0,5)));
			}
			_record.content = line;
		}



		std::string synthese::pt::HastusInterfaceFileFormat::Importer_::_getTextField( std::size_t start, std::size_t length ) const
		{
			if(_record.content.size() < start)
			{
				return string();
			}
			if(_record.content.size() < start+length)
			{
				return trim_copy(_record.content.substr(start));
			}
			return trim_copy(_record.content.substr(start, length));
		}



		std::vector<std::string> synthese::pt::HastusInterfaceFileFormat::Importer_::_getVectorField( std::size_t start, std::size_t length ) const
		{
			vector<string> result;
			if(_record.content.size() < start)
			{
				return result;
			}
			while(_record.content.size() >= start)
			{
				if(_record.content.size() < start+length)
				{
					result.push_back(trim_copy(_record.content.substr(start)));
					break;
				}
				else
				{
					result.push_back(trim_copy(_record.content.substr(start, length)));
				}
				start += length;
			}
			return result;
		}



		void synthese::pt::HastusInterfaceFileFormat::Importer_::_loadRecordOfType(std::size_t recordNumber, std::size_t nextRecordNumber) const
		{
			for(_loadNextRecord(); _record.recordNumber == 0 || _record.recordNumber == recordNumber; _loadNextRecord()) ;
		}



		void HastusInterfaceFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Effacer donn�es anciennes", t.getForm().getOuiNonRadioInput(PARAMETER_CLEAN_OLD_DATA, false));
			stream << t.title("Param�tres");
//			stream << t.cell("R�seau", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.title("Donn�es (remplir un des deux champs)");
			stream << t.cell("Ligne", t.getForm().getTextInput(PARAMETER_PATH, (_pathsSet.size() == 1) ? _pathsSet.begin()->file_string() : string()));
			stream << t.cell("R�pertoire", t.getForm().getTextInput(PARAMETER_DIRECTORY, _dirPath.file_string()));
			stream << t.close();
		}
}	}
