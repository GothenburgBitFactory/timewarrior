#ifndef INCLUDED_RULESPROVIDER
#define INCLUDED_RULESPROVIDER

class RulesProvider
{
public:
  virtual bool has (const std::string&) const = 0;
  virtual std::string get (const std::string&) const = 0;
  virtual int getInteger (const std::string&, int defaultValue) const = 0;
  virtual double getReal (const std::string&) const = 0;
  virtual bool getBoolean (const std::string&) const = 0;

  virtual std::vector <std::string> all (const std::string& stem = "") const = 0;
};

#endif
