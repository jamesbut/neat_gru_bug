#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "argos/src/argos_simulation.h"

class IndividualRun {

public:

   IndividualRun(const std::string& gf, const bool handwritten);
   ~IndividualRun();

   void run();

private:

   void readGenomeFile(const std::string& gf);

   NEAT::Organism* org;

   ARGoS_simulation* as;

   int NUM_RUNS;
   const std::string ENV_PATH;
   const bool RANDOM_ENVS;
   const bool HANDWRITTEN_ENVS;

   const std::string ARGOS_FILE_NAME;
   const std::string ARGOS_FILE_NAME_10;

   EnvironmentGenerator eg;

};

#endif
