import matplotlib.pyplot as plt
import sys
import csv

fig = plt.figure(0)
ax = fig.add_subplot()
fig2 = plt.figure(1)
ax2 = fig2.add_subplot()

for fname in sys.argv[1:]:

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

    avgThrusts = [ thrust - avgThrusts[0] for thrust in avgThrusts ]
    print(avgThrusts)


    x = [row[0] for row in data ]
    p = [row[1] for row in data ]
    y = [row[2] for row in data ]


    ax.scatter(x,y, c='b')
    ax.scatter(x,p, c='r')

    ax2.scatter(percents, avgThrusts, c='g')


plt.show()

