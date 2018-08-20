from numpy import genfromtxt
import matplotlib.pyplot as plt
import numpy as np

#Read data
test_file_data = genfromtxt('../scores/eval_scores/eval_scores_0.txt', delimiter=',')

#Sort data
test_generations = test_file_data[:,0]

test_num_finishes = test_file_data[:,1]

#Get all individual test fitnesses
test_scores = test_file_data[:,2:]

#Max test scores
test_max_scores = np.amax(test_scores, axis=1)

#Mean test scores
test_mean_scores = np.mean(test_scores, axis=1)

#print(test_scores)
#print(test_mean_scores)
#print(len(test_mean_scores))
#print(test_max_scores)

#Plot fitnesses
plt.plot(test_generations, test_max_scores)
plt.plot(test_generations, test_mean_scores)
plt.ylabel('Agent fitnesses')
plt.show()

#Plot number of finishes
plt.plot(test_generations, test_num_finishes)
plt.ylabel('Number of finishes')
plt.show()
