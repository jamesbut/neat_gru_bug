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

   //std::string GetArgosFilePath(bool handwritten_envs);

   NEAT::Organism* org;

   ARGoS_simulation* as;

   const int NUM_RUNS;
   const std::string ENV_PATH;
   const bool RANDOM_ENVS;
   const bool HANDWRITTEN_ENVS;

};

#endif
