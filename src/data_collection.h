#ifndef DATA_COLLECTION_H_
#define DATA_COLLECTION_H_

#include <vector>
#include "argos/src/loop_functions/fitness/fitness_score.h"
#include <NEAT_GRU/include/population.h>
#include "argos/src/argos_simulation.h"
#include "shared_memory.h"
#include "nash_averaging.h"
#include <NEAT_GRU/include/novelty_search.h>


class DataCollection {

public:

   DataCollection(const bool RANDOMLY_GENERATED_ENVS,
                  const bool NO_BEARING,
                  NoveltySearch* ns,
                  const int TEST_EVAL_GEN,
                  const int NUM_TEST_ENVS,
                  const int FLUSH_EVERY,
                  const std::string TEST_SET_PATH,
                  ARGoS_simulation* argos_simulation,
                  const int POP_SIZE);

   void collect_scores(const std::vector<std::vector <RunResult> >& trial_results,
                       NEAT::Population* neatPop,
                       int current_gen);

   void flush_winners(int current_gen);


private:

   void test_on_training_set(const std::vector<std::vector<RunResult> >& gen_results,
                             int current_gen);
   void test_on_eval_set(int current_gen);

   std::vector<std::vector <RunResult> > parallel_eval(const std::vector<NEAT::Organism*>);
   std::vector<std::vector <RunResult> > serial_eval(const std::vector<NEAT::Organism*> genomes_to_be_tested);

   double find_median(std::vector<double>& vec);

   ARGoS_simulation* as;

   const bool RANDOMLY_GENERATED_ENVS;
   const bool NO_BEARING;
   const int TEST_EVAL_GEN;
   const int NUM_TEST_ENVS;
   const int FLUSH_EVERY;
   const std::string TEST_SET_PATH;
   const bool NASH_AVERAGING;
   const int POP_SIZE;

   std::unique_ptr<NEAT::Organism> overall_winner;

   bool overall_winner_change_since_last_eval;

   //Also keep track of other winners for test set evaluations
   std::unique_ptr<NEAT::Organism> gen_n_1;
   std::unique_ptr<NEAT::Organism> gen_n_2;
   std::unique_ptr<NEAT::Organism> gen_n_3;
   std::unique_ptr<NEAT::Organism> gen_nminus1_1;
   std::unique_ptr<NEAT::Organism> gen_nminus1_2;
   std::unique_ptr<NEAT::Organism> gen_nminus2_1;

   EnvironmentGenerator eg;

   //Shared memory object
   SharedMem* shared_mem;

   std::vector<pid_t> slave_PIDs;

   NashAverager nash_averager;

   NoveltySearch* m_ns;

};

#endif
