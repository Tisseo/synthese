
//////////////////////////////////////////////////////////////////////////
/// PTUseRuleUpdateAction class header.
///	@file PTUseRuleUpdateAction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_PTUseRuleUpdateAction_H__
#define SYNTHESE_PTUseRuleUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "PTUseRule.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Transport condition update.
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : PTUseRuleUpdateAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the object to update</li>
		///		<li>actionParamna : new name</li>
		///		<li>actionParamca : new capacity</li>
		///		<li>actionParamor : new value for origin is reference</li>
		///		<li>actionParamfi : new value for default fare</li>
		///		<li>actionParamty : new value for type of reservation rule</li>
		///		<li>actionParammx : new value for max delay days</li>
		///		<li>actionParammd : new value for min delay days</li>
		///		<li>actionParammm : new value for min delay minutes</li>
		///		<li>actionParamdl : new value for hour dead line</li>
		/// </ul>
		class PTUseRuleUpdateAction:
			public util::FactorableTemplate<server::Action, PTUseRuleUpdateAction>
		{
		public:
			static const std::string PARAMETER_RULE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_CAPACITY;
			static const std::string PARAMETER_ORIGIN_IS_REFERENCE;
			static const std::string PARAMETER_FARE_ID;
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_MAX_DELAY_DAYS;
			static const std::string PARAMETER_MIN_DELAY_DAYS;
			static const std::string PARAMETER_MIN_DELAY_MINUTES;
			static const std::string PARAMETER_HOUR_DEADLINE;

		private:
			boost::shared_ptr<PTUseRule> _rule;
			std::string _name;
			graph::UseRule::AccessCapacity _capacity;
			bool _originIsReference;
			boost::shared_ptr<const Fare> _fare;
			PTUseRule::ReservationRuleType _type;
			boost::posix_time::time_duration _minDelayMinutes;
			boost::gregorian::date_duration _minDelayDays; 
			boost::optional<boost::gregorian::date_duration> _maxDelayDays;
			boost::posix_time::time_duration _hourDeadLine;


		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);
			


			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				void setRule(boost::shared_ptr<PTUseRule> value) { _rule = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_PTUseRuleUpdateAction_H__
