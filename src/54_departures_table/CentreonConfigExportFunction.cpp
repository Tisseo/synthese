
/** CentreonConfigExportFunction class implementation.
	@file DisplayScreenContentFunction.cpp

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

#include "CentreonConfigExportFunction.h"

#include "RequestException.h"
#include "Request.h"

#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "User.h"
#include "UserTableSync.h"
#include "SecurityModule.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ProfileTableSync.h"
#include "DisplayMaintenanceRight.h"
#include "Interface.h"
#include "DisplayGetNagiosStatusFunction.h"
#include "Env.h"

#include "MD5Wrapper.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace time;
	using namespace env;
	using namespace interfaces;
	using namespace db;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,departurestable::CentreonConfigExportFunction>::FACTORY_KEY("CentreonConfigExportFunction");

	namespace departurestable
	{

		ParametersMap CentreonConfigExportFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void CentreonConfigExportFunction::_setFromParametersMap(const ParametersMap& map)
		{
		}

		void CentreonConfigExportFunction::_run(
			std::ostream& stream
		) const	{

			Env env;

			const string CHECK_SYNTHESE_COMMAND_ID("5000");
			const string CHECK_SYNTHESE_COMMAND_NAME("'check_synthese'");
			const string SYNTHESE_SERVER_ID("9999999");
			const string SYNTHESE_SERVER_CLASS("9999999");
			const string GENERATED_BY_SYNTHESE("'Generated by SYNTHESE3'");
			const string SYNTHESE_SERVER_NAME("'Serveur SYNTHESE3'");
			const string HARDWARE_SERVICE_ID("5000");
			const string HARDWARE_SERVICE_NAME("'Harware test'");
			const string SYNTHESE_PING_SERVICE_ID("5001");
			const string SYNTHESE_PING_SERVICE_NAME("'Ping'");
			const size_t MAX_PROFILES(99999);

			// Deletion of existing objects
			stream <<
				"BEGIN;" << endl <<
				"DELETE FROM host;" << endl <<
				"DELETE FROM hostgroup;" << endl <<
				"DELETE FROM hostgroup_relation;" << endl <<
				"DELETE FROM service;" << endl <<
				"DELETE FROM host_service_relation;" << endl <<
				"DELETE FROM contactgroup_service_relation;" << endl
			;

			// Insertion of the SYNTHESE command
			stream <<
				"REPLACE INTO command(command_id,command_name,command_line,command_type) VALUES(" <<
				CHECK_SYNTHESE_COMMAND_ID << "," << CHECK_SYNTHESE_COMMAND_NAME <<
				",'$USER1$#S#check_synthese.sh  \"http://" <<
				_request->getHostName() << _request->getClientURL() <<
				Request::PARAMETER_STARTER << Request::PARAMETER_FUNCTION <<
				Request::PARAMETER_ASSIGNMENT << DisplayGetNagiosStatusFunction::FACTORY_KEY <<
				Request::PARAMETER_SEPARATOR << DisplayGetNagiosStatusFunction::PARAMETER_DISPLAY_SCREEN_ID <<
				Request::PARAMETER_ASSIGNMENT << "$HOSTADDRESS$\"',2);" << endl;



			// Services
			stream <<
				"REPLACE INTO service(" <<
				"service_id,command_command_id,timeperiod_tp_id,timeperiod_tp_id2," << //4
				"service_description,service_is_volatile,service_max_check_attempts,service_normal_check_interval," << //4
				"service_retry_check_interval,service_active_checks_enabled,service_passive_checks_enabled," << //3
				"service_parallelize_check,service_obsess_over_service,service_check_freshness," << //3
				"service_event_handler_enabled,service_flap_detection_enabled,service_process_perf_data," << //3
				"service_retain_status_information,service_retain_nonstatus_information,service_notification_interval," << //3
				"service_notification_options,service_notifications_enabled,service_comment," << //3
				"service_register,service_activate" << //2
				") VALUES (" <<
				HARDWARE_SERVICE_ID << "," << CHECK_SYNTHESE_COMMAND_ID << ",1,1," << //4
				HARDWARE_SERVICE_NAME << ",'2',5000,1," << //4
				"1,'2','2'," << //3
				"'2','2','2'," << //3
				"'2','2','1'," << //3
				"'1','2',720," << //3
				"'w,u,c,r,f','2'," << GENERATED_BY_SYNTHESE << "," << //3
				"'1','1'" << //2
			");" << endl <<

			"REPLACE INTO extended_service_information(esi_id,service_service_id) VALUES(" <<
				HARDWARE_SERVICE_ID << "," << HARDWARE_SERVICE_ID <<
			");" << endl <<

			"REPLACE INTO service(" <<
				"service_id,command_command_id,timeperiod_tp_id,timeperiod_tp_id2," <<
				"service_description,service_is_volatile,service_max_check_attempts,service_normal_check_interval," <<
				"service_retry_check_interval,service_active_checks_enabled,service_passive_checks_enabled," <<
				"service_parallelize_check,service_obsess_over_service,service_check_freshness," <<
				"service_event_handler_enabled,service_flap_detection_enabled,service_process_perf_data," <<
				"service_retain_status_information,service_retain_nonstatus_information,service_notification_interval," <<
				"service_notification_options,service_notifications_enabled,service_comment," <<
				"command_command_id_arg,service_register,service_activate" <<
				") VALUES (" <<
				SYNTHESE_PING_SERVICE_ID << ",(SELECT command_id FROM command WHERE command_name='check_centreon_ping'),1,1," <<
				SYNTHESE_PING_SERVICE_NAME << ",'2',5000,1," <<
				"1,'2','2'," <<
				"'2','2','2'," <<
				"'2','2','1'," <<
				"'1','2',15," <<
				"'w,u,c,r,f','2'," << GENERATED_BY_SYNTHESE << "," <<
				"'!3!200,20%!400,50%','1','1'" <<
			");" << endl <<

			"REPLACE INTO extended_service_information(esi_id,service_service_id) VALUES(" <<
				SYNTHESE_PING_SERVICE_ID << "," << SYNTHESE_PING_SERVICE_ID <<
			");" << endl
			;


			// Insertion of profiles
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(env)
			);
			BOOST_FOREACH(shared_ptr<const Profile> profile, profiles)
			{
				if(!profile->isAuthorized<DisplayMaintenanceRight>()) continue;

				stream <<
					"REPLACE INTO contactgroup(cg_id,cg_name,cg_alias,cg_comment,cg_activate) VALUES(" <<
						decodeObjectId(profile->getKey()) << "," <<
						Conversion::ToSQLiteString(profile->getName()) << "," <<
						Conversion::ToSQLiteString(profile->getName()) << "," <<
						GENERATED_BY_SYNTHESE << "," <<
						"'1'"
					");" << endl <<

					"REPLACE INTO contactgroup_service_relation(cgsr_id,contactgroup_cg_id,service_service_id) VALUES(" <<
						decodeObjectId(profile->getKey()) << "," <<
						decodeObjectId(profile->getKey()) << "," <<
						SYNTHESE_PING_SERVICE_ID <<
					");" << endl <<

					"REPLACE INTO contactgroup_service_relation(cgsr_id,contactgroup_cg_id,service_service_id) VALUES(" <<
						(decodeObjectId(profile->getKey()) + MAX_PROFILES) << "," <<
						decodeObjectId(profile->getKey()) << "," <<
						HARDWARE_SERVICE_ID <<
					");" << endl
				;
			}

			// Insertion of the users
			MD5Wrapper md5;
			UserTableSync::SearchResult users(
				UserTableSync::Search(env)
			);
			BOOST_FOREACH(shared_ptr<const User> user, users)
			{
				if(user->getLogin() == SecurityModule::ROOT_USER) continue;
				if(!user->getProfile()->isAuthorized<DisplayMaintenanceRight>()) continue;

				stream <<
					"REPLACE INTO contact(contact_id,timeperiod_tp_id,timeperiod_tp_id2," <<
					"contact_name,contact_alias,contact_passwd," <<
					"contact_lang,contact_host_notification_options,contact_service_notification_options," <<
					"contact_email,contact_comment,contact_oreon,contact_admin," <<
					"contact_type_msg,contact_activate,contact_auth_type" <<
					") VALUES (" <<
					decodeObjectId(user->getKey()) << ",1,1," << 
					Conversion::ToSQLiteString(user->getFullName()) << "," << Conversion::ToSQLiteString(user->getLogin()) << "," << Conversion::ToSQLiteString(md5.getHashFromString(user->getPassword())) << "," <<
					"'fr_FR','n','n'," <<
					Conversion::ToSQLiteString(user->getEMail()) << "," << GENERATED_BY_SYNTHESE << ",'1','0'," << 
					"'txt','1','local'" <<
					");" << endl
				;

				stream <<
					"REPLACE INTO contact_servicecommands_relation(" <<
						"csc_id,contact_contact_id,command_command_id" <<
					") VALUES (" <<
						decodeObjectId(user->getKey()) << "," << decodeObjectId(user->getKey()) << "," <<
						"(SELECT command_id FROM command WHERE command_name='notify-by-email')" <<
					");" << endl <<

					"REPLACE INTO contact_hostcommands_relation(" <<
						"chr_id,contact_contact_id,command_command_id" <<
					") VALUES (" <<
						decodeObjectId(user->getKey()) << "," << decodeObjectId(user->getKey()) << "," <<
						"(SELECT command_id FROM command WHERE command_name='notify-by-email')" <<
					");" << endl <<

					"REPLACE INTO contactgroup_contact_relation(" <<
						"cgr_id,contact_contact_id,contactgroup_cg_id" <<
					") VALUES (" <<
						decodeObjectId(user->getKey()) << "," <<
						decodeObjectId(user->getKey()) << "," <<
						decodeObjectId(user->getProfile()->getKey()) <<
					");" << endl
				;
			}


			// Insertion of the hostgroups
			DisplayTypeTableSync::SearchResult types(
				DisplayTypeTableSync::Search(env)
			);
			BOOST_FOREACH(shared_ptr<const DisplayType> type, types)
			{
				stream <<
					"REPLACE INTO hostgroup(hg_id,hg_name,hg_alias,hg_comment,hg_activate) VALUES(" <<
					decodeObjectId(type->getKey()) << "," <<
					Conversion::ToSQLiteString(type->getName()) << "," <<
					Conversion::ToSQLiteString(type->getName()) << "," <<
					GENERATED_BY_SYNTHESE << "," <<
					"'1'" <<
					");" << endl <<

					"REPLACE INTO host_service_relation(hsr_id,hostgroup_hg_id,service_service_id) VALUES (" <<
					decodeObjectId(type->getKey()) << "," <<
					decodeObjectId(type->getKey()) << "," <<
					HARDWARE_SERVICE_ID <<
					");" << endl
				;				
			}
			stream <<
				"INSERT INTO hostgroup(hg_id,hg_name,hg_alias) VALUES(" <<
				SYNTHESE_SERVER_CLASS << "," <<
				SYNTHESE_SERVER_NAME << "," <<
				SYNTHESE_SERVER_NAME <<
			");" << endl <<

			"REPLACE INTO host_service_relation(hsr_id,hostgroup_hg_id,service_service_id) VALUES (" <<
				SYNTHESE_SERVER_CLASS << "," <<
				SYNTHESE_SERVER_CLASS << "," <<
				SYNTHESE_PING_SERVICE_ID <<
			");" << endl
			;


			// Insertion of the hosts
			DisplayScreenTableSync::SearchResult screens(
				DisplayScreenTableSync::Search(env)
			);
			BOOST_FOREACH(shared_ptr<const DisplayScreen> screen, screens)
			{
				if(!screen->isMonitored()) continue;

				stream <<
					"INSERT INTO host(host_id,command_command_id," <<
					"command_command_id_arg1,"
					"timeperiod_tp_id,timeperiod_tp_id2," <<
					"host_name,host_alias," <<
					"host_address,host_max_check_attempts," <<
					"host_check_interval,host_active_checks_enabled," <<
					"host_passive_checks_enabled,host_checks_enabled," <<
					"host_obsess_over_host,host_check_freshness," <<
					"host_event_handler_enabled,host_flap_detection_enabled," <<
					"host_process_perf_data,host_retain_status_information," <<
					"host_retain_nonstatus_information,host_notification_interval," <<
					"host_notification_options,host_notifications_enabled," <<
					"host_snmp_version,host_comment,host_register,host_activate" <<
					") VALUES(" <<
					decodeObjectId(screen->getKey()) << ",(SELECT command_id FROM command WHERE command_name='check_centreon_dummy')," <<
					"'!0!N/A'," <<
					"1,1," <<
					Conversion::ToSQLiteString(screen->getFullName()) << "," << Conversion::ToSQLiteString(screen->getFullName()) << "," <<
					screen->getKey() << ",1000," <<
					"'1','2'," <<
					"'2','2'," <<
					"'2','2'," <<
					"'2','2'," <<
					"'2','2'," <<
					"'2',720," <<
					"'d,u,r,f','2'," <<
					"'0'," << GENERATED_BY_SYNTHESE << ",'1','1'" <<
					");" << endl <<
					
					"INSERT INTO extended_host_information(" <<
					"ehi_id,host_host_id" <<
					") VALUES (" <<
					decodeObjectId(screen->getKey()) << "," << decodeObjectId(screen->getKey()) <<
					");" << endl <<
					
					"INSERT INTO hostgroup_relation(hgr_id,hostgroup_hg_id,host_host_id) VALUES(" <<
					decodeObjectId(screen->getKey()) << "," <<
					decodeObjectId(screen->getType()->getKey()) << "," <<
					decodeObjectId(screen->getKey()) <<
					");" << endl
				;
			}


			stream <<
				"INSERT INTO host(host_id,command_command_id," << 
				"timeperiod_tp_id,timeperiod_tp_id2," <<
				"host_name,host_alias," <<
				"host_address,host_max_check_attempts," <<
				"host_check_interval,host_active_checks_enabled," <<
				"host_passive_checks_enabled,host_checks_enabled," <<
				"host_obsess_over_host,host_check_freshness," <<
				"host_event_handler_enabled,host_flap_detection_enabled," <<
				"host_process_perf_data,host_retain_status_information," <<
				"host_retain_nonstatus_information,host_notification_interval," <<
				"host_notification_options,host_notifications_enabled," <<
				"host_snmp_version,host_comment,host_register,host_activate" <<
				") VALUES(" <<
				SYNTHESE_SERVER_ID << ",(SELECT command_id FROM command WHERE command_name='check_host_alive')," <<
				"1,1," <<
				SYNTHESE_SERVER_NAME << "," << SYNTHESE_SERVER_NAME << "," <<
				Conversion::ToSQLiteString(_request->getHostName()) << ",1000," <<
				"'1','2'," <<
				"'2','2'," <<
				"'2','2'," <<
				"'2','2'," <<
				"'2','2'," <<
				"'2',720," <<
				"'d,u,r,f','2'," <<
				"'0'," << GENERATED_BY_SYNTHESE << ",'1','1'" <<
				");" << endl <<

				"INSERT INTO extended_host_information(" <<
				"ehi_id,host_host_id" <<
				") VALUES (" <<
				SYNTHESE_SERVER_ID << "," << SYNTHESE_SERVER_ID <<
				");" << endl <<

				"INSERT INTO hostgroup_relation(hgr_id,hostgroup_hg_id,host_host_id) VALUES(" <<
				SYNTHESE_SERVER_ID << "," <<
				SYNTHESE_SERVER_CLASS << "," <<
				SYNTHESE_SERVER_ID  <<
				");" << endl
			;

			stream << "COMMIT;";
		}

		CentreonConfigExportFunction::CentreonConfigExportFunction()
		{
	
		}



		bool CentreonConfigExportFunction::_isAuthorized(
		) const {
			return true;
		}

		std::string CentreonConfigExportFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
