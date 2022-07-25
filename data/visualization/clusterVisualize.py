import os

a = 555555555555555555555555.222222222222222 + 77777777777777777777777777777777777.1111111111111111111111111
print(a)

fileName = input("Enter the file name of training data: ")
curr = os.getcwd()
data = curr + '\\visualization'
os.chdir(data)
file = open(fileName, 'r')

frequency = [0, 0, 0, 0]
intervalAvg = [0, 0, 0, 0]
stdDeriv = [0, 0, 0, 0]
blockNum = [0, 0, 0, 0]

totalNum = 0
clusterCount = [0, 0, 0, 0]

flag = True
for line in file:
    if flag :
        flag = False
    else :
        splitted = line.split(',')
        cluster = int(splitted[5])
        clusterCount[cluster] += 1
        frequency[cluster] += int(splitted[1])
        intervalAvg[cluster] += float(splitted[2])
        stdDeriv[cluster] += float(splitted[3])
        blockNum[cluster] += int(splitted[4])
        totalNum += 1
file.close()

for i in range(0, 4) :
    frequency[i] /= clusterCount[i]
    intervalAvg[i] /= clusterCount[i]
    stdDeriv[i] /= clusterCount[i]
    blockNum[i] /= clusterCount[i]
    print("Cluster", i, "\nFrequency Avg :", frequency[i],
    "\nTime Interval Avg :", intervalAvg[i], "\nStd Deriv Avg :",
    stdDeriv[i], "\nblock # Avg :", blockNum[i], "\nNumber of datas :",
    clusterCount[i], '\n')
print("Total number of datas :", totalNum)
os.chdir(curr)
