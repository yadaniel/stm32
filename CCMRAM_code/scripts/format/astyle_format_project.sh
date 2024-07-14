#!/bin/bash

# set -e
# set -x

PROJECT_FILES=../../Firmware

for i in $(find ${PROJECT_FILES} -type f -iname "*.[h|c]"); do
    astyle --suffix=none --style=java --pad-oper --add-brackets --add-one-line-brackets --indent-switches --convert-tabs ${i}
done

