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

train_file_data_gru = []

for i in range(file_num_start_gru, file_num_end_gru):
    train_file_data_gru.append(genfromtxt('../scores/training_scores/archive_scores/' + dir_name_gru + '/' + dir_name_gru + '_training_' + str(i) + '.txt', delimiter=','))

train_file_data_non_gru = []

for i in range(file_num_start_non_gru, file_num_end_non_gru):
    train_file_data_non_gru.append(genfromtxt('../scores/training_scores/archive_scores/' + dir_name_non_gru + '/' + dir_name_non_gru + '_training_' + str(i) + '.txt', delimiter=','))

#Get data
train_generations = train_file_data_gru[0][:,0]

#GRU max scores averaged
avg_max_scores_gru = []

for i in range(0, len(train_file_data_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(train_file_data_gru)):
        max_scores_per_gen.append(train_file_data_gru[j][i][1])

    avg_max_scores_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#non-GRU max scores averaged
avg_max_scores_non_gru = []

for i in range(0, len(train_file_data_non_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(train_file_data_non_gru)):
        max_scores_per_gen.append(train_file_data_non_gru[j][i][1])

    avg_max_scores_non_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#GRU mean scores averaged
avg_mean_scores_gru = []

for i in range(0, len(train_file_data_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(train_file_data_gru)):
        max_scores_per_gen.append(train_file_data_gru[j][i][2])

    avg_mean_scores_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))

#non-GRU mean scores averaged
avg_mean_scores_non_gru = []

for i in range(0, len(train_file_data_non_gru[0])):
    max_scores_per_gen = []
    for j in range(0, len(train_file_data_non_gru)):
        max_scores_per_gen.append(train_file_data_non_gru[j][i][2])

    avg_mean_scores_non_gru.append(sum(max_scores_per_gen) / len(max_scores_per_gen))


plt.rcParams.update({'font.size': 26})
#Old
#plt.rc('legend', fontsize=16)
#Mod
plt.rc('legend', fontsize=20)


#Old graph
#plt.plot(train_generations, avg_max_scores_gru, label='Max scores GRU')
#plt.plot(train_generations, avg_max_scores_non_gru, label='Max scores non-GRU')
#plt.plot(train_generations, avg_mean_scores_gru, label='Mean scores GRU')
#plt.plot(train_generations, avg_mean_scores_non_gru, label='Mean scores non-GRU')

#New mod graph for presentation
plt.plot(train_generations, avg_mean_scores_gru, label='Average population scores: GRU')
plt.plot(train_generations, avg_mean_scores_non_gru, label='Average population scores: non-GRU')

pylab.legend(loc='upper left')
plt.ylabel('Agent fitness')
plt.xlabel('Generation')
plt.show()

#GRU max num finishes averaged
avg_max_num_finishes_gru = []

for i in range(0, len(train_file_data_gru[0])):
    max_num_finishes_per_gen = []
    for j in range(0, len(train_file_data_gru)):
        max_num_finishes_per_gen.append(train_file_data_gru[j][i][3])

    avg_max_num_finishes_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))

#non-GRU max num finishes averaged
avg_max_num_finishes_non_gru = []

for i in range(0, len(train_file_data_non_gru[0])):
    max_num_finishes_per_gen = []
    for j in range(0, len(train_file_data_non_gru)):
        max_num_finishes_per_gen.append(train_file_data_non_gru[j][i][3])

    avg_max_num_finishes_non_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))

#GRU mean num finishes averaged
avg_mean_num_finishes_gru = []

for i in range(0, len(train_file_data_gru[0])):
    max_num_finishes_per_gen = []
    for j in range(0, len(train_file_data_gru)):
        max_num_finishes_per_gen.append(train_file_data_gru[j][i][4])

    avg_mean_num_finishes_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))

#non-GRU mean num finishes averaged
avg_mean_num_finishes_non_gru = []

for i in range(0, len(train_file_data_non_gru[0])):
    max_num_finishes_per_gen = []
    for j in range(0, len(train_file_data_non_gru)):
        max_num_finishes_per_gen.append(train_file_data_non_gru[j][i][4])

    avg_mean_num_finishes_non_gru.append(sum(max_num_finishes_per_gen) / len(max_num_finishes_per_gen))

plt.plot(train_generations, avg_max_num_finishes_gru, label='Max num finishes GRU')
plt.plot(train_generations, avg_max_num_finishes_non_gru, label='Max num finishes non-GRU')
plt.plot(train_generations, avg_mean_num_finishes_gru, label='Mean num finishes GRU')
plt.plot(train_generations, avg_mean_num_finishes_non_gru, label='Mean num finishes non-GRU')
pylab.legend(loc='upper left')
plt.ylabel('Number of Finishes')
plt.xlabel('Generation')
plt.show()
