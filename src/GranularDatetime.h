#ifndef GRANULARDATETIME_H
#define GRANULARDATETIME_H

#include <Rules.h>
#include <Datetime.h>

class GranularDatetime : public Datetime
{
public:
    GranularDatetime(Rules &rules, const std::string& input = "", const std::string& format = "");

private:
    void correct_for_granularity(int granularity);
};

#endif // GRANULARDATETIME_H
