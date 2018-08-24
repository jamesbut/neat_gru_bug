#include "individual_run.h"
#include "ga.h"

//All debugging includes
#include "argos/src/loop_functions/fitness/astar_on_env.h"

#include <chrono>

using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using namespace std::literals::chrono_literals;

int main(int argc, char *argv[]) {

   // time_point<Clock> start = Clock::now();
   // astar_on_env("../argos_params/environments/kim_envs/rand_env_1.png");
   // time_point<Clock> end = Clock::now();
   // milliseconds diff = duration_cast<milliseconds>(end - start);
   // std::cout << diff.count() << "ms" << std::endl;

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
