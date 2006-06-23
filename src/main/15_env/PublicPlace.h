#ifndef SYNTHESE_ENV_PUBLICPLACE_H
#define SYNTHESE_ENV_PUBLICPLACE_H


#include "01_util/Registrable.h"
#include "Place.h"
#include <string>


namespace synthese
{
namespace env
{


class City;


/** Public place class.

 @ingroup m15
*/
class PublicPlace : public synthese::util::Registrable<int, PublicPlace>, public Place
{

private:

protected:

    PublicPlace (const int& id,
		 const std::string& name,
		 const City* city);

public:

    virtual ~PublicPlace ();


    //! @name Getters/Setters
    //@{

    //@}

    
};


}
}

#endif 	    
