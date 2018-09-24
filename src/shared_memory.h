#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

#include <vector>
#include <string>
#include "argos/src/loop_functions/fitness/fitness_score.h"

class SharedMem {

public:

   SharedMem(int population_size, int num_trials, const std::string PREFIX);
   ~SharedMem();

   std::vector<RunResult> get_run_result(int individual);
   void set_run_result(int individual, int trial_num, RunResult run_result);

private:

   // File name for shared memory area
   const std::string SHARED_MEMORY_FILE_FITNESS;
   const std::string SHARED_MEMORY_FILE_GOT_TO_TOWER;
   const std::string SHARED_MEMORY_FILE_DISTANCE_FROM_TOWER_W_CRASH;
   const std::string SHARED_MEMORY_FILE_TRAJ_PER_ASTAR;

   // Shared mem file descriptor
   int m_sharedMemFD_fitness;
   int m_sharedMemFD_got_to_tower;
   int m_sharedMemFD_distance_from_tower_w_crash;
   int m_sharedMemFD_traj_per_astar;

   // Shared mem pointer
   double* m_sharedMem_fitness;
   bool* m_sharedMem_got_to_tower;
   double* m_sharedMem_distance_from_tower_w_crash;
   double* m_sharedMem_traj_per_astar;

   //Population size - this is stored in order to unmap the memory
   //on destruction of the object
   int m_popSize;
   int m_numTrials;

};

#endif
