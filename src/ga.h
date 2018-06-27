#ifndef GA_H
#define GA_H

#include <NEAT_GRU/include/population.h>
#include "argos/src/argos_simulation.h"

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

   void collect_scores(std::vector<std::vector <double> > trial_scores);
   void flush_winners();

   void test_on_training_set(std::vector<double> overall_winner_scores, bool changed);
   void test_on_eval_set(bool changed);

   /** Current generation */
   int m_unCurrentGeneration;

   //Current population
   NEAT::Population* neatPop;

   ARGoS_simulation* as;

   //Keep track of overall winner over all generations.
   //I do this because it is often the case that the winner of the final
   //generation is not the overall winner of all the generations.
   NEAT::Organism* overall_winner;
   int overall_winner_num_finishes;
   std::vector<double> m_overall_winner_scores;

   std::vector<int> flush_gens;

   const int NUM_FLUSHES;
   const int INCREMENTAL_EV;

   const int FLUSH_EVERY;

   const bool PARALLEL;
   const std::string ENV_PATH;

   const bool HANDWRITTEN_ENVS;

   const double ACCEPTABLE_FITNESS;
   const double GOT_TO_TOWER_DIST;

   const int TEST_EVAL_GEN;
   const std::string TEST_SET_PATH;
   const int NUM_TEST_ENVS;

   int eval_set_finishes;
   std::vector<double> eval_set_scores;
   bool change_since_last_eval;


   //Class for shared memory management
   class SharedMem {

   public:

      SharedMem(int population_size, int num_trials);
      ~SharedMem();

      std::vector<double> get_fitness(int individual);
      void set_fitness(int individual, int trial_num, double fitness);

   private:

      // File name for shared memory area
      const std::string SHARED_MEMORY_FILE;

      // Shared mem file descriptor
      int m_sharedMemFD;

      // Shared mem pointer
      double* m_sharedMem;

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
