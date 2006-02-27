#ifndef SYNTHESE_TIME_SCHEDULE_H
#define SYNTHESE_TIME_SCHEDULE_H


#include "module.h"
#include "Hour.h"

#include <iostream>
#include <string>


namespace synthese
{
namespace time
{

/** Schedule (hour + days duration since departure)
@ingroup m04
*/
class Schedule
{
        Hour _hour; //!< Hour
        DaysDuration _daysSinceDeparture; //!< Elapsed days since departure (relative)

    public:

        Schedule ( const Hour& hour, DaysDuration daysSinceDeparture );
        Schedule ( const Schedule& ref );
        ~Schedule ();


        //! \name Getters/Setters
        //@{
        const Hour& getHour() const;
        MinuteValue getMinutes() const;
        HourValue getHours() const;

        DaysDuration getDaysSinceDeparture () const;
        void setDaysSinceDeparture ( DaysDuration daysSinceDeparture );

        //@}


        //! \name Update methods
        //@{
        Schedule& setMinimum();
        Schedule& setMaximum();
        //@}


        /*! \param op Updates this schedule according to the meaning of
          the given special characters ( m => minimum, M =>
          maximum... max = 5 chars)
         */
        Schedule& operator = ( const std::string& );

        /*! \brief Adds a number of minutes to this schedule
          \param op Number of minutes to add
          \warning Only positive durations!
        */
        Schedule& operator += ( MinutesDuration op );

};

bool operator < ( const Schedule& op1, const Schedule& op2 );
bool operator < ( const Schedule& op1, const Hour& op2 );
bool operator <= ( const Schedule& op1, const Schedule& op2 );
bool operator <= ( const Schedule& op1, const Hour& op2 );
bool operator >= ( const Schedule& op1, const Schedule& op2 );
bool operator >= ( const Schedule& op1, const Hour& op2 );
bool operator > ( const Schedule& op1, const Hour& op2 );

/*! \brief Returns number of minutes elapsed between two
schedules. If this \< op2, the returned duration is negative
*/
MinutesDuration operator - ( const Schedule& op1, const Schedule& op2 );

std::ostream& operator<< ( std::ostream& os, const Schedule& op );


}

}
#endif
