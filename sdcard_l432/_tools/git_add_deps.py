#!/cygdrive/c/Python39/python

import re, glob
import os, sys

# build/usart.o: Core/Src/usart.c Core/Inc/usart.h Core/Inc/main.h \
#  Drivers/STM32F3xx_HAL_Driver/Inc/stm32f3xx_hal.h \
#  Core/Inc/stm32f3xx_hal_conf.h
#
# note: emtpy line after the block

pattern = re.compile(r'^(?P<target>.*?):(?P<dep>.*?)\n\s*\n', re.MULTILINE | re.DOTALL)

dfiles = [f for f in glob.glob("build/*.d")]

git_files = {*()}

for dfile in dfiles:
    # print(f"match {dfile}")
    m = pattern.match(open(dfile).read())
    if m:
        deps = m.group("dep")
        deps = deps.replace("\\", '')
        deps = deps.split()
        git_files = git_files.union({*deps})

for git_file in git_files:
    if len(sys.argv) == 2 and sys.argv[1] == "--exec":
        os.system(f'git add {git_file}')
    else:
        print(f'git add ./{git_file}')

