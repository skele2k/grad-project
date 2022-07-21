import matplotlib.pyplot as plot
import numpy as np
import os

TRAINDATAMAX = 1586700
TRAINDATAINTER = 100000
TRAINDATAYMAX = 1000
TRAINDATAYINTER = 100

TESTDATAMAX = 700873
TESTDATAINTER = 50000
TESTDATAYMAX = 100000#90736
TESTDATAYINTER = 50

fileName = input("Enter the file name of training data: ")
curr = os.getcwd()
data = curr + '\\visualization'
os.chdir(data)
file = open(fileName, 'r')

X = []
Y = []
count = 1
for line in file:
    splitted = line.split()
    Y.append(splitted[1])
    X.append(str(count))
    count = count + 1
file.close()
#print(X)
os.chdir(curr)
print("Data preparation for plot done")
plot.figure(figsize=(14,7))

#plot.yticks(np.arange(0, TRAINDATAYMAX, TRAINDATAYINTER))
#plot.xticks(np.arange(0, TRAINDATAMAX, TRAINDATAINTER))
plot.yticks(np.arange(0, TESTDATAYMAX, TESTDATAYINTER))
plot.xticks(np.arange(0, TESTDATAMAX, TESTDATAINTER))

plot.plot(X, Y)
plot.gca().invert_yaxis()



plot.title(r'data')
plot.ylabel('Frequency')
plot.xlabel('number of datas')
plot.show()