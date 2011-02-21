
/** HeuresFileFormat class implementation.
	@file HeuresFileFormat.cpp

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

#include "HeuresFileFormat.hpp"
#include "DataSource.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "City.h"
#include "CityTableSync.h"
#include "SQLiteTransaction.h"
#include "JourneyPatternTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "CommercialLineTableSync.h"
#include "LineStopTableSync.h"
#include "Calendar.h"
#include "ImportFunction.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "ImportableTableSync.hpp"
#include "PTFileFormat.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::algorithm;



namespace synthese
{
	using namespace util;
	using namespace impex;
	using namespace pt;
	using namespace road;
	using namespace admin;
	using namespace geography;
	using namespace db;
	using namespace graph;
	using namespace calendar;
	using namespace server;
	using namespace html;
	
	

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HeuresFileFormat>::FACTORY_KEY("Heures");
	}

	namespace pt
	{
		const std::string HeuresFileFormat::Importer_::FILE_ITINERAI("1itinerai"); 
		const std::string HeuresFileFormat::Importer_::FILE_TRONCONS("2troncons");
		const std::string HeuresFileFormat::Importer_::FILE_SERVICES("3services");
		const std::string HeuresFileFormat::Importer_::PARAMETER_END_DATE("ed"); 
		const std::string HeuresFileFormat::Importer_::PARAMETER_START_DATE("sd");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HeuresFileFormat>::Files MultipleFileTypesImporter<HeuresFileFormat>::FILES(
			HeuresFileFormat::Importer_::FILE_ITINERAI.c_str(),
			HeuresFileFormat::Importer_::FILE_TRONCONS.c_str(),
			HeuresFileFormat::Importer_::FILE_SERVICES.c_str(),
		"");
	}

	namespace pt
	{
		bool HeuresFileFormat::Importer_::_controlPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ITINERAI));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_TRONCONS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_SERVICES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}

		
		
		SQLiteTransaction HeuresFileFormat::Importer_::_save() const
		{
			SQLiteTransaction transaction;
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				if(service.second->empty())
				{
					ScheduledServiceTableSync::Remove(service.second->getKey(), transaction);
				}
				else
				{
					ScheduledServiceTableSync::Save(service.second.get(), transaction);
				}
			}
			return transaction;
		}



		bool HeuresFileFormat::Importer_::_parse(
			const path& filePath,
			std::ostream& os,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}


			// 1 : Routes
			if(key == FILE_ITINERAI)
			{
				// Load of the stops
				ImportableTableSync::ObjectBySource<StopPointTableSync> stops(_dataSource, _env);
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);

				// Parsing the file
				string line;
				CommercialLine* cline(NULL);
				while(getline(inFile, line))
				{
					// Length control
					if(line.length() < 30)
					{
						continue;
					}

					// Route type
					int routeType(lexical_cast<int>(line.substr(7,1)));
					if(routeType != 0 || routeType != 1)
					{
						continue;
					}
					
					// Commercial line number
					int commercialLineNumber(lexical_cast<int>(trim_copy(line.substr(0, 4))));
					if(!lines.contains(lexical_cast<string>(commercialLineNumber)))
					{
						os << "WARN : commercial line with key " << commercialLineNumber << "not found<br />";
						continue;
					}
					if(lines.get(lexical_cast<string>(commercialLineNumber)).size() > 1)
					{
						os << "WARN : more than one commercial line with key " << commercialLineNumber << "<br />";
					}
					cline = *lines.get(lexical_cast<string>(commercialLineNumber)).begin();

					os << "LOAD : use of existing commercial line" << cline->getKey() << " (" << cline->getName() << ")<br />";

					// Load of existing routes
					JourneyPatternTableSync::SearchResult sroutes(
						JourneyPatternTableSync::Search(_env, cline->getKey())
					);
					BOOST_FOREACH(shared_ptr<JourneyPattern> sroute, sroutes)
					{
						LineStopTableSync::Search(
							_env,
							sroute->getKey(),
							optional<RegistryKeyType>(),
							0,
							optional<size_t>(),
							true, true,
							UP_LINKS_LOAD_LEVEL
						);
						ScheduledServiceTableSync::Search(
							_env,
							sroute->getKey(),
							optional<RegistryKeyType>(),
							optional<RegistryKeyType>(),
							optional<string>(),
							false,
							0, optional<size_t>(), true, true,
							UP_LINKS_LOAD_LEVEL
						);
					}
				
					// Stops
					JourneyPattern::StopsWithDepartureArrivalAuthorization servedStops;
					Edge::MetricOffset distance(0);
					bool ignoreRoute(false);
					for(size_t i(11); i<line.size(); i += 10)
					{
						if(line.length() < i+10)
						{
							os << "WARN : inconsistent line size " << line << "<br />";
							ignoreRoute = true;
							break;
						}

						// Stop search
						string stopNumber(trim_copy(line.substr(i,4)));
						
						// Fake stop : girouette
						if(lexical_cast<int>(stopNumber) > 9000)
						{
							*servedStops.rbegin()->_metricOffset += lexical_cast<Edge::MetricOffset>(trim_copy(line.substr(i+5,5)));
							continue;
						}

						bool regul(lexical_cast<bool>(line.substr(i+4,1)));
						distance += lexical_cast<Edge::MetricOffset>(trim_copy(line.substr(i+5,5)));

						if(!stops.contains(stopNumber))
						{
							os << "WARN : stop " << stopNumber << " not found<br />";
							ignoreRoute = true;
						}

						servedStops.push_back(
							JourneyPattern::StopWithDepartureArrivalAuthorization(
								stops.get(stopNumber),
								distance,
								true,
								true,
								regul
						)	);
					}

					if(ignoreRoute)
					{
						continue;
					}

					// Route number
					string routeNumber(trim_copy(line.substr(6,2)));
					
					// Route identification
					JourneyPattern* route(
						PTFileFormat::CreateOrUpdateRoute(
							*cline,
							optional<const string&>(routeNumber),
							optional<const string&>(),
							optional<const string&>(),
							true,
							Env::GetOfficialEnv().getEditable<RollingStock>(13792273858822585).get(),
							servedStops,
							_dataSource,
							_env,
							os
					)	);

					_routes.insert(
						make_pair(
							make_pair(commercialLineNumber, routeNumber),
							route
					)	);
				}
			} // 2 : Nodes
			else if(key == FILE_TRONCONS)
			{
				string line;
				typedef map<
					pair<JourneyPattern*, string>, // string is service number
					pair<ScheduledService::Schedules, ScheduledService::Schedules> // departure / arrival schedules
				> SchedulesMap;
				SchedulesMap services;
				vector<shared_ptr<ScheduledService> > createdServices;

				// Reading of the file
				while(getline(inFile, line))
				{
					for(size_t i(29); i<line.size(); ++i)
					{
						string routeNumber(trim_copy(line.substr(i,2)));
						int lineNumber(lexical_cast<int>(trim_copy(line.substr(i+2,4))));
						RoutesMap::iterator it(_routes.find(make_pair(lineNumber, routeNumber)));
						if(it == _routes.end())
						{
							os << "WARN : route not found in service file " << lineNumber << "/" << routeNumber << "<br />";
							for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
							continue;
						}
						JourneyPattern* route(it->second);

						string serviceNumber(trim_copy(line.substr(i+8,3)));
						SchedulesMap::iterator itS(services.find(make_pair(route, serviceNumber)));
						if(itS != services.end())
						{
							if(itS->first.first != route)
							{
								os << "WARN : inconsistent route in service file " << serviceNumber << "/" << lineNumber << "/" << routeNumber << "<br />";
								for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
								continue;
							}
						}
						else
						{
							itS = services.insert(
								make_pair(
									make_pair(route, serviceNumber),
									SchedulesMap::mapped_type()
							)	).first;
							size_t schedulesNumber(route->getScheduledStopsNumber());
							for(size_t s(0); s<schedulesNumber; ++s)
							{
								itS->second.first.push_back(time_duration(not_a_date_time));
								itS->second.second.push_back(time_duration(not_a_date_time));
							}
						}

						// Read the available schedules
						size_t rank(0);
						for(i+=11; i<line.size() && line[i]!=';'; i+=8, ++rank)
						{
							string arrivalSchedule(line.substr(i, 4));
							string departureSchedule(line.substr(i+4, 4));

							if(departureSchedule != "9999")
							{
								itS->second.first[rank] = time_duration(
									lexical_cast<int>(departureSchedule.substr(0,2)), 
									lexical_cast<int>(departureSchedule.substr(2,2)),
									0
								);
							}
							if(arrivalSchedule != "9999")
							{
								itS->second.second[rank] = time_duration(
									lexical_cast<int>(arrivalSchedule.substr(0,2)),
									lexical_cast<int>(arrivalSchedule.substr(2,2)),
									0
								);
							}
						}
				}	}

				// Storage as ScheduledService
				BOOST_FOREACH(const SchedulesMap::value_type& it, services)
				{
					typedef map<
						pair<JourneyPattern*, string>, // string is service number
						pair<ScheduledService::Schedules, ScheduledService::Schedules> // departure / arrival schedules
					> SchedulesMap;

					JourneyPattern* route(it.first.first);

					ScheduledService* service(
						PTFileFormat::CreateOrUpdateService(
							*route,
							it.second.first,
							it.second.second,
							it.first.second,
							_dataSource,
							_env,
							os
					)	);

					_services.insert(
						make_pair(
							make_pair(
								lexical_cast<int>(route->getCodeBySource(_dataSource)),
								service->getServiceNumber()
							),
							service
					)	);
				}
			} // 3 : Services
			else if (key == FILE_SERVICES)
			{
				string line;
				while(getline(inFile, line))
				{
					// Read of calendar
					vector<bool> days;
					for(size_t i(0); i<7; ++i)
					{
						days[i] = (line[i==0 ? 12 : i+5] == '1');
					}
					Calendar cal;
					for(gregorian::date d(_startDate); d<=_endDate; d += gregorian::days(1))
					{
						if(days[d.day_of_week()])
						{
							cal.setActive(d);
						}
					}
					
					// Services list
					for(size_t i(13); i<line.size(); i+=29)
					{
						int lineNumber(lexical_cast<int>(trim_copy(line.substr(i,3))));
						string serviceNumber(trim_copy(line.substr(i+3,3)));

						ServicesMap::iterator itS(_services.find(
							make_pair(lineNumber, serviceNumber)
						)	);
						if(itS == _services.end())
						{
							os << "WARN : inconsistent service number " << line << "<br />";
							continue;
						}

						ScheduledService* service(itS->second);

						service->copyDates(cal);
					}
				}
			}
			inFile.close();

			return true;
		}



		void HeuresFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const AdminRequest& request
		) const {

			stream << "<h1>Horaires</h1>";
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Propri�t�s");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Date d�but", t.getForm().getCalendarInput(PARAMETER_START_DATE, _startDate));
			stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAMETER_END_DATE, _endDate));
			stream << t.title("Donn�es");
			stream << t.cell("Itinerai", t.getForm().getTextInput(_getFileParameterName(FILE_ITINERAI), _pathsMap[FILE_ITINERAI].file_string()));
			stream << t.cell("Troncons", t.getForm().getTextInput(_getFileParameterName(FILE_TRONCONS), _pathsMap[FILE_TRONCONS].file_string()));
			stream << t.cell("Services", t.getForm().getTextInput(_getFileParameterName(FILE_SERVICES), _pathsMap[FILE_SERVICES].file_string()));
			stream << t.close();
		}
}	}
