

//////////////////////////////////////////////////////////////////////////
/// ServiceAdmin class header.
///	@file ServiceAdmin.h
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

#ifndef SYNTHESE_ServiceAdmin_H__
#define SYNTHESE_ServiceAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace env
	{
		class NonPermanentService;
		class ScheduledService;
		class ContinuousService;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// ServiceAdmin Admin compound class.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues
		///	@date 2009
		class ServiceAdmin:
			public admin::AdminInterfaceElementTemplate<ServiceAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string TAB_SCHEDULES;
				static const std::string TAB_CONTINUOUS;
				static const std::string TAB_CALENDAR;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const env::NonPermanentService>	_service;
				boost::shared_ptr<const env::ScheduledService>	_scheduledService;
				boost::shared_ptr<const env::ContinuousService>	_continuousService;
			//@}

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Tests if two admin pages can be considered as the same one.
			/// @param other Other page to compare with. The other page will always be
			///	       of the same class. 
			/// @return true if the other page can be considered as the same than the
			///         current one.
			/// @author Hugues
			/// @date 2009
			virtual bool _hasSameContent(
				const AdminInterfaceElement& other
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param request The current request (can be used to determinate the
			///        current user rights.)
			/// @author Hugues
			/// @date 2009
			virtual void _buildTabs(
				const server::FunctionRequest<admin::AdminRequest>& request
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues
			///	@date 2009
			ServiceAdmin();
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@param objectWillBeCreatedLater Indicates that the object that is
			///	       supposed to be loaded by the page is not created yet. If true,
			///	       some controls should be cancelled.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues
			///	@date 2009
			void setFromParametersMap(
				const server::ParametersMap& map,
				bool objectWillBeCreatedLater
			);

			
			
			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues
			///	@date 2009
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param variables Environment variables defined by the interface
			///	@param request The current request
			///	@author Hugues
			///	@date 2009
			void display(
				std::ostream& stream,
				interfaces::VariablesMap& variables,
				const server::FunctionRequest<admin::AdminRequest>& request
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues
			///	@date 2009
			bool isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& request
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues
			///	@date 2009
			virtual std::string getTitle() const;

			void setService(boost::shared_ptr<const env::ScheduledService> value);
			void setService(boost::shared_ptr<const env::ContinuousService> value);
			void setService(boost::shared_ptr<const env::NonPermanentService> value);
			boost::shared_ptr<const env::NonPermanentService> getService() const;
		};
	}
}

#endif // SYNTHESE_ServiceAdmin_H__
