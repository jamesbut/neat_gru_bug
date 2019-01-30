#include "ga.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <thread>

#include "argos/src/loop_functions/environment/environment_generator.h"

GA::GA(std::string neat_param_file, const bool no_bearing) :
   m_unCurrentGeneration(1),
   ARGOS_FILE_NAME("../argos_params/no_walls.argos"),
   ARGOS_FILE_NAME_10("../argos_params/no_walls_10.argos"),
   eg(),
   //NUM_FLUSHES(3),   //Currently not used
   FLUSH_EVERY(1),
   INCREMENTAL_EV(false),
   PARALLEL(true),
   //ACCEPTABLE_FITNESS(13.88),
   NO_BEARING(no_bearing),   //This is now set in main.cpp
   RANDOMLY_GENERATED_ENVS(false),
   NOVELTY_SEARCH(false),
   ns(NULL),
   TEST_EVAL_GEN(1),
   TEST_SET_PATH("../argos_params/environments/kim_envs/rand_env_"),
   //TEST_SET_PATH("../argos_params/environments/training_set/ts_"),
   //NUM_TEST_ENVS(2),
   NUM_TEST_ENVS(209),
   //ENV_PATH("../argos_params/environments/rand_envs_14_3/rand_env_")
   //ENV_PATH("../argos_params/environments/rand_envs_14_2/rand_env_")
   //ENV_PATH("../argos_params/environments/rand_envs_14_2_incr_ev/rand_env_")
   ENV_PATH("../argos_params/environments/training_set/ts_")
   //ENV_PATH("../argos_params/environments/handwritten_envs/e")
   {

   if(NO_BEARING) {

      //Set appropriate start genome
      const int NUM_INPUTS = 1;
      const int NUM_OUTPUTS = 2;

      //Generate start genome
      generate_start_genome(NUM_INPUTS, NUM_OUTPUTS);

      as = new ARGoS_simulation(ARGOS_FILE_NAME_10);
      eg.set_argos_config_file(ARGOS_FILE_NAME_10);

   } else {

      //Set appropriate start genome
      const int NUM_INPUTS = 15;
      const int NUM_OUTPUTS = 2;

      //Generate start genome
      generate_start_genome(NUM_INPUTS, NUM_OUTPUTS);

      as = new ARGoS_simulation(ARGOS_FILE_NAME);
      eg.set_argos_config_file(ARGOS_FILE_NAME);

   }

   initNEAT(neat_param_file);

   //Determine when to flush overall winner to file
   // flush_gens.resize(NUM_FLUSHES);
   //
   // for(int i = 0; i < NUM_FLUSHES; i++) {
   //
   //    flush_gens[i] = (double)(i+1)/(double)NUM_FLUSHES * NEAT::num_gens;
   //    //std::cout << flush_gens[i] << std::endl;
   //
   // }

   //Create shared memory block for master and slaves
   if(PARALLEL) shared_mem = new SharedMem(neatPop->organisms.size(), NEAT::num_trials, "GA");

   if(NOVELTY_SEARCH) {

      //Novelty search params
      const double NOVELTY_THRESHOLD = 6;
      const int K = 6;

      //TODO: Find trial time from somewhere
      ns = new NoveltySearch(NOVELTY_THRESHOLD, K, NEAT::num_trials,
                             neatPop->organisms.size(), 3000);

   }

   //Data collection
   data_collection = new DataCollection(RANDOMLY_GENERATED_ENVS, NO_BEARING, ns,
                                        TEST_EVAL_GEN, NUM_TEST_ENVS, FLUSH_EVERY,
                                        TEST_SET_PATH, as, neatPop->organisms.size());

   //Remove all score files
   system("exec rm ../scores/eval_scores/*");
   system("exec rm ../scores/training_scores/*");

}

GA::~GA() {

   delete neatPop;
   delete data_collection;
   if(PARALLEL) delete shared_mem;

   delete as;

}

void GA::generate_start_genome(const int NUM_INPUTS, const int NUM_OUTPUTS) {

   std::string command = "../starting_genomes/generate_start_genome.sh";
   command += " " + std::to_string(NUM_INPUTS) + " " + std::to_string(NUM_OUTPUTS);
   system(command.c_str());

}

void GA::initNEAT(std::string neat_param_file) {

   std::cout << "Initialising NEAT.." << std::endl;

   if(!loadNEATParams(neat_param_file)) exit (EXIT_FAILURE);

   char curword[20];
   int id;
   std::ifstream iFile;

   if(!INCREMENTAL_EV) iFile = std::ifstream("../starting_genomes/start_genome");
   else iFile = std::ifstream("../winners/archive_winners/g29");

   iFile >> curword;
   iFile >> id;

   std::cout << "Reading in Genome id " << id << std::endl;

   //Initialise starting genome
   NEAT::Genome* start_genome = new NEAT::Genome(id,iFile);
   iFile.close();

   //Spawn the Population
   std::cout << "Spawning Population off Genome" << std::endl;

   if(!INCREMENTAL_EV) neatPop = new NEAT::Population(start_genome,NEAT::pop_size);
   else neatPop = new NEAT::Population(start_genome,NEAT::pop_size,0.0);

   delete start_genome;

   //Verify new population
   std::cout << "Verifying Spawned Pop" << std::endl;

   bool v;
   v = neatPop->verify();

   if (!v) std::cout << "Could not verify population!" << std::endl;

}

//Loads in NEAT parameters for GA
bool GA::loadNEATParams(std::string neat_file) {

   std::string file_prefix = "../neat_params/";
   neat_file = file_prefix + neat_file;

   if(!NEAT::load_neat_params(neat_file.c_str(), true)) {

     std::cerr << "This .ne file does not exist in this directory" << std::endl;
     return false;

   }

  std::cout << "\nParameters Loaded!\n" << std::endl;

  return true;

}

void GA::run() {

   //Start main loop....

   while(!done()) {

      //Evaluate current generation
      if(PARALLEL) {

         parallel_epoch();

      } else {

         epoch();

      }

      //Print organism scores
      std::cout << "Generation #" << m_unCurrentGeneration << "...";
      std::cout << " scores:";

      for(int i = 0; i < neatPop->organisms.size(); ++i) {

         std::cout << " " << neatPop->organisms[i]->fitness;

      }

      std::cout << std::endl;

      nextGen();

    }

}

// Evaluate 1 population
void GA::epoch() {

   std::vector<std::vector <RunResult> > trial_scores(neatPop->organisms.size(), std::vector<RunResult>(NEAT::num_trials));

   //Run individual fitness tests
   for(size_t i = 0; i < NEAT::num_trials; i++) {

      std::string file_name;
      int env_num;
      bool reset = false;
      bool test_envs = false;

      //Create file name and env num
      if(NO_BEARING) {
         file_name = ENV_PATH + "15.png";
         env_num = 15;
      } else if (RANDOMLY_GENERATED_ENVS) {
         file_name = "";
         env_num = i+1;
      } else {
         test_envs = true;
         file_name = ENV_PATH + std::to_string(i+1) + ".png";
         env_num = i+1;
      }

      //Generate environment
      eg.generate_env(file_name, env_num);

      // int rand_seed = rand();

      for(size_t j = 0; j < neatPop->organisms.size(); j++) {

         std::cout << "Organism num: " << j << std::endl;
         std::cout << "Trial num: " << i << std::endl;

         if (j==0 && (!NO_BEARING)) reset = true;

         // trial_scores[j][i] = as->run(*(neatPop->organisms[j]), file_name, env_num,
         //                               reset, false, HANDWRITTEN_ENVS, test_envs, (i+1), rand_seed);

         trial_scores[j][i] = as->run(*(neatPop->organisms[j]), env_num,
                                       reset, false, NO_BEARING, test_envs, (i+1), eg, j);

         reset = false;

      }

   }

   if(NOVELTY_SEARCH) ns->evaluate_population(*neatPop);

   data_collection->collect_scores(trial_scores, neatPop, m_unCurrentGeneration);

   data_collection->flush_winners(m_unCurrentGeneration);

}

//Evalute population in parallel
void GA::parallel_epoch() {

   //Run individual fitness tests
   for(size_t i = 0; i < NEAT::num_trials; i++) {
      //std::cout << "Trial: " << i << std::endl;
      std::string file_name;
      int env_num;
      bool reset = false;
      bool test_envs = false;

      //Create file name and env num
      if(NO_BEARING) {
         file_name = ENV_PATH + "15.png";
         env_num = 15;
      } else if (RANDOMLY_GENERATED_ENVS) {
         file_name = "";
         env_num = i+1;
         reset = true;
      } else {
         test_envs = true;
         file_name = ENV_PATH + std::to_string(i+1) + ".png";
         env_num = i+1;
         reset = true;
      }

      //Generate environment
      eg.generate_env(file_name, env_num);

      unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

      unsigned int num_organisms_tested = 0;

      while(num_organisms_tested < neatPop->organisms.size()) {

         //std::cout << "Organisms tested: " << num_organisms_tested << std::endl;

         unsigned int num_organisms_left = neatPop->organisms.size() - num_organisms_tested;
         unsigned int num_threads_to_spawn = std::min(num_organisms_left, concurentThreadsSupported);

         //std::cout << "Num threads to spawn: " << num_threads_to_spawn << std::endl;

         for(size_t k = 0; k < num_threads_to_spawn; k++) {

            num_organisms_tested++;

            //Spawn slaves
            slave_PIDs.push_back(::fork());

            if(slave_PIDs.back() == 0) {

               shared_mem->set_run_result(num_organisms_tested-1, i, as->run(*(neatPop->organisms[num_organisms_tested-1]),
                                          env_num, reset, false, NO_BEARING, test_envs, (i+1), eg, num_organisms_tested-1));

               //Kill slave with user defined signal
               ::raise(SIGUSR1);

            }

         }

         /* Wait for all the slaves to finish the run */
         int unTrialsLeft = num_threads_to_spawn;
         int nSlaveInfo;
         pid_t tSlavePID;

         while(unTrialsLeft > 0) {

            /* Wait for next slave to finish */
            tSlavePID = ::wait(&nSlaveInfo);

            //std::cout << unTrialsLeft << std::endl;

            //Check for failure
            if (tSlavePID == -1) {
               perror("waitpid");
               exit(EXIT_FAILURE);
            }

            if(WIFSIGNALED(nSlaveInfo))
               //If I didn't terminate slave, print out what did and exit
               if(WTERMSIG(nSlaveInfo) != SIGUSR1) {

                  std::cout << "Terminated with signal: " << WTERMSIG(nSlaveInfo) << std::endl;

                  //Kill all child processes
                  for(size_t i = 0; i < slave_PIDs.size(); i++) {
                     kill(slave_PIDs[i], SIGKILL);
                  }

                  //Kill main process
                  exit(EXIT_FAILURE);

            }

            /* All OK, one less slave to wait for */
            --unTrialsLeft;

         }

         //Clear slave pids
         slave_PIDs.clear();

      }
   }

   //Populate trial scores from shared mem
   std::vector<std::vector <RunResult> > trial_results;

   for(size_t i = 0; i < neatPop->organisms.size(); i++)
      trial_results.push_back(shared_mem->get_run_result(i));

   /*---------------------------------------------------*/

   // for(size_t i = 0; i < trial_results.size(); i++) {
   //    for(size_t j = 0; j < trial_results[i].size(); j++) {
   //       std::cout << trial_results[i][j].fitness << " ";
   //    }
   //    std::cout << std::endl;
   // }
   //
   // std::cout << std::endl;



   /*--------------------------------------------------*/

   if(NOVELTY_SEARCH) ns->evaluate_population(*neatPop);

   data_collection->collect_scores(trial_results, neatPop, m_unCurrentGeneration);

   data_collection->flush_winners(m_unCurrentGeneration);

}

void GA::nextGen() {

   neatPop->epoch(m_unCurrentGeneration);
   ++m_unCurrentGeneration;

}

// Checks that it has got above the number of generations but also that the
// fitness is high enough too
bool GA::done() const {
   //return (m_unCurrentGeneration > NEAT::num_gens) && (overall_winner->fitness > ACCEPTABLE_FITNESS);
   return m_unCurrentGeneration > NEAT::num_gens;
}
