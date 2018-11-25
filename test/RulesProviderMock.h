#ifndef INCLUDED_RULESMOCK
#define INCLUDED_RULESMOCK

#include <gmock/gmock.h>
#include <RulesProvider.h>

class RulesProviderMock : public RulesProvider
{
  MOCK_CONST_METHOD1(has, bool (const std::string&));
  MOCK_CONST_METHOD1(get, std::string (const std::string&));
  MOCK_CONST_METHOD2(getInteger, int (const std::string&, int defaultValue));
  MOCK_CONST_METHOD1(getReal, double (const std::string&));
  MOCK_CONST_METHOD1(getBoolean, bool (const std::string&));
};

#endif
