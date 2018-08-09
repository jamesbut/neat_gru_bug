#include "ga.h"
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <numeric>
#include <sstream>
#include <boost/filesystem.hpp>

GA::GA(std::string neat_param_file) :
   m_unCurrentGeneration(1),
   overall_winner_num_finishes(0),
   NUM_FLUSHES(3),   //Currently not used
   FLUSH_EVERY(1),
   INCREMENTAL_EV(false),
   PARALLEL(true),
   ACCEPTABLE_FITNESS(13.88),
   HANDWRITTEN_ENVS(false),
   RANDOMLY_GENERATED_ENVS(true),
   TEST_EVAL_GEN(10),
   TEST_SET_PATH("../argos_params/environments/kim_envs/rand_env_"),
   NUM_TEST_ENVS(209),
   //ENV_PATH("../argos_params/environments/rand_envs_14_3/rand_env_")
   //ENV_PATH("../argos_params/environments/rand_envs_14_2/rand_env_")
   ENV_PATH("../argos_params/environments/rand_envs_14_2_incr_ev/rand_env_")
   //ENV_PATH("../argos_params/environments/training_set/ts_")
   //ENV_PATH("../argos_params/environments/handwritten_envs/e")
   {

   if(HANDWRITTEN_ENVS)
      as = new ARGoS_simulation("../argos_params/no_walls_10.argos");
   else
      as = new ARGoS_simulation("../argos_params/no_walls.argos");

   initNEAT(neat_param_file);

   //Determine when to flush overall winner to file
   flush_gens.resize(NUM_FLUSHES);

   for(int i = 0; i < NUM_FLUSHES; i++) {

      flush_gens[i] = (double)(i+1)/(double)NUM_FLUSHES * NEAT::num_gens;
      //std::cout << flush_gens[i] << std::endl;

   }

   //Create shared memory block for master and slaves
   if(PARALLEL) shared_mem = new SharedMem(neatPop->organisms.size(), NEAT::num_trials);

   //Remove file ready for next run
   remove("../scores/training_scores.txt");
   remove("../scores/eval_scores.txt");

}

GA::~GA() {

   delete neatPop;
   delete overall_winner;
   if(PARALLEL) delete shared_mem;

   delete as;

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

      //Create file name and env num
      if(HANDWRITTEN_ENVS) {
         file_name = ENV_PATH + "15.png";
         env_num = 15;
      } else if (RANDOMLY_GENERATED_ENVS) {
         file_name = "";
         env_num = i+1;
      } else {
         file_name = ENV_PATH + std::to_string(i+1) + ".png";
         env_num = i+1;
      }

      //Create random seed for parallel processes - it is not really needed
      //in the serial version but the method now needs one
      int rand_seed = rand();

      for(size_t j = 0; j < neatPop->organisms.size(); j++) {

         std::cout << "Organism num: " << j << std::endl;
         std::cout << "Trial num: " << i << std::endl;

         if (j==0 && (!HANDWRITTEN_ENVS)) reset = true;

         trial_scores[j][i] = as->run(*(neatPop->organisms[j]), file_name, env_num,
                                       reset, false, HANDWRITTEN_ENVS, (i+1), rand_seed);

         reset = false;

      }

   }

   collect_scores(trial_scores);

   flush_winners();

   //if(!HANDWRITTEN_ENVS) test_on_eval_set(true);

}

//Evalute population in parallel
void GA::parallel_epoch() {

   //Run individual fitness tests
   for(size_t i = 0; i < NEAT::num_trials; i++) {

      std::string file_name;
      int env_num;
      bool reset = false;

      //Create file name and env num
      if(HANDWRITTEN_ENVS) {
         file_name = ENV_PATH + "15.png";
         env_num = 15;
      } else if (RANDOMLY_GENERATED_ENVS) {
         file_name = "";
         env_num = i+1;
         reset = true;
      } else {
         file_name = ENV_PATH + std::to_string(i+1) + ".png";
         env_num = i+1;
         reset = true;
      }

      //Create random seed for parallel processes
      int rand_seed = rand();

      for(size_t j = 0; j < neatPop->organisms.size(); j++) {

         //Spawn slaves
         slave_PIDs.push_back(::fork());

         if(slave_PIDs.back() == 0) {

            shared_mem->set_run_result(j, i, as->run(*(neatPop->organisms[j]), file_name,
                                                      env_num, reset, false, HANDWRITTEN_ENVS,
                                                      (i+1), rand_seed));

            //Kill slave
            ::raise(SIGTERM);

         }

      }

      /* Wait for all the slaves to finish the run */
      int unTrialsLeft = neatPop->organisms.size();
      int nSlaveInfo;
      pid_t tSlavePID;

      while(unTrialsLeft > 0) {

         /* Wait for next slave to finish */
         tSlavePID = ::waitpid(-1, &nSlaveInfo, WUNTRACED);
         // Check for failure
         if(!WIFSIGNALED(nSlaveInfo)) {
            std::cout << "Something weird happened with slave process" << std::endl;
            exit(1);
         }
         /* All OK, one less slave to wait for */
         --unTrialsLeft;

      }

      //Clear slave pids
      slave_PIDs.clear();

   }

   //Populate trial scores from shared mem
   std::vector<std::vector <RunResult> > trial_results;

   for(size_t i = 0; i < neatPop->organisms.size(); i++)
      trial_results.push_back(shared_mem->get_run_result(i));

   collect_scores(trial_results);

   flush_winners();

}

void GA::collect_scores(std::vector<std::vector <RunResult> > trial_results) {

   int maxPopOrg;
   double maxPopScore;

   //Set fitnesses in NEAT
   for(size_t i = 0; i < neatPop->organisms.size(); i++) {

      std::vector<RunResult> individual_trial_run_results = trial_results[i];
      std::vector<double> individual_trial_fitnesses;

      for(int i = 0; i < individual_trial_run_results.size(); i++)
         individual_trial_fitnesses.push_back(individual_trial_run_results[i].fitness);

      //double minTrial = *std::min_element(individual_trial_scores.begin(), individual_trial_scores.end());
      double meanTrialFitness = std::accumulate(individual_trial_fitnesses.begin(), individual_trial_fitnesses.end(), 0.0) / individual_trial_fitnesses.size();
      //double sumTrial = std::accumulate(individual_trial_scores.begin(), individual_trial_scores.end(), 0.0);

      neatPop->organisms[i]->fitness = meanTrialFitness;

      //Find best organism in population
      if (i == 0 || neatPop->organisms[i]->fitness > maxPopScore) {

         maxPopScore = neatPop->organisms[i]->fitness;
         maxPopOrg = i;

      }

   }

   bool overall_winner_changed = false;

   //Compare generational winner with overall winner
   //And then update overall winner
   if (m_unCurrentGeneration == 1 || maxPopScore > overall_winner->fitness) {

      overall_winner_changed = true;
      overall_winner_change_since_last_eval = true;

      //Duplicate winner because it will be deleted by next generation
      //We need this to flush genome.
      NEAT::Genome* new_genome;
      NEAT::Organism* new_organism;

      new_genome = neatPop->organisms[maxPopOrg]->gnome->duplicate(1);

      overall_winner = new NEAT::Organism(maxPopScore, new_genome, 1);
      overall_winner->winning_gen = m_unCurrentGeneration;

   }

   if(!HANDWRITTEN_ENVS && !RANDOMLY_GENERATED_ENVS) test_on_training_set(trial_results[maxPopOrg], overall_winner_changed);
   if(!HANDWRITTEN_ENVS) test_on_eval_set();

}

//Record the number of finishes on the TRAINING SET for the overall winner
void GA::test_on_training_set(std::vector<RunResult> overall_winner_results, bool changed) {

   if(changed) {

      int finishes = 0;

      for(int i = 0; i < overall_winner_results.size(); i++) {
         //std::cout << overall_winner_scores[i] << std::endl;
         if(overall_winner_results[i].got_to_tower) finishes++;

      }
      //std::cout << "Finishes: " << finishes << std::endl;
      overall_winner_num_finishes = finishes;
      m_overall_winner_results = overall_winner_results;

   }

   std::ofstream outfile;

   //Create directory if it already does not exist
   if (!boost::filesystem::exists("../scores/"))
      boost::filesystem::create_directories("../scores");

   outfile.open("../scores/training_scores.txt", std::ios_base::app);
   outfile << m_unCurrentGeneration << "," << overall_winner_num_finishes;

   for(int i = 0; i < m_overall_winner_results.size(); i++) {

      outfile << "," << m_overall_winner_results[i].fitness;

   }

   outfile << "\n";
   outfile.close();

   std::cout << "Training set scores printed" << std::endl;

}

void GA::test_on_eval_set() {

   //Every TEST_EVAL_GEN test the best genome on the evaluation set and
   //write to file

   if(m_unCurrentGeneration % TEST_EVAL_GEN == 0) {

      //Collect genomes to be tested
      //Here I can include overall winner if it has changed

      if (overall_winner_change_since_last_eval) {

         std::cout << "Evaluating on test set.." << std::endl;

         std::vector<RunResult> run_results(NUM_TEST_ENVS);

         for(int i = 0; i < NUM_TEST_ENVS; i++) {

            std::string file_name = TEST_SET_PATH + std::to_string(i+1) + ".png";

            //overall_winner->gnome->print_to_filename(outfile.c_str());

            //Create random seed for parallel processes - it is not really needed
            //in the serial version but the method now needs one
            int rand_seed = rand();

            run_results[i] = as->run(*overall_winner, file_name, (i+1), true, true,
                                      HANDWRITTEN_ENVS, (i+1), rand_seed);

            //std::cout << "Eval set: " << scores[i] << std::endl;

         }

         int num_finishes = 0;

         for(int i = 0; i < run_results.size(); i++) {

            if(run_results[i].got_to_tower) num_finishes++;

         }

         eval_set_finishes = num_finishes;
         eval_set_results = run_results;

         overall_winner_change_since_last_eval = false;

      }

      std::ofstream outfile;

      //Create directory if it already does not exist
      if (!boost::filesystem::exists("../scores/"))
         boost::filesystem::create_directories("../scores");

      outfile.open("../scores/eval_scores.txt", std::ios_base::app);
      outfile << m_unCurrentGeneration << "," << eval_set_finishes;

      for(int i = 0; i < eval_set_results.size(); i++) {

         outfile << "," << eval_set_results[i].fitness;

      }

      outfile << "\n";
      outfile.close();

      std::cout << "Test set scores printed" << std::endl;

   }

}

void GA::flush_winners() {

   //Flush overall winner every 1/3 of the way through a run with different name
   //if(std::find(flush_gens.begin(), flush_gens.end(), m_unCurrentGeneration) != flush_gens.end()) {

   //Flush every FLUSH_EVERY gens - could to make this equal to TEST_EVAL_GEN so
   //the winner that is tested on the eval set is flushed
   if (m_unCurrentGeneration % FLUSH_EVERY == 0) {

      //Create directory if it already does not exist
      if (!boost::filesystem::exists("../winners/"))
         boost::filesystem::create_directories("../winners");

      std::stringstream ss1, ss2;

      ss1 << "../winners/overall_winner_at_" << m_unCurrentGeneration;
      ss2 << "../winners/overall_winner_org_at_" << m_unCurrentGeneration;
      std::string outfile = ss1.str();
      std::string outfileOrg = ss2.str();

      std::cout << "[Flushing winner from flush gens... ";

      overall_winner->gnome->print_to_filename(outfile.c_str());
      overall_winner->print_to_file(outfileOrg.c_str());

      std::cout << " done.]" <<std::endl;

   }

   //Print out overall winner every generation
   std::string outfile = "../winners/overall_winner";
   std::string outfileOrg = "../winners/overall_winner_org";

   std::cout << "[Flushing winner... ";

   overall_winner->gnome->print_to_filename(outfile.c_str());
   overall_winner->print_to_file(outfileOrg.c_str());

   std::cout << " done.]" <<std::endl;

   std::cout << "Winning organism generation: " << (overall_winner->winning_gen) << std::endl;

}

void GA::nextGen() {

   neatPop->epoch(m_unCurrentGeneration);
   ++m_unCurrentGeneration;

}

// Checks that it has got above the number of generations but also that the
// fitness is high enough too
bool GA::done() const {
   return (m_unCurrentGeneration > NEAT::num_gens) && (overall_winner->fitness > ACCEPTABLE_FITNESS);
}


/* Function implementations for SharedMem class */

GA::SharedMem::SharedMem(int population_size, int num_trials) :
   SHARED_MEMORY_FILE_FITNESS("/SHARED_MEMORY_FITNESS"),
   SHARED_MEMORY_FILE_RESULT("/SHARED_MEMORY_RESULT"),
   m_popSize(population_size),
   m_numTrials(num_trials) {

   //Create shared mem file descriptor
   m_sharedMemFD_fitness = ::shm_open(SHARED_MEMORY_FILE_FITNESS.c_str(),
                               O_RDWR | O_CREAT,
                               S_IRUSR | S_IWUSR);

    //Create shared mem file descriptor
    m_sharedMemFD_result = ::shm_open(SHARED_MEMORY_FILE_RESULT.c_str(),
                                O_RDWR | O_CREAT,
                                S_IRUSR | S_IWUSR);

   //Check it has been initialised correctly
   if(m_sharedMemFD_fitness < 0) {
      ::perror(SHARED_MEMORY_FILE_FITNESS.c_str());
      exit(1);
   }

   if(m_sharedMemFD_result < 0) {
      ::perror(SHARED_MEMORY_FILE_RESULT.c_str());
      exit(1);
   }

   //Resize
   size_t mem_size_fitness = m_popSize * m_numTrials * sizeof(double);
   ::ftruncate(m_sharedMemFD_fitness, mem_size_fitness);

   size_t mem_size_result = m_popSize * m_numTrials * sizeof(bool);
   ::ftruncate(m_sharedMemFD_result, mem_size_result);

   //Get pointer
   m_sharedMem_fitness = reinterpret_cast<double*>(
      ::mmap(NULL,
             mem_size_fitness,
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             m_sharedMemFD_fitness,
             0));

    m_sharedMem_result = reinterpret_cast<bool*>(
       ::mmap(NULL,
              mem_size_result,
              PROT_READ | PROT_WRITE,
              MAP_SHARED,
              m_sharedMemFD_result,
              0));

   //Check for failure
   if(m_sharedMem_fitness == MAP_FAILED) {
      ::perror("shared memory fitness");
      exit(1);
   }

   if(m_sharedMem_result == MAP_FAILED) {
      ::perror("shared memory result");
      exit(1);
   }

}

GA::SharedMem::~SharedMem() {

   munmap(m_sharedMem_fitness, m_popSize * sizeof(double));
   close(m_sharedMemFD_fitness);
   shm_unlink(SHARED_MEMORY_FILE_FITNESS.c_str());

   munmap(m_sharedMem_result, m_popSize * sizeof(bool));
   close(m_sharedMemFD_result);
   shm_unlink(SHARED_MEMORY_FILE_RESULT.c_str());

}

std::vector<RunResult> GA::SharedMem::get_run_result(int individual) {

   std::vector<RunResult> run_results;

   for(size_t i = 0; i < m_numTrials; i++) {

      RunResult rr;
      rr.fitness = m_sharedMem_fitness[individual * m_numTrials + i];
      rr.got_to_tower = m_sharedMem_result[individual * m_numTrials + i];

      run_results.push_back(rr);

   }

   return run_results;

}

void GA::SharedMem::set_run_result(int individual, int trial_num, RunResult run_result) {

   m_sharedMem_fitness[individual * m_numTrials + trial_num] = run_result.fitness;
   m_sharedMem_result[individual * m_numTrials + trial_num] = run_result.got_to_tower;

}
