#include <FS.h>
#include <format.h>
#include <paths.h>
#include <shared.h>

namespace paths
{
static const char *legacy_config_dir = "~/.timewarrior";
static const bool uses_legacy_config = Directory (legacy_config_dir).exists ();
const char *timewarriordb = getenv ("TIMEWARRIORDB");

#ifdef __unix__
std::string getPath (const char *xdg_path)
{
  if (timewarriordb != nullptr)
  {
    return timewarriordb;
  }
  else if (uses_legacy_config)
  {
    return legacy_config_dir;
  }
  else
  {
    return std::string (xdg_path) + "/timewarrior";
  }
}

const char *getenv_default (const char *env, const char *default_value)
{
  const char *value = getenv (env);
  if (value == nullptr)
  {
    return default_value;
  }

  return value;
}

static std::string conf_dir = getPath (getenv_default ("XDG_CONFIG_HOME", "~/.config"));
static std::string data_dir = getPath (getenv_default ("XDG_DATA_HOME", "~/.local/share"));
std::string configDir () { return conf_dir; }
std::string dbDir () { return data_dir; }

#else
std::string getPath ()
{
  if (timewarriordb != nullptr)
  {
    return timewarriordb;
  }
  else
  {
    return legacy_config_dir;
  }
}

static std::string path = getPath ();

std::string configDir ()
{
    return path;
}

std::string dbDir ()
{
    return path;
}

#endif

std::string configFile () { return configDir () + "/timewarrior.cfg"; }
std::string dbDataDir () { return dbDir () + "/data"; }
std::string extensionsDir () { return configDir () + "/extensions"; }

void initializeDirs (const CLI &cli, Rules &rules)
{
  Directory configLocation = Directory (configDir ());
  bool configDirExists = configLocation.exists ();

  if (configDirExists &&
      (!configLocation.readable () ||
       !configLocation.writable () ||
       !configLocation.executable ()))
  {
    throw format ("Config is not readable at '{1}'", configLocation._data);
  }

  Directory dbLocation = Directory (dbDir ());
  bool dataLocationExists = dbLocation.exists ();
  if (dataLocationExists &&
          (!dbLocation.readable () ||
           !dbLocation.writable () ||
           !dbLocation.executable ()))
  {
    throw format ("Database is not readable at '{1}'", dbLocation._data);
  }

  std::string question = "";
  if (!configDirExists)
  {
    question += "Create new config in " + configLocation._data + "?";
  }
  if (!dataLocationExists && configLocation._data != dbLocation._data)
  {
    if (question != "") {
        question += "\n";
    }
    question += "Create new database in " + dbLocation._data + "?";
  }

  if (!configDirExists || !dataLocationExists)
  {
    if (cli.getHint ("yes", false) || confirm (question))
    {
      if (!configDirExists)
      {
        configLocation.create (0700);
      }
      if (!dataLocationExists)
      {
        dbLocation.create (0700);
      }
    }
    else
    {
      throw std::string("Initial setup aborted by user");
    }
  }

  // Create extensions subdirectory if necessary.
  Directory extensionsLocation (extensionsDir ());

  if (!extensionsLocation.exists ())
  {
    extensionsLocation.create (0700);
  }

  // Create data subdirectory if necessary.
  Directory dbDataLocation (dbDataDir ());

  if (!dbDataLocation.exists ())
  {
    dbDataLocation.create (0700);
  }

  Path configFileLocation (configFile ());

  if (!configFileLocation.exists ())
  {
    File (configFileLocation).create (0600);
  }

  // Load the configuration data.
  rules.load (configFileLocation);

  // This value is not written out to disk, as there would be no point.
  // Having located the config file, the 'db' location is already known.
  // This is just for subsequent internal use.
  rules.set ("temp.db", dbLocation);
  rules.set ("temp.extensions", extensionsLocation);
  rules.set ("temp.config", configFileLocation);

  // Perhaps some subsequent code would like to know this is a new db and possibly a first run.
  if (!dataLocationExists)
    rules.set ("temp.shiny", 1);
}

} // namespace paths
