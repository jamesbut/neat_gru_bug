from numpy import genfromtxt
import matplotlib.pyplot as plt
import sys
import pylab

#Read data
dir_name_gru = 'ap_35_f8_fw_fts'
dir_name_non_gru = 'ap_39_f8_fw'
file_num_start_gru = 18
file_num_end_gru = 38
file_num_start_non_gru = 10
file_num_end_non_gru = 30

eval_file_data_gru = []

for i in range(file_num_start_gru, file_num_end_gru):
    eval_file_data_gru.append(genfromtxt('../scores/eval_scores/archive_scores/' + dir_name_gru + '/' + dir_name_gru + '_eval_0_' + str(i) + '.txt', delimiter=','))

eval_file_data_non_gru = []

for i in range(file_num_start_non_gru, file_num_end_non_gru):
    eval_file_data_non_gru.append(genfromtxt('../scores/eval_scores/archive_scores/' + dir_name_non_gru + '/' + dir_name_non_gru + '_eval_0_' + str(i) + '.txt', delimiter=','))

#GRU mean scores averaged
avg_max_scores_gru = []

for i in range(0, len(eval_file_data_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(eval_file_data_gru)):
        max_scores_per_gen.append(eval_file_data_gru[j][i][3])

    avg_max_scores_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#non-GRU mean scores averaged
avg_max_scores_non_gru = []

for i in range(0, len(eval_file_data_non_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(eval_file_data_non_gru)):
        max_scores_per_gen.append(eval_file_data_non_gru[j][i][3])

    avg_max_scores_non_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#Get data
eval_generations = eval_file_data_gru[0][:,0]

#Sort data
# test_generations = test_file_data[:,0]
#
# test_num_finishes = test_file_data[:,1]
#
# traj_per_astar = test_file_data[:,2]
#
# #Get all individual test fitnesses
# #test_scores = test_file_data[:,2:]
# test_mean_scores = test_file_data[:,3]

#Max test scores
#test_max_scores = np.amax(test_scores, axis=1)

#Mean test scores
#test_mean_scores = np.mean(test_scores, axis=1)

#print(test_scores)
#print(test_mean_scores)
#print(np.mean(test_mean_scores))
#print(len(test_mean_scores))
#print(test_max_scores)

#Plot graphs
#plt.plot(test_generations, test_max_scores)
plt.plot(eval_generations, avg_max_scores_gru, label='Max scores GRU')
plt.plot(eval_generations, avg_max_scores_non_gru, label='Max scores non-GRU')
plt.ylabel('Agent fitnesses')
plt.xlabel('Generation')
pylab.legend(loc='upper left')
plt.show()

#Set figure size for saving
# figure = plt.gcf() # get current figure
#
# #Set figure sizes
# pixel_width = 1855
# pixel_height = 1022
# DPI = 96
#
# #Save figure to file
# figure.set_size_inches(pixel_width/DPI, pixel_height/DPI)
# plt.savefig('../scores/eval_scores/archive_graphs/' + dir_name + '/' + dir_name + '_scores_0_' + file_num + '.png', bbox_inches='tight')
# #plt.show()
#
# plt.gcf().clear()
#
# #Plot number of finishes
# plt.plot(test_generations, test_num_finishes)
# plt.ylabel('Number of finishes')
# #plt.show()
#
# #Save figure to file
# figure.set_size_inches(pixel_width/DPI, pixel_height/DPI)
# plt.savefig('../scores/eval_scores/archive_graphs/' + dir_name + '/' + dir_name + '_numfinishes_0_' + file_num + '.png', bbox_inches='tight')
# #plt.show()
