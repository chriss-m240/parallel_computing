from matplotlib import pyplot as plt
import csv

with open('./out.csv') as csv_file:
    csv_data = csv.reader(csv_file, delimiter=',')
    x = []
    y = []
    for row in csv_data:
        x.append(int(row[0]))
        y.append(float(row[1]))
    print(x)
    print(y)
    plt.plot(x, y)
    plt.show()
