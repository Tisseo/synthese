
/** GTFSFileFormat class implementation.
	@file GTFSFileFormat.cpp

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

#include "GTFSFileFormat.hpp"
#include "TransportNetwork.h"
#include "StopArea.hpp"
#include "PTFileFormat.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ImpExModule.h"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "StopPointAdmin.hpp"
#include "StopAreaAddAction.h"
#include "RollingStock.h"
#include "StopArea.hpp"
#include "DataSource.h"
#include "ImpExModule.h"
#include "Importer.hpp"
#include "StopPointMoveAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "GTFSFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "Junction.hpp"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"

#include "StopPoint.hpp"
#include "RequestException.h"
#include "TransportNetworkTableSync.h"
#include "ContinuousService.h"

#include "ServerModule.h"
#include "stdlib.h"
/*
#include "CommercialLine.h"
#include "Path.h"
#include "JourneyPattern.hpp"*/

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace calendar;
	using namespace graph;
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace geography;
	
	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,GTFSFileFormat>::FACTORY_KEY("GTFS");
	}
	
	namespace pt
	{
		const std::string GTFSFileFormat::Importer_::FILE_STOPS("stops");
		const std::string GTFSFileFormat::Importer_::FILE_TRANSFERS("transfers");
		const std::string GTFSFileFormat::Importer_::FILE_AGENCY("agency");
		const std::string GTFSFileFormat::Importer_::FILE_ROUTES("routes");
		const std::string GTFSFileFormat::Importer_::FILE_CALENDAR("calendar");
		const std::string GTFSFileFormat::Importer_::FILE_CALENDAR_DATES("calendar_dates");
		const std::string GTFSFileFormat::Importer_::FILE_TRIPS("trips");
		const std::string GTFSFileFormat::Importer_::FILE_STOP_TIMES("stop_times");
		const std::string GTFSFileFormat::Importer_::FILE_FARE_ATTRIBUTES("fare_attributes");
		const std::string GTFSFileFormat::Importer_::FILE_FARE_RULES("fare_rules");
		const std::string GTFSFileFormat::Importer_::FILE_SHAPES("shapes");
		const std::string GTFSFileFormat::Importer_::FILE_FREQUENCIES("frequencies");
		const std::string GTFSFileFormat::Importer_::SEP(",");

		const std::string GTFSFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string GTFSFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string GTFSFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string GTFSFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		
		const std::string GTFSFileFormat::Exporter_::PARAMETER_NETWORK_ID("ni");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<GTFSFileFormat>::Files MultipleFileTypesImporter<GTFSFileFormat>::FILES(
			GTFSFileFormat::Importer_::FILE_STOPS.c_str(),
			GTFSFileFormat::Importer_::FILE_TRANSFERS.c_str(),
			GTFSFileFormat::Importer_::FILE_AGENCY.c_str(),
			GTFSFileFormat::Importer_::FILE_ROUTES.c_str(),
			GTFSFileFormat::Importer_::FILE_CALENDAR.c_str(),
			GTFSFileFormat::Importer_::FILE_CALENDAR_DATES.c_str(),
			GTFSFileFormat::Importer_::FILE_TRIPS.c_str(),
			GTFSFileFormat::Importer_::FILE_STOP_TIMES.c_str(),
			GTFSFileFormat::Importer_::FILE_FARE_ATTRIBUTES.c_str(),
			GTFSFileFormat::Importer_::FILE_FARE_RULES.c_str(),
			GTFSFileFormat::Importer_::FILE_SHAPES.c_str(),
			GTFSFileFormat::Importer_::FILE_FREQUENCIES.c_str(),
		"");
	}


	namespace pt
	{
		bool GTFSFileFormat::Importer_::_controlPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_AGENCY));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_STOPS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ROUTES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_TRIPS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_STOP_TIMES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CALENDAR);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		GTFSFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<GTFSFileFormat>(env, dataSource),
			_importStopArea(false),
			_interactive(false),
			_displayLinkedStops(false),
			_networks(_dataSource, _env),
			_stopPoints(_dataSource, _env),
			_lines(_dataSource, _env)
		{}



		bool GTFSFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> request
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
			string line;
			if(!getline(inFile, line))
			{
				return false;
			}
			_loadFieldsMap(line);

			// 1 : Routes
			// Stops
			if(key == FILE_STOPS)
			{
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(_dataSource, _env);

				// 2.1 : stop areas
				if(_importStopArea)
				{
					PTFileFormat::ImportableStopAreas linkedStopAreas;
					PTFileFormat::ImportableStopAreas nonLinkedStopAreas;

					// Loop
					while(getline(inFile, line))
					{
						_loadLine(line);
						if(_getValue("location_type") != "1")
						{
							continue;
						}

						string id(_getValue("stop_id"));
						string name(_getValue("stop_name"));

						if(request)
						{
							PTFileFormat::ImportableStopArea isa;
							isa.operatorCode = id;
							isa.name = name;
							isa.linkedStopAreas = stopAreas.get(id);

							if(isa.linkedStopAreas.empty())
							{
								nonLinkedStopAreas.push_back(isa);
							}
							else if(_displayLinkedStops)
							{
								linkedStopAreas.push_back(isa);
							}
						}
						else
						{
							PTFileFormat::CreateOrUpdateStopAreas(
								stopAreas,
								id,
								name,
								*_defaultCity,
								_stopAreaDefaultTransferDuration,
								_dataSource,
								_env,
								stream
							);
						}
					}

					if(request)
					{
						PTFileFormat::DisplayStopAreaImportScreen(
							nonLinkedStopAreas,
							*request,
							true,
							false,
							_defaultCity,
							_env,
							_dataSource,
							stream
						);
						if(_displayLinkedStops)
						{
							PTFileFormat::DisplayStopAreaImportScreen(
								linkedStopAreas,
								*request,
								true,
								false,
								_defaultCity,
								_env,
								_dataSource,
								stream
								);
						}
					}
				}

				// 2.2 : stops
				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

				// Loop
				inFile.clear();
				inFile.seekg(0, ios::beg);
				getline(inFile, line);
				while(getline(inFile, line))
				{
					_loadLine(line);
					if(_getValue("location_type") != "0")
					{
						continue;
					}

					string id(_getValue("stop_id"));
					string name(_getValue("stop_name"));

					// Stop area
					string stopAreaId(_getValue("parent_station"));
					const StopArea* stopArea(NULL);
					if(stopAreas.contains(stopAreaId))
					{
						stopArea = *stopAreas.get(stopAreaId).begin();
					}
					else if(_stopPoints.contains(id))
					{
						stopArea = (*_stopPoints.get(id).begin())->getConnectionPlace();
					}
					else
					{
						stream << "WARN : inconsistent stop area id "<< stopAreaId <<" in the stop point "<< id <<"<br />";
						continue;
					}

					// Point
					shared_ptr<geos::geom::Point> point(
						CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
							lexical_cast<double>(_getValue("stop_lon")),
							lexical_cast<double>(_getValue("stop_lat"))
					)	);
					if(point->isEmpty())
					{
						point.reset();
					}

					if(request)
					{
						PTFileFormat::ImportableStopPoint isp;
						isp.operatorCode = id;
						isp.name = name;
						isp.linkedStopPoints = _stopPoints.get(id);
						isp.stopArea = stopArea;
						isp.coords = point;

						if(isp.linkedStopPoints.empty())
						{
							nonLinkedStopPoints.push_back(isp);
						}
						else if(_displayLinkedStops)
						{
							linkedStopPoints.push_back(isp);
						}
					}
					else
					{
						// Creation or update
						PTFileFormat::CreateOrUpdateStopPoints(
							_stopPoints,
							id,
							name,
							stopArea,
							point.get(),
							NULL,
							optional<time_duration>(),
							_dataSource,
							_env,
							stream
						);
					}
				}

				if(request)
				{
					PTFileFormat::DisplayStopPointImportScreen(
						nonLinkedStopPoints,
						*request,
						_env,
						_dataSource,
						stream
					);
					if(_displayLinkedStops)
					{
						PTFileFormat::DisplayStopPointImportScreen(
							linkedStopPoints,
							*request,
							_env,
							_dataSource,
							stream
						);
					}
				}
			}
			else if(key == FILE_TRANSFERS)
			{
				//TODO
			}
			else if(key == FILE_AGENCY)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					PTFileFormat::CreateOrUpdateNetwork(
						_networks,
						_getValue("agency_id"),
						_getValue("agency_name"),
						_dataSource,
						_env,
						stream
					);
				}
			}
			// 3 : Lines
			else if(key == FILE_ROUTES)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					// Network
					string networkId(_getValue("agency_id"));
					string id(_getValue("route_id"));
					const TransportNetwork* network;
					if(_networks.contains(networkId))
					{
						network = *_networks.get(networkId).begin();
					}
					else if(_lines.contains(id))
					{
						network = (*_lines.get(id).begin())->getNetwork();
					}
					else
					{
						stream << "WARN : inconsistent network id "<< networkId <<" in the line "<< id <<"<br />";
						continue;
					}

					PTFileFormat::CreateOrUpdateLine(
						_lines,
						id,
						_getValue("route_long_name"),
						_getValue("route_short_name"),
						_getValue("route_color").size() == 7 ? optional<RGBColor>(RGBColor::FromXMLColor(_getValue("route_color"))) : optional<RGBColor>(),
						*network,
						_dataSource,
						_env,
						stream
					);
				}
			}
			// 4 : Calendars
			else if(key == FILE_CALENDAR)
			{
				vector<string> week_days;
				week_days.push_back("sunday");
				week_days.push_back("monday");
				week_days.push_back("tuesday");
				week_days.push_back("wednesday");
				week_days.push_back("thursday");
				week_days.push_back("friday");
				week_days.push_back("saturday");

				while(getline(inFile, line))
				{
					_loadLine(line);

					Calendar c;

					string startDateStr(_getValue("start_date"));
					date startDate(
						lexical_cast<int>(startDateStr.substr(0,4)),
						lexical_cast<int>(startDateStr.substr(4,2)),
						lexical_cast<int>(startDateStr.substr(6,2))
					);
					string endDateStr(_getValue("end_date"));
					date endDate(
						lexical_cast<int>(endDateStr.substr(0,4)),
						lexical_cast<int>(endDateStr.substr(4,2)),
						lexical_cast<int>(endDateStr.substr(6,2))
					);
					
					for(date curDate(startDate); curDate<=endDate; curDate += days(1))
					{
						if(_getValue(week_days[curDate.day_of_week()]) == "1")
						{
							c.setActive(curDate);
						}
					}

					_calendars[_getValue("service_id")] = c;
				}
			}
			else if(key == FILE_CALENDAR_DATES) //5
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					std::map<std::string, calendar::Calendar>::iterator it(_calendars.find(_getValue("service_id")));
					if(it == _calendars.end())
					{
						it = _calendars.insert(make_pair(_getValue("service_id"), Calendar())).first;
					}

					string dateStr(_getValue("date"));
					date d(
						lexical_cast<int>(dateStr.substr(0,4)),
						lexical_cast<int>(dateStr.substr(4,2)),
						lexical_cast<int>(dateStr.substr(6,2))
					);

					if(_getValue("exception_type") == "1")
					{
						it->second.setActive(d);
					}
					else if(_getValue("exception_type") == "2")
					{
						it->second.setInactive(d);
					}
				}
			}
			// 6 : Routes / Services
			else if(key == FILE_TRIPS)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					Trip trip;
					
					// Line
					string id(_getValue("trip_id"));
					string lineCode(_getValue("route_id"));
					if(!_lines.contains(lineCode))
					{
						stream << "WARN : inconsistent line id "<< lineCode <<" in the trip "<< id <<"<br />";
						continue;
					}
					trip.line = *_lines.get(lineCode).begin();

					// Calendar
					string calendarCode(_getValue("service_id"));
					Calendars::const_iterator it(_calendars.find(calendarCode));
					if(it == _calendars.end())
					{
						stream << "WARN : inconsistent service id "<< calendarCode <<" in the trip "<< id <<"<br />";
						continue;
					}
					trip.calendar = it->second;

					// Destination
					trip.destination = _getValue("trip_headsign");

					// Direction
					trip.direction = lexical_cast<bool>(_getValue("direction_id"));

					_trips.insert(make_pair(id, trip));
				}
			}
			else if(key == FILE_STOP_TIMES)
			{
				string lastTripCode;
				TripDetailVector tripDetailVector;

				while(getline(inFile, line))
				{
					_loadLine(line);

					string tripCode(_getValue("trip_id"));
					if(tripCode != lastTripCode && !lastTripCode.empty())
					{
						// Trip
						TripsMap::const_iterator it(_trips.find(tripCode));
						if(it == _trips.end())
						{
							stream << "WARN : inconsistent trip id "<< tripCode <<" in the trip stops file<br />";
							continue;
						}
						Trip trip(it->second);

						// Route
						JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
						Edge::MetricOffset offsetSum(0);
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							offsetSum += tripStop.offsetFromLast;
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								tripStop.stop,
								offsetSum
							);
							stops.push_back(stop);
						}
						
						JourneyPattern* route(
							PTFileFormat::CreateOrUpdateRoute(
								*trip.line,
								optional<const string&>(),
								optional<const string&>(),
								optional<const string&>(trip.destination),
								trip.direction,
								NULL,
								stops,
								_dataSource,
								_env,
								stream
						)	);

						// Service
						ScheduledService::Schedules departures;
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							departures.push_back(tripStop.departureTime);
						}
						ScheduledService::Schedules arrivals;
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							arrivals.push_back(tripStop.arrivalTime);
						}

						ScheduledService* service(
							PTFileFormat::CreateOrUpdateService(
								*route,
								departures,
								arrivals,
								tripCode,
								_dataSource,
								_env,
								stream
						)	);
						*service |= trip.calendar;
						
						tripDetailVector.clear();
					}

					TripDetail tripDetail;
					tripDetail.offsetFromLast = lexical_cast<Edge::MetricOffset>(_getValue("shape_dist_traveled"));
					tripDetail.arrivalTime = duration_from_string(_getValue("arrival_time"));
					if(tripDetail.arrivalTime.seconds())
					{
						tripDetail.arrivalTime += seconds(60 - tripDetail.arrivalTime.seconds());
					}
					tripDetail.departureTime = duration_from_string(_getValue("departure_time"));
					if(tripDetail.departureTime.seconds())
					{
						tripDetail.departureTime -= seconds(tripDetail.departureTime.seconds());
					}

					string stopCode(_getValue("stop_id"));
					if(!_stopPoints.contains(stopCode))
					{
						stream << "WARN : inconsistent stop id "<< stopCode <<" in the trip "<< tripCode <<"<br />";
						continue;
					}
					tripDetail.stop = _stopPoints.get(stopCode);
					
					tripDetailVector.push_back(tripDetail);
					lastTripCode = tripCode;
				}
			}
			else if(key == FILE_FARE_ATTRIBUTES)
			{
				//TODO
			}
			else if(key == FILE_FARE_RULES)
			{
				//TODO
			}
			else if(key == FILE_SHAPES)
			{
				//TODO
			}
			else if(key == FILE_FREQUENCIES)
			{
				//TODO
			}
			return true;
		}



		void GTFSFileFormat::Importer_::displayAdmin(std::ostream& stream, const admin::AdminRequest& request) const
                {
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Fichiers");
			stream << t.cell("Fichier stops", t.getForm().getTextInput(_getFileParameterName(FILE_STOPS), _pathsMap[FILE_STOPS].file_string()));
			stream << t.cell("Fichier transfers (optionnel)", t.getForm().getTextInput(_getFileParameterName(FILE_TRANSFERS), _pathsMap[FILE_TRANSFERS].file_string()));
			stream << t.cell("Fichier réseaux", t.getForm().getTextInput(_getFileParameterName(FILE_AGENCY), _pathsMap[FILE_AGENCY].file_string()));
			stream << t.cell("Fichier routes", t.getForm().getTextInput(_getFileParameterName(FILE_ROUTES), _pathsMap[FILE_ROUTES].file_string()));
			stream << t.cell("Fichier calendriers", t.getForm().getTextInput(_getFileParameterName(FILE_CALENDAR), _pathsMap[FILE_CALENDAR].file_string()));
			stream << t.cell("Fichier dates", t.getForm().getTextInput(_getFileParameterName(FILE_CALENDAR_DATES), _pathsMap[FILE_CALENDAR_DATES].file_string()));
			stream << t.cell("Fichier voyages", t.getForm().getTextInput(_getFileParameterName(FILE_TRIPS), _pathsMap[FILE_TRIPS].file_string()));
			stream << t.cell("Fichier horaires", t.getForm().getTextInput(_getFileParameterName(FILE_STOP_TIMES), _pathsMap[FILE_STOP_TIMES].file_string()));
			stream << t.cell("Fichier attributs tarification", t.getForm().getTextInput(_getFileParameterName(FILE_FARE_ATTRIBUTES), _pathsMap[FILE_FARE_ATTRIBUTES].file_string()));
			stream << t.cell("Fichier règles tarification", t.getForm().getTextInput(_getFileParameterName(FILE_FARE_RULES), _pathsMap[FILE_FARE_RULES].file_string()));
			stream << t.cell("Fichier géométries", t.getForm().getTextInput(_getFileParameterName(FILE_SHAPES), _pathsMap[FILE_SHAPES].file_string()));
			stream << t.cell("Fichier services continus", t.getForm().getTextInput(_getFileParameterName(FILE_FREQUENCIES), _pathsMap[FILE_FREQUENCIES].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Import zones d'arrêt", t.getForm().getOuiNonRadioInput(PARAMETER_IMPORT_STOP_AREA, _importStopArea));
			stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.close();
		}



		db::SQLiteTransaction GTFSFileFormat::Importer_::_save() const
		{
			SQLiteTransaction transaction;

			// Saving of each created or altered objects
			if(_importStopArea)
			{
				BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
				{
					StopAreaTableSync::Save(cstop.second.get(), transaction);
				}
			}
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Junction>::value_type& junction, _env.getRegistry<Junction>())
			{
				JunctionTableSync::Save(junction.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<TransportNetwork>::value_type network, _env.getRegistry<TransportNetwork>())
			{
				TransportNetworkTableSync::Save(network.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
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
			return transaction;
		}



		void GTFSFileFormat::Importer_::_loadFieldsMap( const std::string& line ) const
		{
			vector<string> cols;
			split(cols, line, is_any_of(SEP));
			_fieldsMap.clear();
			size_t rank(0);
			BOOST_FOREACH(const string& col, cols)
			{
				_fieldsMap[col] = rank;
				++rank;
			}
		}



		std::string GTFSFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return trim_copy(_line[_fieldsMap[field]]);
		}



		void GTFSFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();
			string utfLine(ImpExModule::ConvertChar(line, _dataSource.getCharset(), "UTF-8"));
			split(_line, utfLine, is_any_of(SEP));
		}



		server::ParametersMap GTFSFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_IMPORT_STOP_AREA, _importStopArea);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			return map;
		}
		
		server::ParametersMap GTFSFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap result;			
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			return result;
		}



		void GTFSFileFormat::Importer_::_setFromParametersMap( const server::ParametersMap& map )
		{
			_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}
		}
		
		void GTFSFileFormat::Exporter_::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), Env::GetOfficialEnv());
			}
			catch (...)
			{
				throw RequestException("Transport network " + lexical_cast<string>(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID)) + " not found");
			}			
		}


// ------------------------ PROVINDING	ALL	FILES
 
		RegistryKeyType GTFSFileFormat::Exporter_::_gtfsKey(RegistryKeyType key,RegistryKeyType suffix = 0) const
		{
			RegistryKeyType gtfsKey;

			gtfsKey = 0;

			if(suffix)
				gtfsKey = suffix << 32;

			gtfsKey |= key & (RegistryKeyType)0xFFFFFFFF;

			return gtfsKey;
		}

		string GTFSFileFormat::Exporter_::_gtfsStr(string str)const
		{
			string sub;
			size_t rp;
			size_t lp=str.find("\"",0);
			while (lp!=string::npos)
			{
				rp=str.find("\"",lp+1);
				if(rp!=string::npos)
				{
						sub= str.substr (lp,rp);
						sub= "\"" + sub + "\"";
						str.replace(lp,rp, sub);
						lp = rp + 3;
				}
				else
				{
					string sub = "\"\"\"";
					str.replace(lp, 1, sub);
					break;
				}
				lp=str.find("\"",lp);
			}


			if((str.find(",",0)!=string::npos) || (str.find("'",0)!=string::npos))
			{	str = "\"" + str + "\"";	}

			return str;
		}

		// ------------------------ EXPORTER_::BUILD

		void GTFSFileFormat::Exporter_::build(ostream& os) const
		{
			//Get temporary directory;
			const filesystem::path tempDir(ModuleClass::GetParameter (ServerModule::MODULE_PARAM_TEMP_DIR), filesystem::native);

			// Path to each file  in temporary directory used by system
			string pathToAgencyFile			= tempDir.string() + "/agency.txt";
			string pathToStopsFile			= tempDir.string() + "/stops.txt";
			string pathToRoutesFile			= tempDir.string() + "/routes.txt";
			string pathToTripsFile 			= tempDir.string() + "/trips.txt";
			string pathToStopTimesFile 		= tempDir.string() + "/stop_times.txt";
			string pathToCalendarFile 		= tempDir.string() + "/calendar.txt";
			string pathToCalendarDatesFile	= tempDir.string() + "/calendar_dates.txt";
			string pathToZipFile 			= tempDir.string() + "/GTFS_Tisseo__3.2.1.zip";

			//Create or replace files
			ofstream agency_txt			(pathToAgencyFile.c_str(), ios::out);
			ofstream stops_txt			(pathToStopsFile.c_str(), ios::out);
			ofstream routes_txt			(pathToRoutesFile.c_str(), ios::out);
			ofstream trips_txt			(pathToTripsFile.c_str(), ios::out);
			ofstream stop_times_txt		(pathToStopTimesFile.c_str(), ios::out);
			ofstream calendar_txt		(pathToCalendarFile.c_str(), ios::out);
			ofstream calendar_dates_txt	(pathToCalendarDatesFile.c_str(), ios::out);


			//add header line to each file
			agency_txt  		<< "﻿agency_id,agency_name,agency_url,agency_timezone,agency_phone,agency_lang";
			stops_txt 			<< "stop_id,stop_code,stop_name,stop_lat,stop_lon,location_type,parent_station";
			routes_txt 			<< "route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color";
			//trips_txt 			<< "route_id,service_id,﻿trip_id,trip_headsign,direction_id,block_id,shape_id";
			trips_txt 			<< "trip_id,service_id,route_id,trip_headsign";
			stop_times_txt 		<< "﻿trip_id,stop_id,stop_sequence,arrival_time,departure_time,stop_headsign,pickup_type,drop_off_type,shape_dist_traveled";
			calendar_txt 		<< "﻿service_id,monday,tuesday,wednesday,thursday,friday,saturday,sunday,start_date,end_date";
			calendar_dates_txt 	<< "﻿service_id,date,exception_type";

			// ------------------------------------ BEGIN	AGENCY.TXT
			BOOST_FOREACH(Registry<TransportNetwork>::value_type myAgency, Env::GetOfficialEnv().getRegistry<TransportNetwork>())
			{
				//﻿agency_id,agency_name,agency_url,agency_timezone,agency_phone,agency_lang

				/*
				os << "\n" << dst_zone_abbrev();
				os << "\n" << std_zone_abbrev();
				os << "\n" << dst_zone_name();
				os << "\n" << std_zone_name();
				os << "\n" << dst_zone_abbrev();
				os << "\n" << dst_zone_abbrev();



				*/
				agency_txt  << "\n"
							<< _gtfsKey(myAgency.first) << "," 						// ﻿agency_id
							<< _gtfsStr(myAgency.second->getRuleUserName()) << "," 	// agency_name
							<< "http://www.tisseo.fr/" 						<< ","	// agency_url
							<< "Europe/Paris"								<< ","	// agency_timezone
							<< "05 61 41 70 70"								<< ","	// agency_phone
							<< "FR";												// agency_lang

				agency_txt.close();
			}
			// ------------------------------------ END	AGENCY.TXT

			// ------------------------------------ BEGIN	STOPS.TXT
			BOOST_FOREACH(
				Registry<StopPoint>::value_type itps,
				Env::GetOfficialEnv().getRegistry<StopPoint>()
			){
				const StopPoint& stopPoint(*itps.second);
				if (stopPoint.getDepartureEdges().empty() && stopPoint.getArrivalEdges().empty()) continue;

				shared_ptr<Point> gp;
				if(stopPoint.hasGeometry())
				{
					gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*stopPoint.getGeometry());
				}

				if(gp.get())
				{
					stops_txt 	<< "\n"
								<< _gtfsKey(stopPoint.getKey()) << "," 					//stop_id
								<< stopPoint.getCodeBySources() << "," 	    	//stop_code
								//(!stopPoint.getName()?stopPoint.getConnectionPlace()->getName():stopPoint.getName())
								//<< _gtfsStr(stopPoint.getName()) << "," 					//stop_name
								<< _gtfsStr(((stopPoint.getName())==""?stopPoint.getConnectionPlace()->getName():stopPoint.getName())) << "," 					//stop_name
								<< gp->getX() << "," 							//stop_lat
								<< gp->getY() << "," 							//stop_lon
								<< "0," 										//location_type
								<< _gtfsKey(stopPoint.getConnectionPlace()->getKey()); 	//parent_station
				}
			}

			BOOST_FOREACH(
				Registry<StopArea>::value_type itcp,
				Env::GetOfficialEnv().getRegistry<StopArea>()
			){
				const StopArea* connPlace(itcp.second.get());

				shared_ptr<Point> gp;
				if(connPlace->getPoint().get() && !connPlace->getPoint()->isEmpty())
				{
					gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
						*connPlace->getPoint()
					);
				}
				else //get first point coordinates
				{
					const StopArea::PhysicalStops& stops(connPlace->getPhysicalStops());
					if(!stops.empty())
					{
						if(stops.begin()->second->hasGeometry())
						{
							gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*stops.begin()->second->getGeometry());
						}
					}
				}

				if(gp.get())
				{
					stops_txt	<< "\n"
								<< _gtfsKey(connPlace->getKey()) << "," 		//stop_id
								<< ","						 		//stop_code
								<< _gtfsStr(connPlace->getName()) << ","		//stop_name
								<< gp->getX() << "," 				//stop_lat
								<< gp->getY() << "," 				//stop_lon
								<< "1,"; 							//location_type
																	//StopArea does not have parentStation !
				}
			}

			// ------------------------------------ END	STOPS.TXT

			// ------------------------------------ BEGIN	STOP_TIMES.TXT

			BOOST_FOREACH(Registry<ScheduledService>::value_type itsdsrv, Env::GetOfficialEnv().getRegistry<ScheduledService>())
			{
				const ScheduledService* sdsrv(itsdsrv.second.get());

				LineStopTableSync::SearchResult linestops(LineStopTableSync::Search(_env, sdsrv->getPathId()));

				BOOST_FOREACH(shared_ptr<LineStop> ls, linestops)
				{
					string departureTimeStr;
					string arrivalTimeStr;

					if (ls->getRankInPath() > 0 && ls->isArrival())
					{
						arrivalTimeStr = to_simple_string(Service::GetTimeOfDay(sdsrv->getArrivalBeginScheduleToIndex(false, ls->getRankInPath())));
					}
					else
					{
						arrivalTimeStr = to_simple_string(Service::GetTimeOfDay(sdsrv->getDepartureBeginScheduleToIndex(false, ls->getRankInPath())));
					}
					if (ls->getRankInPath()+1 != linestops.size() && ls->isDeparture())
					{
						departureTimeStr = to_simple_string(Service::GetTimeOfDay(sdsrv->getDepartureBeginScheduleToIndex(false, ls->getRankInPath())));
					}
					else
					{
						departureTimeStr = to_simple_string(Service::GetTimeOfDay(sdsrv->getArrivalBeginScheduleToIndex(false, ls->getRankInPath())));
					}

					const StopPoint * stopPoint(static_cast<const StopPoint *>(ls->getFromVertex()));

					//﻿trip_id,stop_id,stop_sequence,arrival_time,departure_time,stop_headsign,pickup_type,drop_off_type,shape_dist_traveled
					stop_times_txt	<< "\n"
									<< _gtfsKey(sdsrv->getKey(),1) << ","				// trip_id
									<< _gtfsKey(stopPoint->getKey()) << "," 			// stop_id
									<< ls->getRankInPath() << "," 			// stop_sequence ==> serviceRange()
									<< arrivalTimeStr.substr(0,8) << ","	// arrival_time
									<< departureTimeStr.substr(0,8) << ","	// departure_time
									<< "," 									// stop_headsign
									<< "," 									// pickup_type
									<< "," 									// drop_off_type
									<< ","; 								// shape_dist_traveled

				}
			}

			// ------------------------------------ END STOP_TIMES.TXT

			// ------------------------------------ BEGIN	ROUTES.TXT
			//typedef const pair<const RegistryKeyType, shared_ptr<CommercialLine> > myType;
			BOOST_FOREACH(Registry<CommercialLine>::value_type  myLine,Env::GetOfficialEnv().getRegistry<CommercialLine>())
			{
				//﻿route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color

				routes_txt  << "\n"
							<< _gtfsKey(myLine.first) << "," 																// route_id
							<< _gtfsKey(myLine.second->getNetwork()->getKey()) << ","										// agency_id
							<< _gtfsStr(myLine.second->getShortName()) << "," 										 	// route_short_name
							<< _gtfsStr(myLine.second->getLongName()) << "," 												// route_long_name
							<< _gtfsStr(myLine.second->getRuleUserName()) << "," 											// route_desc
							<< static_cast<const JourneyPattern *>(*myLine.second->getPaths().begin())->getRollingStock()->getGTFSKey() << "," 												// route_type
							<< "," 																				// route_url
							<< (myLine.second->getColor()?myLine.second->getColor()->toXMLColor():"") << ","; 	// route_color
																												// route_text_color"
			}

			// ------------------------ END	ROUTES.TXT

			// ------------------------ BEGIN	CALENDAR	&	TRIPS.TXT # SERVICES

			list < pair<const Calendar *, RegistryKeyType> > calendarMap;
			RegistryKeyType serviceKey;
			BOOST_FOREACH(Registry<ScheduledService>::value_type itsdsrv, Env::GetOfficialEnv().getRegistry<ScheduledService>())
			//BOOST_FOREACH(const Service* service, path->getServices())
			{
				const ScheduledService* sdService(itsdsrv.second.get());

				if(sdService)
				{
					const Calendar * currentCal = static_cast<const Calendar *>(sdService);
					list < pair<const Calendar *, RegistryKeyType> >::iterator itCal = calendarMap.begin();
					bool alreadyExist = false;
					while(itCal != calendarMap.end())
					{
						if(*(itCal->first) == *currentCal)
						{
							alreadyExist = true;
							break;
						}
						itCal++;
					}

					//if(itCal == calendarMap.end())
					if(!alreadyExist)
					{
						boost::gregorian::date currentDay,firstActiveDay;
						boost::gregorian::date lastActiveDay = sdService->getLastActiveDate();
						bool weekDays [7];
						currentDay = firstActiveDay = sdService->getFirstActiveDate();
						boost::gregorian::date::day_of_week_type dayOfWeek=firstActiveDay.day_of_week();
						unsigned int firstActiveDayIndex=(dayOfWeek+6)%7;
						serviceKey = _gtfsKey(sdService->getKey());
						calendarMap.push_back(make_pair(currentCal,serviceKey));

						/*os  << "\n	_gtfsKey(serviceKey): "
							<< serviceKey
							<< ": added"
							<< endl;*/

						// ------------------------ BEGIN	TRIPS.TXT	1/2
						//trip_id,service_id,route_id,trip_headsign
						trips_txt 		<< "\n"
								<<  _gtfsKey(sdService->getKey(),1) << ","	// trip_id
								<<  serviceKey << ","	// service_id
								<< _gtfsKey(sdService->getRoute()->getCommercialLine()->getKey()) << ","					// route_id
								<< _gtfsStr(sdService->getRoute()->getName());			// trip_head_sign
						// ------------------------ END	TRIPS.TXT


						// ------------------------ BEGIN	CALENDAR.TXT
						//route_id,service_id,﻿trip_id,trip_headsign,direction_id,block_id,shape_id

						calendar_txt 	<< "\n"
								<< serviceKey << ",";




						/*
						 * 1 -> Mon
						 * 2 -> Tues
						 * 3 -> Wed
						 * ...
						 * 0 -> Sun
						 *
						 */


						for(int i=0; i<7; i++)
						{
							weekDays[(i+firstActiveDayIndex)%7] = (sdService->isActive(firstActiveDay + date_duration(i))? 1 : 0);
						}

						for(int i=0; i<7; i++)
						{
							calendar_txt	<< weekDays[i] << ",";
						}


						calendar_txt	<< to_iso_string(sdService->getFirstActiveDate()) << ","
								<< to_iso_string(sdService->getLastActiveDate());
						// ------------------------ END	CALENDAR.TXT



						// ------------------------ BEGIN	CALENDAR_DATES.TXT

						for(int i=0; currentDay <= lastActiveDay;i++)
						{
							bool isNormalyActive = weekDays[firstActiveDayIndex + i%7];
							if(isNormalyActive != (sdService->isActive(currentDay)? 1 : 0))
							{
								if(isNormalyActive == true)
								{
									//service_id,date,exception_type
									calendar_dates_txt << serviceKey << ","
											<< to_iso_string(currentDay) << ","
											<< 1
											<< endl;
								}
								else
								{
									//service_id,date,exception_type
									calendar_dates_txt << serviceKey << ","
											<< to_iso_string(currentDay) << ","
											<< 2
											<< endl;
								}
							}
							currentDay += date_duration(i);
						}


						// ------------------------ END	CALENDAR_DATES.TXT


					}
					else
					{
						serviceKey = itCal->second;
						/*
						os 	<< "\n	"
							<< serviceKey
							<< ": exits."
							<< endl;
						*/

						//trip_id,service_id,route_id,trip_headsign
						trips_txt 		<< "\n"
								<< _gtfsKey(sdService->getKey(), 1) << ","	// trip_id
								<<  serviceKey << ","	// service_id
								<< _gtfsKey(sdService->getRoute()->getCommercialLine()->getKey()) << ","					// route_id
								<< _gtfsStr(sdService->getRoute()->getName());			// trip_head_sign
						// ------------------------ END	TRIPS.TXT
					}




				}
				else
				{
					cerr << "warning! unavailable Scheduled Service" << endl;
					break;
				}
			}

			os << "\n---------------BOUCLE2---------------BOUCLE2---------------BOUCLE2---------------\n" << endl;




			BOOST_FOREACH(Registry<ContinuousService>::value_type itcssrv, Env::GetOfficialEnv().getRegistry<ContinuousService>())
			{
				const ContinuousService* csService(itcssrv.second.get());

				if(csService)
				{


					const Calendar * currentCal = static_cast<const Calendar *>(csService);
					list < pair<const Calendar *, RegistryKeyType> >::iterator itCal = calendarMap.begin();
					bool alreadyExist = false;
					while(itCal != calendarMap.end())
					{
						if(*(itCal->first) == *currentCal)
						{
							alreadyExist = true;
							break;
						}
						itCal++;
					}

					if(!alreadyExist)
					//if(itCal==calendarMap.end())
					{
						boost::gregorian::date currentDay,firstActiveDay;
						boost::gregorian::date lastActiveDay = csService->getLastActiveDate();
						bool weekDays [7];
						currentDay = firstActiveDay = csService->getFirstActiveDate();
						boost::gregorian::date::day_of_week_type dayOfWeek=firstActiveDay.day_of_week();
						unsigned int firstActiveDayIndex=(dayOfWeek+6)%7;
						serviceKey = _gtfsKey(csService->getKey());
						calendarMap.push_back(make_pair(currentCal,serviceKey));

						/*os  << "\n	_gtfsKey(serviceKey): "
							<< serviceKey
							<< ": added"
							<< endl;
						*/

						// ------------------------ BEGIN	CALENDAR.TXT
						//route_id,service_id,﻿trip_id,trip_headsign,direction_id,block_id,shape_id

						calendar_txt 	<< "\n"
								<< serviceKey << ",";


						for(int i=0; i<7; i++)
						{
							weekDays[(i+firstActiveDayIndex)%7] = (csService->isActive(firstActiveDay + date_duration(i))? 1 : 0);
						}

						for(int i=0; i<7; i++)
						{
							calendar_txt	<< weekDays[i] << ",";
						}

						calendar_txt		<< to_iso_string(csService->getFirstActiveDate()) << ","
								<< to_iso_string(lastActiveDay);
						// shape_id
						// ------------------------ END	CALENDAR.TXT


						// ------------------------ BEGIN	CALENDAR_DATES.TXT

						firstActiveDay = csService->getFirstActiveDate();

						/*
						 * 0 -> Mon
						 * 1 -> Tues
						 * 2 -> Wed
						 * ...
						 * 6 -> Sun
						 *
						 */

						for(int i=0; currentDay <= lastActiveDay;i++)
						{
							bool isNormalyActive = weekDays[firstActiveDayIndex + i%7];
							if(isNormalyActive != (csService->isActive(currentDay)? 1 : 0))
							{
								if(isNormalyActive == true)
								{
									//service_id,date,exception_type
									calendar_dates_txt << serviceKey << ","
											<< to_iso_string(currentDay) << ","
											<< 1
											<< endl;
								}
								else
								{
									//service_id,date,exception_type
									calendar_dates_txt << serviceKey << ","
											<< to_iso_string(currentDay) << ","
											<< 2
											<< endl;
								}
							}
							currentDay += date_duration(i);
						}


						// ------------------------ END	CALENDAR_DATES.TXT




						// ------------------------ BEGIN	TRIPS.TXT	2/2
						//trip_id,service_id,route_id,trip_headsign
						trips_txt 		<< "\n"
								<< _gtfsKey(csService->getKey(),1) << ","	// trip_id
								<< serviceKey << ","				// service_id
								<< _gtfsKey(static_cast<const JourneyPattern *>(&(*csService->getPath()))->getCommercialLine()->getKey())  << ","					// route_id
								<< _gtfsStr(static_cast<const JourneyPattern *>(&(*csService->getPath()))->getName());			// trip_head_sign

						// ------------------------ END	TRIPS.TXT


					}
					else
					{
						serviceKey = itCal->second;
						/*
						os 	<< "\n	"
								<< serviceKey
								<< ": exists."
								<< endl;
						*/

						// ------------------------ BEGIN	TRIPS.TXT	2/2
						//trip_id,service_id,route_id,trip_headsign
						trips_txt 		<< "\n"
								<< _gtfsKey(csService->getKey(),1) << ","	// trip_id
								<< serviceKey << ","				// service_id
								<< _gtfsKey(static_cast<const JourneyPattern *>(&(*csService->getPath()))->getCommercialLine()->getKey())  << ","					// route_id
								<< _gtfsStr(static_cast<const JourneyPattern *>(&(*csService->getPath()))->getName());			// trip_head_sign

						// ------------------------ END	TRIPS.TXT
					}




				}
				else
				{
					cerr << "warning! unavailable Continuous Service" << endl;
					break;
				}

			}

			// ------------------------ END	CALENDAR	&	TRIPS.TXT # SERVICES

			// ------------------------ BEGIN	CALENDAR_DATES.TXT



			// ------------------------ END	CALENDAR_DATES.TXT

			agency_txt.close();
			routes_txt.close();
			stops_txt.close();
			trips_txt.close();
			stop_times_txt.close();
			calendar_txt.close();

			//
			//string 	commandeSyst  = "zip " + pathToZipFile + " " + pathToStopsFile + " " +  pathToRoutesFile + " " + pathToTripsFile + " " + pathToStopTimesFile + " " +  pathToCalendarFile + " " + pathToCalendarDatesFile;
			string 	commandeSyst  = "cd " + tempDir.string() + "\n zip GTFS_Tisseo_3.2.1.zip agency.txt stops.txt routes.txt trips.txt stop_times.txt calendar.txt calendar_dates.txt";
			//os 	<< "\n Commande externe à exécuter: " << commandeSyst
			//	<< "\n Résultat de l'exécution: " << std::system(commandeSyst.c_str());
			int sys = std::system(commandeSyst.c_str());
				

			//stringstream fileZIP ...
			stringstream ZipFile	(pathToZipFile.c_str(), ios::out | ios::binary);

			ZipFile.flush();
			os 	<< ZipFile
				<< flush;
		}
	}	
}

/*

			BOOST_FOREACH(Registry<ContinuousService>::value_type itcssrv, Env::GetOfficialEnv().getRegistry<ContinuousService>())
			{
				const ContinuousService* csService(itcssrv.second.get());

				boost::gregorian::date firstActiveDay;
				boost::date_time::first_kday_after<date> fkda(Monday);

				if(csService)
				{

					// ------------------------ BEGIN	TRIPS.TXT
					//trip_id,service_id,route_id,trip_headsign
					trips_txt 		<< "\n"
									<< _gtfsKey(csService->getKey(),1) << ","	// trip_id
									<< _gtfsKey(csService->getKey()) << ","				// service_id
									<< _gtfsKey(static_cast<const JourneyPattern *>(&(*csService->getPath()))->getCommercialLine()->getKey())  << ","					// route_id
									<< _gtfsStr(static_cast<const JourneyPattern *>(&(*csService->getPath()))->getName());			// trip_head_sign
					// ------------------------ END	TRIPS.TXT

					// ------------------------ BEGIN	CALENDAR.TXT
					//route_id,service_id,﻿trip_id,trip_headsign,direction_id,block_id,shape_id
					firstActiveDay = csService->getFirstActiveDate();
					if(firstActiveDay.day_of_week().as_long_string()!="Mon")
					{
						firstActiveDay = fkda.get_date(firstActiveDay);
					}

					calendar_txt 	<< "\n"
									<< _gtfsKey(csService->getKey()) << ",";

					for(int i=0; i<7; i++)
					{
						calendar_txt	<< (csService->isActive(firstActiveDay + date_duration(i))? 1 : 0) << ","; //tuesday
					}

					calendar_txt		<< to_iso_string(csService->getFirstActiveDate()) << ","
										<< to_iso_string(csService->getLastActiveDate());
																			// shape_id
					// ------------------------ END	CALENDAR.TXT
				}
				else
				{
					cerr << "warning! unavailable Continuous Service" << endl;
					break;
				}

			}

 */
