from numpy import genfromtxt
import matplotlib.pyplot as plt

#Read data
#train_file_data = genfromtxt('../scores/training_scores_1.txt', delimiter=',')
train_file_data = genfromtxt('../scores/training_scores/archive_scores/ap_19_fw/ap_19_fw_training_1.txt', delimiter=',')
#train_file_data = genfromtxt('../scores/training_scores/archive_scores/ap_6_f2/ap_6_f2_training_5.txt', delimiter=',')

#Get data
train_generations = train_file_data[:,0]

#Fitness scores
train_max_scores = train_file_data[:,1]
train_mean_scores = train_file_data[:,2]

#Num finishes
train_max_num_finishes = train_file_data[:,3]
train_mean_num_finishes = train_file_data[:,4]

#Plot graphs
#plt.plot(train_generations, train_max_scores, '--bo')
plt.plot(train_generations, train_max_scores)
plt.plot(train_generations, train_mean_scores)
plt.ylabel('Agent fitnesses')
plt.show()

plt.plot(train_generations, train_max_num_finishes)
plt.plot(train_generations, train_mean_num_finishes)
plt.ylabel('Num Finishes')
plt.show()
