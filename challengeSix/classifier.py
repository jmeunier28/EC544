# python implementation of knn classifier

from sklearn.neighbors import KNeighborsClassifier, NearestNeighbors
import pandas as pd
import numpy as np
import sys
import argparse
from sklearn.externals import joblib

# parser = argparse.ArgumentParser()
# parser.add_argument("b1", help="signal Beacon one", type = float)
# parser.add_argument("b2", help="signal Beacon two", type = float)
# parser.add_argument("b3", help="signal Beacon three", type = float)
# parser.add_argument("b4", help="signal Beacon four", type = float)
# args = vars(parser.parse_args())

# b1 = args.get("b1",None)
# b2 = args.get("b2",None)
# b3 = args.get("b3",None)
# b4 = args.get("b4",None)

# print("\nB1: "), print(b1)
# print("\nB2: "), print(b2)
# print("\nB3: "), print(b3)
# print("\nB4: "), print(b4)

nbrs = KNeighborsClassifier(n_neighbors=3, weights='distance', algorithm='kd_tree', p=1) # based off of 3 nearest neighbors

# class FindPoint:


# 	def __init__(self):
# 		print(" ")

# 	def trainMe(self,test_file, train_file):

# 		train = pd.read_csv(test_file)
# 		test = pd.read_csv(train_file)
# 		train.head()
# 		col = ['Beacon 1', 'Beacon 2','Beacon 3', 'Beacon 4']
# 		col2 = ['Bin']
# 		trainArr = train.as_matrix(col)
# 		trainOut = train.as_matrix(col2)
# 		testArr = test.as_matrix(col)
# 		testOut = test.as_matrix(col2)
# 		nbrs.fit(trainArr, trainOut) #fit the data

# 	def findBin(self,b1,b2,b3,b4):
# 		point = np.array([b1,b2,b3,b4])
# 		point = point.reshape(1, -1)
# 		bin = nbrs.predict(point)
# 		return bin


# find = FindPoint()
# find.trainMe('train2.csv','sample_rssi_values.csv')
# print(find.findBin(b1,b2,b3,b4))

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
joblib.dump(nbrs,'data.pkl')

# point = np.array([b1,b2,b3,b4])
# point = point.reshape(1, -1)
# bin = nbrs.predict(point)
# print(bin)
# location = nbrs.predict(testArr)

# correct = 0.0

# correct = 0.0
# for i in range(len(location)):
# 	# print("\nActuatl Bin: ")
# 	# print(trainOut[i][0])
# 	# print(" Predicted Bin: ")
# 	#print(testOut[i][0])

# 	if testOut[i][0] == location[i]: 
# 		correct += 1

# print(correct / len(location))

