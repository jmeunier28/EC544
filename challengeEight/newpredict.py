from sklearn.neighbors import KNeighborsClassifier, NearestNeighbors
import pandas as pd
import numpy as np
import sys
import argparse
from sklearn.externals import joblib

parser = argparse.ArgumentParser()
parser.add_argument("b1", help="signal Beacon one")
parser.add_argument("b2", help="signal Beacon two")
parser.add_argument("b3", help="signal Beacon three")
parser.add_argument("b4", help="signal Beacon four")
args = vars(parser.parse_args())

b1 = args.get("b1",None)
b2 = args.get("b2",None)
b3 = args.get("b3",None)
b4 = args.get("b4",None)


def predict(point):
	nbrs = joblib.load('model.pkl')
	bin = nbrs.predict(point)
	return bin



point = np.array([b1,b2,b3,b4])
point = point.reshape(1, -1)

print(predict(point))
