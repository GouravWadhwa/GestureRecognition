import tensorflow as tf
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

old_v = tf.logging.get_verbosity()
tf.logging.set_verbosity(tf.logging.ERROR)

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

	print (X.shape, Y.shape)

	testing_size = int (test_size * X.shape[0] + 1)
	train_x = X[:-testing_size, :]
	train_y = Y[:-testing_size, :]

	test_x = X[-testing_size:, :]
	test_y = Y[-testing_size:, :]

	return train_x, train_y, test_x, test_y

def neural_network_model (x, weights, biases) :
	layer_1 = tf.add (tf.matmul (x, weights['h1']), biases['b1'])
	layer_1 = tf.nn.sigmoid (layer_1)

	layer_2 = tf.add (tf.matmul (layer_1, weights['h2']), biases['b2'])
	layer_2 = tf.nn.sigmoid (layer_2)

	layer_3 = tf.add (tf.matmul (layer_2, weights['h3']), biases['b3'])
	layer_3 = tf.nn.sigmoid (layer_3)

	layer_4 = tf.add (tf.matmul (layer_3, weights['h4']), biases['b4'])
	layer_4 = tf.nn.sigmoid (layer_4)

	out_layer = tf.add (tf.matmul (layer_4, weights['out']), biases['out'])
	out_layer = tf.nn.sigmoid (out_layer)
	
	return out_layer

def next_batch (train_x, train_y, count) :
	Z = np.concatenate ((train_x, train_y), axis = 1)
	np.random.shuffle (Z)
	train_x = Z[:, 0:1400]
	train_y = Z[:, 1400:]
	r = np.random.randint (low = 0, high = train_x.shape[0] - count - 10)

	return train_x[r:count + r, :], train_y[r:count + r, :]

cost_history = np.empty (shape = [1], dtype = float)

n_dim = 1400
n_hidden_1 = 1000
n_hidden_2 = 1000
n_hidden_3 = 1000
n_hidden_4 = 1000
n_class = 10

model_path = "/home/gourav"

epochs = 100
batch_size = 1000

weights = {
	'h1' : tf.Variable(tf.truncated_normal ([n_dim, n_hidden_1])),
	'h2' : tf.Variable(tf.truncated_normal ([n_hidden_1, n_hidden_2])),
	'h3' : tf.Variable(tf.truncated_normal ([n_hidden_2, n_hidden_3])),
	'h4' : tf.Variable(tf.truncated_normal ([n_hidden_3, n_hidden_4])),
	'out' : tf.Variable(tf.truncated_normal ([n_hidden_4, n_class]))
}

biases = {
	'b1' : tf.Variable(tf.truncated_normal ([n_hidden_1])),
	'b2' : tf.Variable(tf.truncated_normal ([n_hidden_2])),
	'b3' : tf.Variable(tf.truncated_normal ([n_hidden_3])),
	'b4' : tf.Variable(tf.truncated_normal ([n_hidden_4])),
	'out' : tf.Variable(tf.truncated_normal ([n_class]))
}

x = tf.placeholder (tf.float32, [None, 1400])
y_ = tf.placeholder (tf.float32, [None, 10])

print ("Retriving Data ......")
X, Y = read_dataset()
train_x, train_y, test_x, test_y = train_test_split (X, Y, 0.1)
print ("Data Retrived !!!!!")
print ("Size of Training Data : ", train_x.shape[0])
print ("Size of Testing Data : ", test_x.shape[0], "\n")

init = tf.initialize_all_variables()
saver = tf.train.Saver()

y = neural_network_model (x, weights, biases)

cost_function = tf.reduce_mean (tf.nn.softmax_cross_entropy_with_logits (logits = y, labels = y_))
training_step = tf.train.AdamOptimizer (learning_rate = 0.01).minimize (cost_function)

with tf.Session() as sess :
	sess.run (init)
	mse_history = []
	accuracy_history = []

	for epoch in range (epochs) :
		sess.run (training_step, feed_dict = {x : train_x, y_ : train_y})
		cost = sess.run (cost_function, feed_dict = {x : train_x, y_ : train_y})
		cost_history = np.append (cost_history, cost)
		correct_prediction = tf.equal (tf.argmax (y, 1), tf.argmax (y_, 1))
		accuracy = tf.reduce_mean (tf.cast (correct_prediction, tf.float32))

		pred_y = sess.run (y, feed_dict = {x : test_x})
		mse = tf.reduce_mean (tf.square (pred_y - test_y))
		mse_ = sess.run (mse)
		mse_history.append (mse_)
		accuracy = sess.run (accuracy, feed_dict = {x : train_x, y_ : train_y})
		accuracy_history.append (accuracy)

		print ('epoch : ', epoch, ' - ', 'cost : ', cost, ' - MSE : ', mse_, " - Train Accuracy : ", accuracy)

	save_path = saver.save (sess, model_path)
	print ("Model saved in file : ", save_path)

	plt.plot (mse_history, 'r')
	plt.show ()
	plt.plot (accuracy_history)
	plt.show()

	correct_prediction = tf.equal (tf.argmax (y, 1), tf.argmax (y_, 1))
	accuracy = tf.reduce_mean (tf.cast (correct_prediction, tf.float32))
	print ("Test Accuracy : ", sess.run (accuracy, feed_dict = {x : test_x, y : test_y}))

	pred_y = sess.run (y, feed_dict = {x : test_x})
	mse = tf.reduce_mean (tf.square (pred_y - test_y))
	print ("MSE : ", sess.run (mse))