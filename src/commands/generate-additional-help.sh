#!/bin/sh

echo "// Generated file. Do not modify"
echo "static const char * documented_concepts[] = {"
for command in $(/bin/ls doc/man7/*.in | /bin/sed -n 's|doc/man7/timew-\(\w\+\).7.in|\1|p' | /usr/bin/sort); do
  echo "  \"${command}\","
done
echo "};"

echo "static const char * documented_commands[] = {"
for command in $(/bin/ls doc/man1/*.in | /bin/sed -n 's|doc/man1/timew-\(\w\+\).1.in|\1|p' | /usr/bin/sort); do
  echo "  \"${command}\","
done
echo "};"
