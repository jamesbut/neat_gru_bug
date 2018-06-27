from numpy import genfromtxt
import matplotlib.pyplot as plt

#Read data
train_file_data = genfromtxt('../scores/training_scores.txt', delimiter=',')
test_file_data = genfromtxt('../scores/eval_scores.txt', delimiter=',')

#Sort data
train_generations = train_file_data[:,0].astype(int)
test_generations = test_file_data[:,0].astype(int)

train_num_finishes = train_file_data[:,1].astype(int)
test_num_finishes = test_file_data[:,1].astype(int)

train_scores = train_file_data[:,2:]
test_scores = train_file_data[:,2:]

#Plot graphs
plt.plot(train_generations, train_num_finishes, '--bo')
plt.ylabel('Number of finishes')
plt.show()

plt.plot(test_generations, test_num_finishes, '--bo')
plt.ylabel('Number of finishes')
plt.show()
