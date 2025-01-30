import matplotlib.pyplot as plt
import sys
import csv

fig = plt.figure(0)
axp = fig.add_subplot()
axy = axp.twinx()


print(sys.argv)
fr = "Rear" if sys.argv[1].lower()=="rear" else "Front"
legend = []

if fr=="Rear":
    legend = ["Test " + fname.split('.')[0][-1] for fname in sys.argv[2:]]
else:
    legend = ["Test " + fname.split('_')[1][-1] for fname in sys.argv[2:]]


axp.set_title(f"{fr} Thrust Test Over Time")
axp.set_xlabel("Time (s)")
axp.set_ylabel("Throttle (%)")
axy.set_ylabel("Thrust (kg)")

fig2 = plt.figure(1)
ax2 = fig2.add_subplot()
ax2.set_title(f"{fr} Average Thrust vs. Throttle")
ax2.set_xlabel("Throttle (%)")
ax2.set_ylabel("Thrust (kg)")

tcolors = plt.colormaps['tab20'].colors

i = 0
for fname in sys.argv[2:]:

    f = open(fname, 'r')
    reader = csv.reader(f)
    data = []
    pToY = {}
    pToYC = {}

    for row in reader:
        flots = [float(x) for x in row]
        data.append(flots)
        if flots[1] in pToY.keys():
            pToY[flots[1]] += flots[2]
            pToYC[flots[1]] += 1
        else:
            pToY[flots[1]] = flots[2]
            pToYC[flots[1]] = 0

    percents = []
    avgThrusts = []
    for key in pToY.keys():
        percents.append(key)
        avgThrusts.append(-pToY[key]/pToYC[key])

    tareCorrection = avgThrusts[0]

    avgThrusts = [ thrust - tareCorrection for thrust in avgThrusts ]
    print(avgThrusts)


    x = [row[0] for row in data ]
    p = [row[1] for row in data ]
    y = [-row[2] - tareCorrection for row in data ]


    axy.plot(x,y, c=tcolors[i*2])
    axp.plot(x,p, c=tcolors[i*2 + 1])

    ax2.scatter(percents, avgThrusts, marker='x', c=tcolors[i*2])
    i += 1

ax2.legend(legend, loc='upper left')
ax2.grid(linestyle=":", color='gray')
axp.grid(linestyle=":", color='gray')
axp.legend(legend, loc='upper left')

plt.show()

