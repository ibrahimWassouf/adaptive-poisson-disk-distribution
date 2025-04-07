import matplotlib.pyplot as plt


y = [0] * 7

#map N value to index in y array
lookup = {
        '1000' : 0,
        '3000' : 1,
        '10000' : 2,
        '20000' : 3,
        '40000' : 4,
        '60000' : 5,
        '100000' : 6
}

with open("result.txt", "r") as data:
    for line in data:
        line = line.split()
        N = line[2]
        time = int(line[3])
        y[lookup[N]] += time
        print(time, N, y[lookup[N]])

print(y)

y = [e/20 for e in y]

print(y)

x = [1000, 3000, 10000, 20000, 40000, 60000, 100000]
plt.yscale("log")
plt.xlabel("N stipples")
plt.ticklabel_format(axis="x",style="scientific", scilimits=(4,4))
plt.ylabel("Time (milliseconds)")
plt.title("Execution time over selected stipple count")
plt.plot(x, y)
plt.show()
