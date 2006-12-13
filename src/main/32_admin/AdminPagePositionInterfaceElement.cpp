
#include <sstream>

#include "11_interfaces/ValueElementList.h"

#include "30_server/Request.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminPagePositionInterfaceElement.h"

namespace synthese
{
	using namespace std;
	using namespace interfaces;
	using namespace util;
	using namespace server;

	namespace admin
	{
		std::string AdminPagePositionInterfaceElement::getUpPages(const AdminInterfaceElement* page, const Request* request, bool isFirst)
		{
			stringstream str;
			Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); 
			for (; it != Factory<AdminInterfaceElement>::end() && it->getFactoryKey() != page->getSuperior();
				++it);
			
			if (it != Factory<AdminInterfaceElement>::end())
			{
				string supStr = getUpPages(*it, request, false);
				str << supStr << "&nbsp;&gt;&nbsp;";
			}
			if (!isFirst)
			{
				str << page->getHTMLLink(request);
			}
			else
			{
				str << page->getTitle();
			}			
			return str.str();
		}

		void AdminPagePositionInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{

		}

		AdminPagePositionInterfaceElement::~AdminPagePositionInterfaceElement()
		{

		}

		std::string AdminPagePositionInterfaceElement::getValue( const ParametersVector&, const void* object /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			const AdminInterfaceElement* aie = (const AdminInterfaceElement*) object;
			return getUpPages(aie, request);
		}
	}
}

