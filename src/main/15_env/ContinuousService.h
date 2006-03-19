#ifndef SYNTHESE_ENV_CONTINUOUSSERVICE_H
#define SYNTHESE_ENV_CONTINUOUSSERVICE_H

#include "Service.h"
#include <string>



namespace synthese
{
namespace env
{

    class Calendar;


/** Continuous service.


 @ingroup m15
*/
class ContinuousService : public Service
{
private:

    int _range; //!< Continuous service range.
    int _maxWaitingTime; //!< Max waiting waiting time before next service.
    

public:

    ContinuousService (const std::string& serviceNumber,
		      const Path* path,
		      Calendar* calendar,
		      const synthese::time::Schedule* departureSchedule,
		      int range,
		      int maxWaitingTime);

    ~ContinuousService ();

    
    //! @name Getters/Setters
    //@{
    int getMaxWaitingTime () const;
    int getRange () const;
    //@}

    //! @name Query methods
    //@{
    bool isContinuous () const;
    //@}

};



}
}


#endif
