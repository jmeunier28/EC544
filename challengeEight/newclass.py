'''
python implementation of knn classifier
will dump model into pickle file to cache model in memory
will be called by server at start of app 

'''


from sklearn.neighbors import KNeighborsClassifier, NearestNeighbors
import pandas as pd
import numpy as np
import sys
import argparse
from sklearn.externals import joblib


nbrs = KNeighborsClassifier(n_neighbors=3, weights='distance', algorithm='kd_tree', p=1) # based off of 3 nearest neighbors


train = pd.read_csv('data.csv')
#test = pd.read_csv('test.csv')
train.head()
col = ['C0:56:27:3A:34:29','48:F8:B3:24:A8:3F','F0:29:29:92:6B:50','B4:E9:B0:E5:0B:D0']
col2 = ['Bin']
trainArr = train.as_matrix(col)
trainOut = train.as_matrix(col2)
#testArr = test.as_matrix(col)
#testOut = test.as_matrix(col2)
#nbrs = KNeighborsClassifier(n_neighbors=10, algorithm='auto')
nbrs.fit(trainArr, trainOut) #fit the data

#dumb into pickle file to cache the model
joblib.dump(nbrs,'model.pkl')

