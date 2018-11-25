#ifndef INCLUDED_LEGACYRULESWRAPPER
#define INCLUDED_LEGACYRULESWRAPPER

#include <Rules.h>
#include <RulesProvider.h>

class LegacyRulesWrapper : public RulesProvider
{
public:
  explicit LegacyRulesWrapper(Rules &rules);

  bool has (const std::string&) const override;
  std::string get (const std::string&) const override;
  int getInteger (const std::string&, int defaultValue) const override;
  double getReal (const std::string&) const override;
  bool getBoolean (const std::string&) const override;

  std::vector <std::string> all (const std::string& stem) const;

private:
  Rules& _rules;
};

#endif
