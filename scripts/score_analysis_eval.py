from numpy import genfromtxt
import numpy as np
import matplotlib.pyplot as plt
import sys

#Read data
dir_name = sys.argv[1]
file_num = sys.argv[2]
test_file_data = genfromtxt('../scores/eval_scores/archive_scores/' + dir_name + '/' + dir_name + '_eval_0_' + file_num + '.txt', delimiter=',')
#test_file_data = genfromtxt('../scores/eval_scores/archive_scores/ap_22_f7_fw/ap_22_f7_fw_eval_0_2.txt', delimiter=',')

#Sort data
test_generations = test_file_data[:,0]

test_num_finishes = test_file_data[:,1]

traj_per_astar = test_file_data[:,2]

#Get all individual test fitnesses
#test_scores = test_file_data[:,2:]
test_mean_scores = test_file_data[:,3]

#Max test scores
#test_max_scores = np.amax(test_scores, axis=1)

#Mean test scores
#test_mean_scores = np.mean(test_scores, axis=1)

#print(test_scores)
print(test_mean_scores)
print(np.mean(test_mean_scores))
#print(len(test_mean_scores))
#print(test_max_scores)

#Plot graphs
#plt.plot(test_generations, test_max_scores)
plt.plot(test_generations, test_mean_scores)
plt.ylabel('Agent fitnesses')

#Set figure size for saving
figure = plt.gcf() # get current figure

#Set figure sizes
pixel_width = 1855
pixel_height = 1022
DPI = 96

#Save figure to file
figure.set_size_inches(pixel_width/DPI, pixel_height/DPI)
plt.savefig('../scores/eval_scores/archive_graphs/' + dir_name + '/' + dir_name + '_scores_0_' + file_num + '.png', bbox_inches='tight')
#plt.show()

plt.gcf().clear()

#Plot number of finishes
plt.plot(test_generations, test_num_finishes)
plt.ylabel('Number of finishes')
#plt.show()

#Save figure to file
figure.set_size_inches(pixel_width/DPI, pixel_height/DPI)
plt.savefig('../scores/eval_scores/archive_graphs/' + dir_name + '/' + dir_name + '_numfinishes_0_' + file_num + '.png', bbox_inches='tight')
#plt.show()
