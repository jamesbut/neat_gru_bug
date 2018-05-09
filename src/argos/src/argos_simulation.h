#ifndef ARGOS_SIMULATION_H
#define ARGOS_SIMULATION_H

#include <NEAT_GRU/include/organism.h>

class ARGoS_simulation {

public:

   ARGoS_simulation();
   ~ARGoS_simulation();

   void run(NEAT::Organism &org, bool parallel);

   void launch_argos(int individual);

private:

   std::string ARGOS_FILE_NAME;

   int m_individual;

};

#endif
