#include "GranularDatetime.h"
#include <Datetime.h>
#include <timew.h>
#include <format.h>

GranularDatetime::GranularDatetime(Rules &rules, const std::string &input, const std::string &format) : Datetime()
{
    if (!input.empty()) {
        std::string::size_type start = 0;
        if (! parse (input, start, format))
            throw ::format ("'{1}' is not a valid date in the '{2}' format.", input, format);
    }

    debug (::format ("Granular time, input {1}", this->toISOLocalExtended()));

    if (rules.has("record.granularity"))
        this->correct_for_granularity(rules.getInteger("record.granularity", 0));

    debug (::format ("Granular time, output {1}", this->toISOLocalExtended()));
}

void GranularDatetime::correct_for_granularity(int granularity)
{
    debug (::format ("Granularity set to {1}", granularity));

    int seconds = this->second();
    int minutes = this->minute();

    debug (::format ("Seconds {1}, minutes {2}", seconds, minutes));

    int minute_delta = granularity == 0 ? 0 : minutes % granularity;

    debug (::format ("Seconds {1}, minutes {2}", seconds, minutes-minute_delta));

    this->_date -= minute_delta*60 + seconds;
}
