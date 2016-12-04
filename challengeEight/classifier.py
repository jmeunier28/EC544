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


train = pd.read_csv('train.csv')
test = pd.read_csv('test.csv')
train.head()
col = ['Beacon 1', 'Beacon 2','Beacon 3', 'Beacon 4']
col2 = ['Bin']
trainArr = train.as_matrix(col)
trainOut = train.as_matrix(col2)
testArr = test.as_matrix(col)
testOut = test.as_matrix(col2)
nbrs = KNeighborsClassifier(n_neighbors=10, algorithm='auto')
nbrs.fit(trainArr, trainOut) #fit the data

#dumb into pickle file to cache the model
joblib.dump(nbrs,'data.pkl')

