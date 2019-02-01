#include "individual_run.h"
#include "ga.h"

//All debugging includes
#include "argos/src/loop_functions/fitness/astar_on_env.h"

#include <chrono>

#include "argos/src/loop_functions/environment/environment_generator.h"

using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using namespace std::literals::chrono_literals;

const bool NO_BEARING = true;

int main(int argc, char *argv[]) {

   //time_point<Clock> start = Clock::now();


   // Seed random number generator with time
   srand((unsigned)time(NULL));

   if(argv[1] == NULL) {

      std::cout << "Please provide valid command line arguments" << std::endl;
      return 0;

   }

   //Get either an individual genome file or a NEAT .ne file from cl args
   std::string argString = argv[1];

   if(argString.find(".ne") != std::string::npos) {

      GA ga(argString, NO_BEARING);
      ga.run();

   } else {

      IndividualRun ir(argString, NO_BEARING);
      ir.run();

   }

   // time_point<Clock> end = Clock::now();
   // milliseconds diff = duration_cast<milliseconds>(end - start);
   // std::cout << diff.count() << "ms" << std::endl;

}
