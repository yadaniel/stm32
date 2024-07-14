#!/bin/bash

# set -e
# set -x

PROJECT_FILES=../../Firmware

for i in $(find ${PROJECT_FILES} -type f -iname "*.[h|c]"); do
    clang-format -i -style=file:.clang-format ${i}
done

