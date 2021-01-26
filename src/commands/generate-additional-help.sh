#!/bin/sh

CONCEPTS="$( find doc/man7 -name "timew-*.7*" | sed -E 's|doc/man7/timew-(.*)\.7.*|\1|' | sort -u )"
COMMANDS="$( find doc/man1 -name "timew-*.1*" | sed -E 's|doc/man1/timew-(.*)\.1.*|\1|' | sort -u | tr '\n' ' ' )"

echo "// Generated file. Do not modify"
echo "#include <vector>"
echo "#include <string>"
echo "static const std::vector <std::string> timew_help_concepts = {"
for concept in ${CONCEPTS} ; do
   ( echo "${COMMANDS}" | grep -vq "${concept}" ) && echo "  \"${concept}\","
done
echo "};"
