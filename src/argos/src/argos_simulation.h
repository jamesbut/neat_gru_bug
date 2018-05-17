#ifndef ARGOS_SIMULATION_H
#define ARGOS_SIMULATION_H

#include <NEAT_GRU/include/organism.h>

class ARGoS_simulation {

public:

   ARGoS_simulation(const std::string& argos_file);
   ~ARGoS_simulation();

   //Runs the experiment and returns the fitness score
   double run(NEAT::Organism &org, std::string env_path, bool reset, bool indv_run);

private:

   const std::string ARGOS_FILE_NAME;

   int m_individual;

};

#endif
