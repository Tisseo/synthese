
//////////////////////////////////////////////////////////////////////////////////////////
/// ReservationEditFunction class header.
///	@file ReservationEditFunction.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ReservationEditFunction_H__
#define SYNTHESE_ReservationEditFunction_H__

#include "FactorableTemplate.h"
#include "RequestWithInterface.h"

namespace synthese
{
	namespace resa
	{
		class ReservationTransaction;

		/** ReservationEditFunction Function class.
			@author Hugues
			@date 2009
			@ingroup m51Functions refFunctions
		*/
		class ReservationEditFunction:
			public util::FactorableTemplate<interfaces::RequestWithInterface,ReservationEditFunction>
		{
		public:
			static const std::string PARAMETER_RESERVATION_ID;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const ReservationTransaction>	_resa;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			ReservationEditFunction();


			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_ReservationEditFunction_H__
