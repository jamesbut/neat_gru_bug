from numpy import genfromtxt
import numpy
import matplotlib.pyplot as plt
import sys
import pylab

#Read data
dir_name_gru = 'ap_4_nb'
dir_name_non_gru = 'ap_5_nb'
file_num_start_gru = 1
file_num_end_gru = 10
file_num_start_non_gru = 1
file_num_end_non_gru = 2

train_file_data_gru = []

for i in range(file_num_start_gru, file_num_end_gru):
    train_file_data_gru.append(genfromtxt('../scores/training_scores/' + dir_name_gru + '/t_' + str(i) + '.txt', delimiter=','))

train_file_data_non_gru = []

for i in range(file_num_start_non_gru, file_num_end_non_gru):
    train_file_data_non_gru.append(genfromtxt('../scores/training_scores/' + dir_name_non_gru + '/t_' + str(i) + '.txt', delimiter=','))

#Get data
train_generations = numpy.arange(1,5001)

mod_train_file_data_gru = []

#Append final number on those that never reached 5000 generations
for i in range(0, len(train_file_data_gru)):
    zero = numpy.zeros((5000, 7))
    zero[:min(5000, len(train_file_data_gru[i])),:] = train_file_data_gru[i][:min(5000, len(train_file_data_gru[i])),:]
    last = train_file_data_gru[i][-1]
    for j in range(len(train_file_data_gru[i]), 5000):
        zero[j,:] = last
    mod_train_file_data_gru.append(zero)

mod_train_file_data_non_gru = []

for i in range(0, len(train_file_data_non_gru)):
    zero = numpy.zeros((5000, 7))
    zero[:min(5000, len(train_file_data_non_gru[i])),:] = train_file_data_non_gru[i][:min(5000, len(train_file_data_non_gru[i])),:]
    last = train_file_data_non_gru[i][-1]
    for j in range(len(train_file_data_non_gru[i]), 5000):
        zero[j,:] = last
    mod_train_file_data_non_gru.append(zero)

#James - addition
max_scores_data_gru = []

# If next max is less than previous, make it the max
for i in range(0, len(mod_train_file_data_gru)):
    max_scores = []
    max_score_so_far = mod_train_file_data_gru[i][0][1]
    for j in range(0, len(mod_train_file_data_gru[i])):
        if mod_train_file_data_gru[i][j][1] > max_score_so_far:
            max_score_so_far = mod_train_file_data_gru[i][j][1]
        max_scores.append(max_score_so_far)
    max_scores_data_gru.append(max_scores)

max_scores_data_non_gru = []

# If next max is less than previous, make it the max
for i in range(0, len(mod_train_file_data_non_gru)):
    max_scores = []
    max_score_so_far = mod_train_file_data_non_gru[i][0][1]
    for j in range(0, len(mod_train_file_data_non_gru[i])):
        if mod_train_file_data_non_gru[i][j][1] > max_score_so_far:
            max_score_so_far = mod_train_file_data_non_gru[i][j][1]
        max_scores.append(max_score_so_far)
    max_scores_data_non_gru.append(max_scores)


#GRU max scores averaged
# avg_max_scores_gru = []
#
# for i in range(0, len(mod_train_file_data_gru[0])):
#     max_scores_per_gen = []
#     for j in range(0, len(train_file_data_gru)):
#         max_scores_per_gen.append(mod_train_file_data_gru[j][i][1])
#
#     avg_max_scores_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#GRU max scores averaged - ALT!!
avg_max_scores_gru = []

for i in range(0, len(max_scores_data_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(max_scores_data_gru)):
        max_scores_per_gen.append(max_scores_data_gru[j][i])

    avg_max_scores_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

# non-GRU max scores averaged
# avg_max_scores_non_gru = []
#
# for i in range(0, len(train_file_data_non_gru[0])):
#     max_scores_per_gen = []
#     for j in range(0, len(train_file_data_non_gru)):
#         max_scores_per_gen.append(train_file_data_non_gru[j][i][1])
#
#     avg_max_scores_non_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#non-GRU max scores averaged - ALT!!
avg_max_scores_non_gru = []

for i in range(0, len(max_scores_data_non_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(max_scores_data_non_gru)):
        max_scores_per_gen.append(max_scores_data_non_gru[j][i])

    avg_max_scores_non_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#GRU mean scores averaged
# avg_mean_scores_gru = []
#
# for i in range(0, len(train_file_data_gru[0])):
#     max_scores_per_gen = []
#     for j in range(0, len(train_file_data_gru)):
#         max_scores_per_gen.append(train_file_data_gru[j][i][2])
#
#     avg_mean_scores_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

# #non-GRU mean scores averaged
# avg_mean_scores_non_gru = []
#
# for i in range(0, len(train_file_data_non_gru[0])):
#     max_scores_per_gen = []
#     for j in range(0, len(train_file_data_non_gru)):
#         max_scores_per_gen.append(train_file_data_non_gru[j][i][2])
#
#     avg_mean_scores_non_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

plt.rcParams.update({'font.size': 22})
plt.rc('legend', fontsize=20)

plt.plot(train_generations, avg_max_scores_gru, label='Max scores GRU')
plt.plot(train_generations, avg_max_scores_non_gru, label='Max scores non-GRU')
# plt.plot(train_generations, avg_mean_scores_gru, label='Mean scores GRU')
# # plt.plot(train_generations, avg_mean_scores_non_gru, label='Mean scores non-GRU')
pylab.legend(loc='upper left')
plt.ylabel('Agent fitness')
plt.xlabel('Generation')
plt.show()
#
# #GRU max num finishes averaged
# avg_max_num_finishes_gru = []
#
# for i in range(0, len(train_file_data_gru[0])):
#     max_num_finishes_per_gen = []
#     for j in range(0, len(train_file_data_gru)):
#         max_num_finishes_per_gen.append(train_file_data_gru[j][i][3])
#
#     avg_max_num_finishes_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))
#
# #non-GRU max num finishes averaged
# avg_max_num_finishes_non_gru = []
#
# for i in range(0, len(train_file_data_non_gru[0])):
#     max_num_finishes_per_gen = []
#     for j in range(0, len(train_file_data_non_gru)):
#         max_num_finishes_per_gen.append(train_file_data_non_gru[j][i][3])
#
#     avg_max_num_finishes_non_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))
#
# #GRU mean num finishes averaged
# avg_mean_num_finishes_gru = []
#
# for i in range(0, len(train_file_data_gru[0])):
#     max_num_finishes_per_gen = []
#     for j in range(0, len(train_file_data_gru)):
#         max_num_finishes_per_gen.append(train_file_data_gru[j][i][4])
#
#     avg_mean_num_finishes_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))
#
# #non-GRU mean num finishes averaged
# avg_mean_num_finishes_non_gru = []
#
# for i in range(0, len(train_file_data_non_gru[0])):
#     max_num_finishes_per_gen = []
#     for j in range(0, len(train_file_data_non_gru)):
#         max_num_finishes_per_gen.append(train_file_data_non_gru[j][i][4])
#
#     avg_mean_num_finishes_non_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))
#
# plt.plot(train_generations, avg_max_num_finishes_gru, label='Max num finishes GRU')
# plt.plot(train_generations, avg_max_num_finishes_non_gru, label='Max num finishes non-GRU')
# plt.plot(train_generations, avg_mean_num_finishes_gru, label='Mean num finishes GRU')
# plt.plot(train_generations, avg_mean_num_finishes_non_gru, label='Mean num finishes non-GRU')
# pylab.legend(loc='upper left')
# plt.ylabel('Number of Finishes')
# plt.xlabel('Generation')
# plt.show()
