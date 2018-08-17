#include "individual_run.h"
#include "ga.h"

#include "argos/src/loop_functions/fitness/astar_on_env.h"

int main(int argc, char *argv[]) {

   //std::vector<CVector2> path = astar_on_env("../argos_params/environments/kim_envs/rand_env_1.png");

   // Seed random number generator with time
   srand((unsigned)time(NULL));

   //Generate starting genome file with bash script
   system("/bin/bash -c ../starting_genomes/generate_start_genome.sh");

   if(argv[1] == NULL) {

      std::cout << "Please provide valid command line arguments" << std::endl;
      return 0;

   }

   //Get either an individual genome file or a NEAT .ne file from cl args
   std::string argString = argv[1];

   if(argString.find(".ne") != std::string::npos) {

      GA ga(argString);
      ga.run();

   } else {

      IndividualRun ir(argString);
      ir.run();

   }

}
