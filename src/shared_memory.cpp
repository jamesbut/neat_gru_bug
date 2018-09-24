#include "shared_memory.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

//#include <thread>

SharedMem::SharedMem(int population_size, int num_trials, std::string const PREFIX) :
   SHARED_MEMORY_FILE_FITNESS("/" + PREFIX + "_SHARED_MEMORY_FITNESS"),
   SHARED_MEMORY_FILE_GOT_TO_TOWER("/" + PREFIX + "_SHARED_MEMORY_GOT_TO_TOWER"),
   SHARED_MEMORY_FILE_DISTANCE_FROM_TOWER_W_CRASH("/" + PREFIX + "_SHARED_MEMORY_DISTANCE_FROM_TOWER_W_CRASH"),
   SHARED_MEMORY_FILE_TRAJ_PER_ASTAR("/" + PREFIX + "_SHARED_MEMORY_TRAJ_PER_ASTAR"),
   m_popSize(population_size),
   m_numTrials(num_trials) {

   //Create shared mem file descriptor
   m_sharedMemFD_fitness = ::shm_open(SHARED_MEMORY_FILE_FITNESS.c_str(),
                               O_RDWR | O_CREAT,
                               S_IRUSR | S_IWUSR);

   //Create shared mem file descriptor
   m_sharedMemFD_got_to_tower = ::shm_open(SHARED_MEMORY_FILE_GOT_TO_TOWER.c_str(),
                             O_RDWR | O_CREAT,
                             S_IRUSR | S_IWUSR);

   m_sharedMemFD_distance_from_tower_w_crash = ::shm_open(SHARED_MEMORY_FILE_DISTANCE_FROM_TOWER_W_CRASH.c_str(),
                             O_RDWR | O_CREAT,
                             S_IRUSR | S_IWUSR);

   m_sharedMemFD_traj_per_astar = ::shm_open(SHARED_MEMORY_FILE_TRAJ_PER_ASTAR.c_str(),
                              O_RDWR | O_CREAT,
                              S_IRUSR | S_IWUSR);

   //Check it has been initialised correctly
   if(m_sharedMemFD_fitness < 0) {
      ::perror(SHARED_MEMORY_FILE_FITNESS.c_str());
      exit(1);
   }

   if(m_sharedMemFD_got_to_tower < 0) {
      ::perror(SHARED_MEMORY_FILE_GOT_TO_TOWER.c_str());
      exit(1);
   }

   if(m_sharedMemFD_distance_from_tower_w_crash < 0) {
      ::perror(SHARED_MEMORY_FILE_DISTANCE_FROM_TOWER_W_CRASH.c_str());
      exit(1);
   }

   if(m_sharedMemFD_traj_per_astar < 0) {
      ::perror(SHARED_MEMORY_FILE_TRAJ_PER_ASTAR.c_str());
      exit(1);
   }

   //Resize
   size_t mem_size_fitness = m_popSize * m_numTrials * sizeof(double);
   ::ftruncate(m_sharedMemFD_fitness, mem_size_fitness);

   size_t mem_size_got_to_tower = m_popSize * m_numTrials * sizeof(bool);
   ::ftruncate(m_sharedMemFD_got_to_tower, mem_size_got_to_tower);

   size_t mem_size_distance_from_tower_w_crash = m_popSize * m_numTrials * sizeof(double);
   ::ftruncate(m_sharedMemFD_distance_from_tower_w_crash, mem_size_distance_from_tower_w_crash);

   size_t mem_size_traj_per_astar = m_popSize * m_numTrials * sizeof(double);
   ::ftruncate(m_sharedMemFD_traj_per_astar, mem_size_traj_per_astar);

   //Get pointer
   m_sharedMem_fitness = reinterpret_cast<double*>(
      ::mmap(NULL,
             mem_size_fitness,
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             m_sharedMemFD_fitness,
             0));

   m_sharedMem_got_to_tower = reinterpret_cast<bool*>(
    ::mmap(NULL,
           mem_size_got_to_tower,
           PROT_READ | PROT_WRITE,
           MAP_SHARED,
           m_sharedMemFD_got_to_tower,
           0));

   m_sharedMem_distance_from_tower_w_crash = reinterpret_cast<double*>(
   ::mmap(NULL,
          mem_size_distance_from_tower_w_crash,
          PROT_READ | PROT_WRITE,
          MAP_SHARED,
          m_sharedMemFD_distance_from_tower_w_crash,
          0));

   m_sharedMem_traj_per_astar = reinterpret_cast<double*>(
   ::mmap(NULL,
        mem_size_traj_per_astar,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        m_sharedMemFD_traj_per_astar,
        0));

   //Check for failure
   if(m_sharedMem_fitness == MAP_FAILED) {
      ::perror("shared memory fitness");
      exit(1);
   }

   if(m_sharedMem_got_to_tower == MAP_FAILED) {
      ::perror("shared memory result");
      exit(1);
   }

   if(m_sharedMem_distance_from_tower_w_crash == MAP_FAILED) {
      ::perror("shared memory result");
      exit(1);
   }

   if(m_sharedMem_traj_per_astar == MAP_FAILED) {
      ::perror("shared memory result");
      exit(1);
   }

}

SharedMem::~SharedMem() {

   munmap(m_sharedMem_fitness, m_popSize * sizeof(double));
   close(m_sharedMemFD_fitness);
   shm_unlink(SHARED_MEMORY_FILE_FITNESS.c_str());

   munmap(m_sharedMem_got_to_tower, m_popSize * sizeof(bool));
   close(m_sharedMemFD_got_to_tower);
   shm_unlink(SHARED_MEMORY_FILE_GOT_TO_TOWER.c_str());

   munmap(m_sharedMem_distance_from_tower_w_crash, m_popSize * sizeof(double));
   close(m_sharedMemFD_distance_from_tower_w_crash);
   shm_unlink(SHARED_MEMORY_FILE_DISTANCE_FROM_TOWER_W_CRASH.c_str());

   munmap(m_sharedMem_traj_per_astar, m_popSize * sizeof(double));
   close(m_sharedMemFD_traj_per_astar);
   shm_unlink(SHARED_MEMORY_FILE_TRAJ_PER_ASTAR.c_str());

}

std::vector<RunResult> SharedMem::get_run_result(int individual) {

   std::vector<RunResult> run_results;

   for(size_t i = 0; i < m_numTrials; i++) {

      RunResult rr;
      rr.fitness = m_sharedMem_fitness[individual * m_numTrials + i];
      rr.got_to_tower = m_sharedMem_got_to_tower[individual * m_numTrials + i];
      rr.distance_from_tower_w_crash = m_sharedMem_distance_from_tower_w_crash[individual * m_numTrials + i];
      rr.traj_per_astar = m_sharedMem_traj_per_astar[individual * m_numTrials + i];

      run_results.push_back(rr);

   }

   return run_results;

}

void SharedMem::set_run_result(int individual, int trial_num, RunResult run_result) {

   m_sharedMem_fitness[individual * m_numTrials + trial_num] = run_result.fitness;
   m_sharedMem_got_to_tower[individual * m_numTrials + trial_num] = run_result.got_to_tower;
   m_sharedMem_distance_from_tower_w_crash[individual * m_numTrials + trial_num] = run_result.distance_from_tower_w_crash;
   m_sharedMem_traj_per_astar[individual * m_numTrials + trial_num] = run_result.traj_per_astar;

}
