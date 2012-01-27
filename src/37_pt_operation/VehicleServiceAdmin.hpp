
//////////////////////////////////////////////////////////////////////////
/// VehicleServiceAdmin class header.
///	@file VehicleServiceAdmin.hpp
///	@author Hugues Romain
///	@date 2011
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

#ifndef SYNTHESE_VehicleServiceAdmin_H__
#define SYNTHESE_VehicleServiceAdmin_H__

#include "AdminInterfaceElementTemplate.h"

#include <boost/date_time/date.hpp>

namespace synthese
{
	namespace pt_operation
	{
		class VehicleService;

		//////////////////////////////////////////////////////////////////////////
		/// VehicleServiceAdmin Admin compound class.
		///	@ingroup m37Admin refAdmin
		///	@author Hugues Romain
		///	@date 2011
		class VehicleServiceAdmin:
			public admin::AdminInterfaceElementTemplate<VehicleServiceAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string TAB_ROUTE;
				static const std::string TAB_DRIVER_SERVICES;

				static const std::string PARAMETER_DATE;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const VehicleService> _vehicleService;
				boost::gregorian::date _date;
			//@}

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Tests if two admin pages can be considered as the same one.
			/// @param other Other page to compare with. The other page will always be
			///	       of the same class.
			/// @return true if the other page can be considered as the same than the
			///         current one.
			/// @author Hugues Romain
			/// @date 2011
			virtual bool _hasSameContent(
				const AdminInterfaceElement& other
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param profile The profile of the current user
			/// @author Hugues Romain
			/// @date 2011
			virtual void _buildTabs(
				const security::Profile& profile
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2011
			VehicleServiceAdmin();



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2011
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2011
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2011
			void display(
				std::ostream& stream,
				const admin::AdminRequest& _request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization check.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2011
			bool isAuthorized(
				const security::User& user
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues Romain
			///	@date 2011
			virtual std::string getTitle() const;


			virtual PageLinks _getCurrentTreeBranch() const;


			void setVehicleService(boost::shared_ptr<const VehicleService> value){ _vehicleService = value; }
		};
}	}

#endif // SYNTHESE_VehicleServiceAdmin_H__

