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

   std::string GetArgosFilePath(bool handwritten_envs);

   /** Current generation */
   int m_unCurrentGeneration;

   //Current population
   NEAT::Population* neatPop;

   ARGoS_simulation as;

   //Keep track of overall winner over all generations.
   //I do this because it is often the case that the winner of the final
   //generation is not the overall winner of all the generations.
   NEAT::Organism* overall_winner;

   std::vector<int> flush_gens;

   const int NUM_FLUSHES;
   const int INCREMENTAL_EV;

   const bool PARALLEL;
   const std::string ENV_PATH;

   const bool HANDWRITTEN_ENVS;

   const double ACCEPTABLE_FITNESS;


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
