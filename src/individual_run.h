#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "argos/src/argos_simulation.h"

class IndividualRun {

public:

   IndividualRun(const std::string& gf);
   ~IndividualRun();

   void run();

private:

   void readGenomeFile(const std::string& gf);

   NEAT::Organism* org;

   ARGoS_simulation as;

   const int NUM_RUNS;

};

#endif
