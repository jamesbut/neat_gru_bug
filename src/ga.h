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

   double getTrialScore(int trial, int org);

   bool loadNEATParams(std::string neat_file);

   void cleanup() {};

   void epoch();
   void parallel_epoch();

   void nextGen();

   bool done() const;

   /** Current generation */
   int m_unCurrentGeneration;

   //Current population
   NEAT::Population* neatPop;

   //Start genome for population
   NEAT::Genome* start_genome;

   ARGoS_simulation as;

   //Keep track of overall winner over all generations.
   //I do this because it is often the case that the winner of the final
   //generation is not the overall winner of all the generations.
   NEAT::Organism* winner;

   // Set up vector of scores for each trial
   std::vector<double> trialScores;

   std::vector<int> flush_gens;

   const int NUM_ROBOTS;
   const int NUM_FLUSHES;
   const int MUTATING_START;

   const bool PARALLEL;


   //Class for shared memory management
   class SharedMem {

   public:

      SharedMem(int population_size);
      ~SharedMem();

      double get_fitness(int individual);
      void set_fitness(int individual, double fitness);

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

   };

   //Shared memory object
   SharedMem* shared_mem;

   //Slave pid's
   std::vector<pid_t> slave_PIDs;

};

#endif
