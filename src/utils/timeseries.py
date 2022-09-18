from pandas import DataFrame
from pandas import concat
import numpy as np

def windowGenerator(df,windowSize,outputSize, features=None, targets=None, dropNan=True):
    """
    A function that generates window from a dataframe.
    Arguments:
        df: a dataframe to be windowed
        features: a list containing names of the features (must include because didnt consider edge case where no feature is given)
        targets: a list containing names of the target (must include )
        windowSize: number of the time steps for input
        outputSize: number of time steps to predict
        dropNan: drop NaN values
    """
    length = df.shape[0]
    newL = length - windowSize + 1
    x = np.zeros(shape=(len(features), newL, windowSize))
    # input sequence
    if features:
        singleCol = np.zeros(shape=(newL, windowSize))
        for i, feature in enumerate(features):
            for j in range(newL):
                singleCol[j] = df[feature].iloc[j:j+windowSize].to_numpy()
            x[i] = singleCol
                
    y = np.zeros(shape=(len(targets), newL))
    # forecast sequence
    if targets:
        for i, target in enumerate(targets):
            y[i] = df[target].shift(-windowSize+1).iloc[:newL].to_numpy()

    # res = concat(cols)
    # res.columns = features + names
    # if dropNan:
    #     res.dropna(inplace=True)
    return (x, y)


mydict = [{'a': 1, 'b': 2, 'c': 3, 'd': 4},
           {'a': 110, 'b': 210, 'c': 310, 'd': 410},
           {'a': 120, 'b': 220, 'c': 320, 'd': 420},
           {'a': 130, 'b': 230, 'c': 330, 'd': 430},
           {'a': 140, 'b': 240, 'c': 340, 'd': 440},
           {'a': 1000, 'b': 2000, 'c': 3000, 'd': 4000 }]

df = DataFrame(mydict)
windowedDataframe = windowGenerator(df, 3, 1, features=["a", "b", "c"], targets=["d"])
print(windowedDataframe[0])
print(windowedDataframe[1])
print("x shape: ",windowedDataframe[0].shape)
print("y shape: ",windowedDataframe[1].shape)
