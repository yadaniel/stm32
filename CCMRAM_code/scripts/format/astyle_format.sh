#!/bin/bash

# set -e
# set -x

# astyle --suffix=none --style=java --pad-oper --add-brackets --add-one-line-brackets $1
astyle --suffix=none --style=java --pad-oper --add-brackets --add-one-line-brackets --indent-switches --convert-tabs $1
# astyle --suffix=none --style=java --pad-oper --add-brackets --add-one-line-brackets --indent-switches --delete-empty-lines $1
# astyle --suffix=none --style=java --pad-oper --add-brackets --add-one-line-brackets --indent-switches --fill-empty-lines $1

