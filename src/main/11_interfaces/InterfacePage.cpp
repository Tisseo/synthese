
#include "InterfacePage.h"
#include "LineLabelInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		void InterfacePage::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
			std::string label_to_go = "";
			for (PageComponentsVector::const_iterator it = _components.begin(); it != _components.end(); ++it)
			{
				if (label_to_go == "" || it->first == label_to_go)
				{
					it->second->display(stream, parameters, object, site);
					const LineLabelInterfaceElement* llie = dynamic_cast<const LineLabelInterfaceElement*>(it->second);
					label_to_go = (llie == NULL) ? "" : llie->getLabel();
				}
			}
		}

		void InterfacePage::clear()
		{
			for ( PageComponentsVector::iterator it = _components.begin(); it != _components.end(); ++it )
				delete it->second;
			_components.clear();
		}

		void InterfacePage::parse( const std::string& text )
		{
			size_t start_pos;
			size_t end_pos;
			std::string last_label = "";
			for (start_pos = 0; start_pos < text.size(); start_pos = end_pos + 1)
			{
				for (end_pos = start_pos; end_pos < text.size() && text[end_pos] != '\n'; ++end_pos);

				LibraryInterfaceElement* lie = LibraryInterfaceElement::create( text.substr(start_pos, end_pos - start_pos) );
				if (lie != NULL)
				{
					LineLabelInterfaceElement* llie = dynamic_cast<LineLabelInterfaceElement*>(lie);
					if ( llie != NULL )
					{
						last_label = llie->getLabel();
						delete lie;
					}
					else
					{
						_components.push_back( make_pair( last_label, lie ));
						last_label = "";
					}
				}
			}
		}
	}
}
