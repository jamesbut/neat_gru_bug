#ifndef ARGOS_SIMULATION_H
#define ARGOS_SIMULATION_H

#include <NEAT_GRU/include/organism.h>
#include "loop_functions/fitness_score.h"

class ARGoS_simulation {

public:

   ARGoS_simulation();
   ARGoS_simulation(const std::string& argos_file);
   ~ARGoS_simulation();

   //Runs the experiment and returns the fitness score
   RunResult run(NEAT::Organism &org, std::string env_path, int env_num,
      bool reset, bool indv_run, bool handwritten, int trial_num, int rand_seed);

private:

   const std::string ARGOS_FILE_NAME;

   int m_individual;

};

#endif
