#ifndef ARGOS_SIMULATION_H
#define ARGOS_SIMULATION_H

#include <NEAT_GRU/include/organism.h>

class ARGoS_simulation {

public:

   ARGoS_simulation();
   ~ARGoS_simulation();

   void run(NEAT::Organism &org);

private:

   std::string ARGOS_FILE_NAME;

};

#endif
