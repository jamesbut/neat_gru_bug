
//#include "individual_run.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "ga.h"

int main(int argc, char *argv[]) {

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

      // IndividualRun ir(argString);
      // ir.run();

   }

}
