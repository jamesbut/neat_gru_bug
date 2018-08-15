#ifndef DATA_COLLECTION_H_
#define DATA_COLLECTION_H_

#include <vector>
#include "argos/src/loop_functions/fitness_score.h"
#include <NEAT_GRU/include/population.h>
#include "argos/src/argos_simulation.h"

class DataCollection {

public:

   DataCollection(const bool RANDOMLY_GENERATED_ENVS,
                  const bool HANDWRITTEN_ENVS,
                  const bool TEST_EVAL_GEN,
                  const bool NUM_TEST_ENVS,
                  const int FLUSH_EVERY,
                  const std::string TEST_SET_PATH,
                  ARGoS_simulation* argos_simulation);

   void collect_scores(std::vector<std::vector <RunResult> > trial_results,
                       NEAT::Population* neatPop,
                       int current_gen);

   void flush_winners(int current_gen);


private:

   void test_on_training_set(std::vector<std::vector<RunResult> > gen_results,
                             int current_gen);
   void test_on_eval_set(int current_gen);

   ARGoS_simulation* as;

   const bool RANDOMLY_GENERATED_ENVS;
   const bool HANDWRITTEN_ENVS;
   const bool TEST_EVAL_GEN;
   const bool NUM_TEST_ENVS;
   const int FLUSH_EVERY;
   const std::string TEST_SET_PATH;

   NEAT::Organism* overall_winner;

   bool overall_winner_change_since_last_eval;

   //Also keep track of other winners for test set evaluations
   NEAT::Organism* gen_n_1;
   NEAT::Organism* gen_n_2;
   NEAT::Organism* gen_n_3;
   NEAT::Organism* gen_nminus1_1;
   NEAT::Organism* gen_nminus1_2;
   NEAT::Organism* gen_nminus2_1;

};

#endif
