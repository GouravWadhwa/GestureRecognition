import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import json

def sigmoid (Z) :
	A = 1 / (1 + np.exp (-Z))
	return A

def relu (Z) :
	A = np.maximum (0, Z)
	return A

def back_sigmoid (dA, Z) :
	s = 1 / (1 + np.exp (-Z))
	dZ = dA * s * (1 - s)
	return dZ

def back_relu (dA, Z) :
	dZ = np.array (dA, copy = True)
	dZ [Z <= 0] = 0
	return dZ

def intialize_pareameter (layer_dims) :
	parameters = {}
	L = len(layer_dims) 

	for l in range(1, L):nn for game
		parameters['W' + str(l)] = np.random.randn (layer_dims[l], layer_dims[l-1]) * 0.01
		parameters['b' + str(l)] = np.zeros ((layer_dims[l], 1))

	return parameters

def linear_forward (A, W, b) :
	Z = np.dot (W, A) + b
	cache = (A, W, b, Z)
	return Z, cache

def linear_activation_forward (A_prev, W, b, activation) :
	if activation == 'sigmoid' :
		Z, cache = linear_forward (A_prev, W, b)
		A = sigmoid (Z)

	if activation == 'relu' :
		Z, cache = linear_forward (A_prev, W, b)
		A = sigmoid (Z)

	return A, cache

def forward_propogation (X, parameters) :
	caches = []
	A = X
	L = len(parameters) // 2

	for l in range (1, L) :
		A_prev = A
		A, cache = linear_activation_forward (A, parameters ['W' + str(l)], parameters['b' + str(l)], 'sigmoid')
		caches.append (cache)

	A_last, cache = linear_activation_forward (A, parameters['W' + str(L)], parameters['b' + str(L)], 'sigmoid')
	caches.append (cache)

	return A_last, caches

def compute_cost (A_last, Y) :
	m = Y.shape[1]
	n = Y.shape[0]
	cost = 0
	for i in range (m) :
		for j in range (n) :
			if Y[j][i] == 1 :
				cost += Y[j][i] * np.log (A_last[j][i])
			else :
				cost += (1 - Y[j][i])* np.log (1 - A_last[j][i])

	cost = - (cost / m)
	cost = np.squeeze (cost)

	return cost

def linear_backward (dZ, cache) :
	A_prev, W, b, Z = cache
	m = A_prev.shape[1]

	dW = (1 / m) * np.dot (dZ, A_prev.T)
	db = (1 / m) * np.sum (dZ, axis = 1, keepdims = True)
	dA_prev = np.dot (W.T, dZ)

	return dA_prev, dW, db

def linear_activation_backward (dA, cache, activation) :
	A_prev, W, b, Z = cache

	if activation == 'sigmoid' :
		dZ = back_sigmoid (dA, Z)
		dA_prev, dW, db = linear_backward (dZ, cache)

	if activation =='relu' :
		dZ = back_relu (dA, Z)
		dA_prev, dW, db = linear_backward (dZ, cache)

	return dA_prev, dW, db

def backward_propogation (A_last, Y, caches) :
	grads = {}
	L = len(caches)
	m = A_last.shape[1]
	Y = Y.reshape(A_last.shape)

	dA_last = - (np.divide(Y, A_last) - np.divide(1 - Y, 1 - A_last))

	current_cache = caches[L-1]
	grads["dA" + str(L-1)], grads["dW" + str(L)], grads["db" + str(L)] = linear_activation_backward (dA_last, current_cache, "sigmoid")

	for l in reversed(range(L-1)):
		current_cache = caches[l]
		dA_prev_temp, dW_temp, db_temp = linear_activation_backward (grads["dA" + str(l + 1)], current_cache, "sigmoid")
		grads["dA" + str(l)] = dA_prev_temp
		grads["dW" + str(l + 1)] = dW_temp
		grads["db" + str(l + 1)] = db_temp

	return grads

def update_parameters (parameters, grads, learning_rate) :
	L = len(parameters) // 2
	for l in range(L):
		parameters["W" + str(l+1)] -= learning_rate * grads ["dW" + str(l + 1)]
		parameters["b" + str(l+1)] -= learning_rate * grads ["db" + str(l + 1)]
	return parameters

def read_dataset () :
	lst = ['Deer', 'FingerSpread', 'Fist', 'OK', 'Spidy', 'ThumbsUp', 'Tick', 'Victory', 'WaveIn', 'WaveOut']
	X = None
	Y = None
	for i in range (len (lst)) :
		path = "Dataset2/" + lst[i] + ".csv"
		df = pd.read_csv (path)
		x = df[df.columns[:]].values
		y = one_hot_encode (x.shape[0], 10, i)
		if i == 0 :
			X = x
			Y = y
		else :
			X = np.concatenate ((X, x), axis = 0)
			Y = np.concatenate ((Y, y), axis = 0)
	return X, Y


def one_hot_encode (row, col, pos) :
	y = np.zeros((row, col))
	y[:, pos] = 1
	return y

def train_test_split (X, Y, test_size = 0.1) :
	Z = np.concatenate ((X, Y), axis = 1)
	np.random.shuffle (Z)
	X = Z[:, 0:1400]
	Y = Z[:, 1400:]

	testing_size = int ((test_size * X.shape[0]) + 1)
	train_x = X[:-testing_size, :]
	train_y = Y[:-testing_size, :]

	test_x = X[-testing_size:, :]
	test_y = Y[-testing_size:, :]

	return train_x.T, train_y.T, test_x.T, test_y.T

def neural_network_model (X, Y, layer_dims, learning_rate = 0.01, num_iterations = 100, print_cost = True) :
	costs = []
	parameters = intialize_pareameter (layer_dims)
	for i in range (num_iterations) :
		A_last, caches = forward_propogation (X, parameters)

		cost = compute_cost (A_last, Y)
		grads = backward_propogation (A_last, Y, caches)

		parameters = update_parameters (parameters, grads, learning_rate)

		if print_cost :
			print ("Cost after iteration %i: %f" %(i, cost))
		if print_cost and i != 0 :
			costs.append(cost)

	plt.plot(np.squeeze(costs))
	plt.ylabel('cost')
	plt.xlabel('iterations (per tens)')
	plt.title("Learning rate =" + str(learning_rate))
	plt.show()

	return parameters

def predict (X, Y, parameters) :
	m = X.shape[1]
	n = len(parameters) // 2 
	p = np.zeros((10,m))

	probas, caches = forward_propogation (X, parameters)

	sum = 0
	for i in range(0, probas.shape[1]) :
		highest_number = -1
		highest_index = -1 
		for j in range (0, probas.shape[0]) :
			if highest_number < probas[j][i] :
				highest_index = j
				highest_number = probas[j][i]
		p[highest_index][i] = 1
		if Y[highest_index][i] == 1 and probas[highest_index][i] > 0.8 :
			sum += 1
	print("Accuracy: ", sum / m)    
	return p

print ("Retriving Data ......")
X, Y = read_dataset ()
train_x, train_y, test_x, test_y = train_test_split (X, Y, 0.1)
print ("Data Retrived !!!!!!!!")

print (train_x.shape)
print (test_x.shape)

print ("\n !!!!!!!!!!!!!!! Model Is Training !!!!!!!!!!!!!!!!!!!! \n")
layer_dims = [1400, 1600, 10]
parameters = neural_network_model (X.T, Y.T, layer_dims, learning_rate = 0.1, num_iterations = 1000, print_cost = True)
print ("\n !!!!!!!!!!!!!!!! Model Is Trained !!!!!!!!!!!!!!!!!!!! \n")


pred_train = predict (train_x, train_y, parameters)
# for i in range (pred_train.shape[1]) :
# 	for j in range (pred_train.shape[0]) :
# 		print (pred_train[j][i], end = " ")
# 	print ()

pred_test = predict (test_x, test_y, parameters)
# for i in range (pred_test.shape[1]) :
# 	for j in range (pred_test.shape[0]) :
# 		print (pred_test[j][i], end = " ")
# 	print ()

for i in range (1, len (layer_dims)) :
	np.savetxt("Weight" + str (i) + ".csv", parameters['W' + str(i)], delimiter=",")
	np.savetxt("biases" + str (i) + ".csv", parameters['b' + str(i)], delimiter=",")
