#!/cygdrive/c/Python3/python.exe

import sys, os.path, re
from matplotlib import pylab as plt
import numpy
# import pdb

def usage(msg):
    if msg is not None:
        print(f"exiting with message => {msg}")
    print("usage: rd2007.py <fname1> [<fname2> ..]")
    sys.exit()

# uptime=38728.30 [s], total pulses=1154, cpm=(0.00, 1.79) [1/min]
pattern = re.compile(r"^uptime=(?P<ts>[.0-9]+)\s.*\bpulses=(?P<val>\d+),.*$", re.DOTALL)

def process_file(fname):
    tss = []
    vals = []
    lines = open(fname).readlines()
    for line in lines:
        if m := pattern.match(line):
            tss.append(int(float(m.group("ts"))))
            vals.append(int(float(m.group("val"))))
    return (tss, vals)

def print_stat(tss, vals, fname):
    # TODO: extend with derivative
    slope_s = max(vals)/max(tss)
    slope_m = max(vals)/(max(tss)/60)
    print(f"{fname} => slope_s = {slope_s:.5f} [pulse/s]", f"slope_m = {slope_m:.2f} [pulse/m]")

if len(sys.argv) <= 1:
    usage(None)

xs = []
ys = []
names = []
N = 0
YMAX = 0

for fname in sys.argv[1:]:
    if os.path.exists(fname):
        tss,vals = process_file(fname)
        xs.append(tss)
        ys.append(vals)
        names.append(fname)
        N += 1
        # pdb.set_trace()
        YMAX = max(YMAX, max(vals))
    else:
        usage(f"'{fname}' does not exist")

fig = plt.figure("rd2007")
ax = plt.gca()
plt.title(", ".join(names))

for i in range(N):
    plt.plot(xs[i], ys[i], label=names[i])
    print_stat(xs[i], ys[i], names[i])

# plt.yticks(range(YMAX)[::100])
# ax.xaxis_date()
# fig.autofmt_xdate()

plt.ylabel("rd2007 pulses")
plt.xlabel("time [s]")
plt.legend()
plt.show()

