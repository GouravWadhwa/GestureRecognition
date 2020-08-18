import numpy as np
import pandas as pd
import csv

def read_dataset () :
	lst = ['Deer', 'FingerSpread', 'Fist', 'OK', 'Spidy', 'ThumbsUp', 'Tick', 'Victory', 'WaveIn', 'WaveOut'] 
	for name in lst :
		x = []
		for i in range(40) :
			path = "Dataset/" + name + "_" + str (i+1) + ".csv"
			df = pd.read_csv (path)
			X = df[df.columns[:]].values
			path = "Dataset2/" + name + ".csv"
			for j in range (90) :
				x.append(np.reshape (X[(j * 10) : (j * 10) + 100, :], (1400, )))
		np.savetxt(path, x, delimiter=', ', fmt='%12.8f')

read_dataset()