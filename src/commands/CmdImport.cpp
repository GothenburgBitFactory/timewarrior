////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025, Gothenburg Bit Factory.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <commands.h>
#include <format.h>
#include <iostream>
#include <timew.h>
#include <IntervalFactory.h>
#include <JSON.h>

////////////////////////////////////////////////////////////////////////////////
std::vector<Interval> parse_content (const std::string& content)
{
    const std::unique_ptr<json::value> json(json::parse (content));

    if (content.empty() || (json == nullptr))
    {
        throw std::string ("Contents invalid.");
    }

    if (json->type () != json::j_array)
    {
        throw std::string ("Expected JSON array for import data.");
    }

    std::vector<Interval> intervals;

    for (const auto item: dynamic_cast<json::array *> (json.get ())->_data)
    {
        Interval new_interval = IntervalFactory::fromJson (item->dump ());
        intervals.push_back (new_interval);
    }

    return intervals;
}

////////////////////////////////////////////////////////////////////////////////
std::string read_input ()
{
    std::string content;
    std::string line;

    while (std::getline (std::cin, line))
    {
        content += line;
    }

    return content;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<Interval> import_file (const std::string& file_name)
{
    Path file_path;

    if (file_name.empty ())
    {
        throw format ("Attempted to import from empty file name!", file_name);
    }

    if (file_name.at (0) == '/')
    {
        file_path = file_name;
    }
    else
    {
        file_path = Directory::cwd () + "/" + file_name;
    }

    std::string content;

    if (const bool exists = file_path.exists (); exists && File::read (file_path, content))
    {
        return parse_content (content);
    }

    throw format ("File {1} does not exist or cannot be read!", file_name);
}

void import_intervals (
    const CLI& cli,
    const Rules& rules,
    Database& database,
    Journal& journal,
    const bool verbose,
    std::vector<Interval>& intervals)
{
    journal.startTransaction ();
    for (auto& interval: intervals)
    {
        // Add each interval to the database
        if (validate (cli, rules, database, interval))
        {
            database.addInterval (interval, verbose);
            database.commit ();
        }
    }
    journal.endTransaction ();
}

////////////////////////////////////////////////////////////////////////////////
int CmdImport (
    CLI& cli,
    Rules& rules,
    Database &database,
    Journal &journal)
{
    const bool verbose = rules.getBoolean ("verbose");

    if (const auto fileNames = cli.getWords (); fileNames.empty ())
    {
        const auto content = read_input ();
        auto intervals = parse_content (content);

        import_intervals (cli, rules, database, journal, verbose, intervals);

        if (verbose)
        {
            std::cout << "Imported " << intervals.size () << " interval(s)." << std::endl;
        }

    }
    else
    {
        for (const auto& fileName: fileNames)
        {
            try
            {
                auto intervals = import_file (fileName);

                import_intervals (cli, rules, database, journal, verbose, intervals);

                if (verbose)
                {
                    std::cout << "Imported " << intervals.size () << " interval(s) from '" << fileName << "'." << std::endl;
                }
            } catch (const std::string& error)
            {
                throw format ("Error importing '{1}': {2}", fileName, error);
            }
        }
    }
    return 0;
}
