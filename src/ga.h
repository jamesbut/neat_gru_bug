#ifndef GA_H
#define GA_H

#include <NEAT_GRU/include/population.h>
#include "argos/src/argos_simulation.h"
#include "data_collection.h"
#include "shared_memory.h"

/*
 * A simple GA using NEAT.
 *
 */

class GA {

public:

   GA(std::string neat_param_file, const bool no_bearing);

   ~GA();

   void run();

private:

   void initNEAT(std::string neat_param_file);

   bool loadNEATParams(std::string neat_file);

   bool done() const;

   void epoch();
   void parallel_epoch();

   void nextGen();

   void generate_start_genome(const int NUM_INPUTS, const int NUM_OUTPUTS);

   /** Current generation */
   int m_unCurrentGeneration;

   //Current population
   NEAT::Population* neatPop;

   ARGoS_simulation* as;

   std::vector<int> flush_gens;

   //const int NUM_FLUSHES;
   const int INCREMENTAL_EV;

   const int FLUSH_EVERY;

   const bool PARALLEL;
   const std::string ENV_PATH;

   const bool NO_BEARING;
   const bool RANDOMLY_GENERATED_ENVS;

   const bool NOVELTY_SEARCH;

   //const double ACCEPTABLE_FITNESS;

   const int TEST_EVAL_GEN;
   const std::string TEST_SET_PATH;
   const int NUM_TEST_ENVS;

   const std::string ARGOS_FILE_NAME;
   const std::string ARGOS_FILE_NAME_10;

   //Data collection
   DataCollection* data_collection;

   EnvironmentGenerator eg;

   //Shared memory object
   SharedMem* shared_mem;

   //Slave pid's
   std::vector<pid_t> slave_PIDs;

   NoveltySearch* ns;

};

#endif
