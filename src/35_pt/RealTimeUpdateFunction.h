
//////////////////////////////////////////////////////////////////////////////////////////
/// RealTimeUpdateFunction class header.
///	@file RealTimeUpdateFunction.h
///	@author Hugues
///	@date 2009
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

#ifndef SYNTHESE_RealTimeUpdateFunction_H__
#define SYNTHESE_RealTimeUpdateFunction_H__

#include "FactorableTemplate.h"
#include "RequestWithInterface.h"

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
		class ScheduledService;
	}

	namespace pt
	{
		/** RealTimeUpdateFunction Function class.
			@author Hugues
			@date 2009
			@ingroup m35Functions refFunctions

			Parameters :

			Display of the list of the next services :
				- roid : place ID
				- i : interface id
				- n : rank of the first service to display (comparing to the next one)

			Display of the update screen :
				- roid : place ID
				- i : interface id
				- se : service (optional)
				- ls : linestop (optional)
		*/
		class RealTimeUpdateFunction:
			public util::FactorableTemplate<interfaces::RequestWithInterface,RealTimeUpdateFunction>
		{
		public:
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_LINE_STOP_RANK;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const env::ScheduledService>	_service;
				std::size_t _lineStopRank;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
		public:
			RealTimeUpdateFunction();

			

			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual void _run(std::ostream& stream) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool _isAuthorized() const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;


			void setService(boost::shared_ptr<const env::ScheduledService> value);
			void setLineStopRank(std::size_t value);
		};
	}
}

#endif // SYNTHESE_RealTimeUpdateFunction_H__
