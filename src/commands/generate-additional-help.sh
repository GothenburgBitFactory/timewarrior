#!/bin/sh

CONCEPTS="$( ls doc/man7/*.in | sed -E 's|doc/man7/timew-(.*).7.in|\1|' | sort )"
COMMANDS="$( ls doc/man1/*.in | sed -E 's|doc/man1/timew-(.*).1.in|\1|' | sort | tr '\n' ' ' )"

echo "// Generated file. Do not modify"
echo "#include <vector>"
echo "#include <string>"
echo "static const std::vector <std::string> timew_help_concepts = {"
for concept in ${CONCEPTS} ; do
   ( echo "${COMMANDS}" | grep -vq "${concept}" ) && echo "  \"${concept}\","
done
echo "};"
