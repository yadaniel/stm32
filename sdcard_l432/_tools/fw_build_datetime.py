#!/cygdrive/c/Python39/python

import sys
import os, os.path
import datetime as dt
from dateutil import tz
# from tkinter import messagebox

# set correct timezone
TZ = tz.gettz("Europe/Berlin")

fw_build = dt.datetime.now(TZ)

hfile = f"""
#ifndef FW_BUILD_TIME_DEFINED
#define FW_BUILD_TIME_DEFINED

#define FW_BUILD_YEAR       {fw_build.year - 2000}
#define FW_BUILD_MONTH      {fw_build.month}
#define FW_BUILD_DAY        {fw_build.day}
#define FW_BUILD_DAYWEEK    {fw_build.isoweekday()}

#define FW_BUILD_HOUR       {fw_build.hour}
#define FW_BUILD_MINUTE     {fw_build.minute}
#define FW_BUILD_SECOND     {fw_build.second}

#endif  // FW_BUILD_TIME_DEFINED
"""

try:
    hfilename = f"{os.getcwd()}\\FW\\fw_build_time.h"
    # messagebox.showinfo("debug", hfilename)
    open(hfilename, "w").write(hfile)
    print("0", flush = True)
    sys.exit(0)
except Exception as e:
    # messagebox.showinfo("debug", e)
    print("1", flush = True)
    sys.exit(1)

