#ifndef GA_H
#define GA_H

#include <NEAT_GRU/include/population.h>
#include "argos/src/argos_simulation.h"
#include "data_collection.h"

/*
 * A simple GA using NEAT.
 *
 */

class GA {

public:

   GA(std::string neat_param_file);

   ~GA();

   void run();

private:

   void initNEAT(std::string neat_param_file);

   bool loadNEATParams(std::string neat_file);

   bool done() const;

   void epoch();
   void parallel_epoch();

   void nextGen();

   // void flush_winners();

   // void test_on_training_set(std::vector<std::vector<RunResult> > gen_results);
   // void test_on_eval_set();

   /** Current generation */
   int m_unCurrentGeneration;

   //Current population
   NEAT::Population* neatPop;

   ARGoS_simulation* as;

   //Keep track of overall winner over all generations.
   //I do this because it is often the case that the winner of the final
   //generation is not the overall winner of all the generations.
   //NEAT::Organism* overall_winner;
   //int overall_winner_num_finishes;
   //std::vector<RunResult> m_overall_winner_results;

   //Also keep track of other winners for test set evaluations
   // NEAT::Organism* gen_n_1;
   // NEAT::Organism* gen_n_2;
   // NEAT::Organism* gen_n_3;
   // NEAT::Organism* gen_nminus1_1;
   // NEAT::Organism* gen_nminus1_2;
   // NEAT::Organism* gen_nminus2_1;

   std::vector<int> flush_gens;

   const int NUM_FLUSHES;
   const int INCREMENTAL_EV;

   const int FLUSH_EVERY;

   const bool PARALLEL;
   const std::string ENV_PATH;

   const bool HANDWRITTEN_ENVS;
   const bool RANDOMLY_GENERATED_ENVS;

   //const double ACCEPTABLE_FITNESS;

   const int TEST_EVAL_GEN;
   const std::string TEST_SET_PATH;
   const int NUM_TEST_ENVS;

   //const std::string ARGOS_FILE_NAME;
   //const std::string ARGOS_FILE_NAME_10;

   // int eval_set_finishes_overall_winner;
   // std::vector<RunResult> eval_set_results_overall_winner;
   //bool overall_winner_change_since_last_eval;

   //Data collection
   DataCollection* data_collection;

   EnvironmentGenerator eg;


   //Class for shared memory management
   class SharedMem {

   public:

      SharedMem(int population_size, int num_trials);
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

   //Shared memory object
   SharedMem* shared_mem;

   //Slave pid's
   std::vector<pid_t> slave_PIDs;

};

#endif
