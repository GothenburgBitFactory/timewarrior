#include <LegacyRulesWrapper.h>

LegacyRulesWrapper::LegacyRulesWrapper(Rules &rules)
  : _rules(rules)
{}

bool LegacyRulesWrapper::has (const std::string& key) const
{
  return _rules.has(key);
}

std::string LegacyRulesWrapper::get (const std::string& key) const
{
  return _rules.get(key);
}

int LegacyRulesWrapper::getInteger (const std::string& key, int defaultValue) const
{
  return _rules.getInteger(key, defaultValue);
}

double LegacyRulesWrapper::getReal (const std::string& key) const
{
  return _rules.getReal(key);
}

bool LegacyRulesWrapper::getBoolean (const std::string& key) const
{
  return _rules.getBoolean(key);
}

std::vector <std::string> LegacyRulesWrapper::all (const std::string& stem = "") const
{
  return _rules.all(stem);
}
