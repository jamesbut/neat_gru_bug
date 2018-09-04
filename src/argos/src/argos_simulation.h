#ifndef ARGOS_SIMULATION_H
#define ARGOS_SIMULATION_H

#include <NEAT_GRU/include/organism.h>
#include "loop_functions/fitness/fitness_score.h"
#include "loop_functions/environment/environment_generator.h"

class ARGoS_simulation {

public:

   ARGoS_simulation();
   ARGoS_simulation(const std::string& argos_file);
   ~ARGoS_simulation();

   //Runs the experiment and returns the fitness score
   // RunResult run(NEAT::Organism &org, std::string env_path, int env_num,
   //    bool reset, bool indv_run, bool handwritten, bool test_envs, int trial_num,
   //    int rand_seed);
   RunResult run(NEAT::Organism &org, int env_num,
                 bool reset, bool indv_run, bool handwritten,
                 bool test_envs, int trial_num, EnvironmentGenerator& env_generator);

private:

   const std::string ARGOS_FILE_NAME;

};

#endif
