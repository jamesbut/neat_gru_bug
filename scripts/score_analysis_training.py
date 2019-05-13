from numpy import genfromtxt
import matplotlib.pyplot as plt
import sys

#Read data
dir_name = sys.argv[1]
file_num = sys.argv[2]
train_file_data = genfromtxt('../scores/training_scores/archive_scores/' + dir_name + '/' + dir_name + '_training_' + file_num + '.txt', delimiter=',')
#train_file_data = genfromtxt('../scores/training_scores/archive_scores/ap_22_f7_fw/ap_22_f7_fw_training_2.txt', delimiter=',')

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

#Set figure size for saving
figure = plt.gcf() # get current figure

#Set figure sizes
pixel_width = 1855
pixel_height = 1022
DPI = 96

#Save figure to file
figure.set_size_inches(pixel_width/DPI, pixel_height/DPI)
plt.savefig('../scores/training_scores/archive_graphs/' + dir_name + '/' + dir_name + '_scores_' + file_num + '.png', bbox_inches='tight')
#plt.show()

plt.gcf().clear()

plt.plot(train_generations, train_max_num_finishes)
plt.plot(train_generations, train_mean_num_finishes)
plt.ylabel('Num Finishes')

#Save figure to file
figure.set_size_inches(pixel_width/DPI, pixel_height/DPI)
plt.savefig('../scores/training_scores/archive_graphs/' + dir_name + '/' + dir_name + '_numfinishes_' + file_num + '.png', bbox_inches='tight')
#plt.show()
