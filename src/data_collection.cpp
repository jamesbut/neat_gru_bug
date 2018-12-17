#include "data_collection.h"
#include <boost/filesystem.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <thread>

DataCollection::DataCollection(const bool RANDOMLY_GENERATED_ENVS,
                               const bool NO_BEARING,
                               const int TEST_EVAL_GEN,
                               const int NUM_TEST_ENVS,
                               const int FLUSH_EVERY,
                               const std::string TEST_SET_PATH,
                               ARGoS_simulation* argos_simulation) :
   RANDOMLY_GENERATED_ENVS(RANDOMLY_GENERATED_ENVS),
   NO_BEARING(NO_BEARING),
   TEST_EVAL_GEN(TEST_EVAL_GEN),
   NUM_TEST_ENVS(NUM_TEST_ENVS),
   FLUSH_EVERY(FLUSH_EVERY),
   TEST_SET_PATH(TEST_SET_PATH),
   NASH_AVERAGING(false),
   as(argos_simulation),
   eg() {

      //Create shared memory block for master and slaves
      const int MAX_NUM_GENOMES_TO_TEST = 7;
      shared_mem = new SharedMem(MAX_NUM_GENOMES_TO_TEST, NUM_TEST_ENVS, "DC");

   }

void DataCollection::collect_scores(const std::vector<std::vector <RunResult> >& trial_results,
                                    NEAT::Population* neatPop,
                                    int current_gen) {

   //Get fitnesses out of trial results for nash averaging
   std::vector<std::vector<double> > trial_fitnesses(trial_results.size());

   for(size_t i = 0; i < trial_results.size(); i++) {

      trial_fitnesses[i].resize(trial_results[i].size());

      for(size_t j = 0; j < trial_results[i].size(); j++)
         trial_fitnesses[i][j] = trial_results[i][j].fitness;

   }

   std::vector<double> agent_skills;

   if(NASH_AVERAGING) {

      //Calculate agent skills via nash averaging
      std::cout << "Solving nash game.." << std::endl;
      agent_skills = nash_averager.calculate_agent_skills(trial_fitnesses,
                                                          trial_results.size(),
                                                          trial_results[0].size());
      std::cout << "..game solved!" << std::endl;

   }

   int maxPopOrg;
   double maxPopScore;

   if(NASH_AVERAGING) std::cout << "Mean scores:" << std::endl;
   //Set fitnesses in NEAT
   for(size_t i = 0; i < neatPop->organisms.size(); i++) {

      std::vector<RunResult> individual_trial_run_results = trial_results[i];
      std::vector<double> individual_trial_fitnesses;

      for(int i = 0; i < individual_trial_run_results.size(); i++)
         individual_trial_fitnesses.push_back(individual_trial_run_results[i].fitness);

      //double minTrial = *std::min_element(individual_trial_scores.begin(), individual_trial_scores.end());
      double meanTrialFitness = std::accumulate(individual_trial_fitnesses.begin(), individual_trial_fitnesses.end(), 0.0) / individual_trial_fitnesses.size();
      //double sumTrial = std::accumulate(individual_trial_scores.begin(), individual_trial_scores.end(), 0.0);

      if(NASH_AVERAGING) std::cout << meanTrialFitness << " ";

      //Set skills to either uniform average OR nash average
      if(NASH_AVERAGING) neatPop->organisms[i]->fitness = agent_skills[i];
      else neatPop->organisms[i]->fitness = meanTrialFitness;

      //Find best organism in population
      if (i == 0 || neatPop->organisms[i]->fitness > maxPopScore) {

         maxPopScore = neatPop->organisms[i]->fitness;
         maxPopOrg = i;

      }

   }

   if(NASH_AVERAGING) std::cout << std::endl;

   bool overall_winner_changed = false;

   //Compare generational winner with overall winner
   //And then update overall winner
   if (current_gen == 1 || maxPopScore > overall_winner->fitness) {

      overall_winner_changed = true;
      overall_winner_change_since_last_eval = true;

      //Duplicate winner because it will be deleted by next generation
      //We need this to flush genome.
      NEAT::Genome* new_genome = neatPop->organisms[maxPopOrg]->gnome->duplicate(1);

      overall_winner.reset(new NEAT::Organism(maxPopScore, new_genome, 1));
      overall_winner->winning_gen = current_gen;

   }

   //if(RANDOMLY_GENERATED_ENVS) {

      //Collect rest of genomes for evaluation on test set
      //Scan through array 3 times choosing the largest and moving down

      //Set older genomes to previous generational winners
      gen_nminus2_1 = std::move(gen_nminus1_1);
      gen_nminus1_1 = std::move(gen_n_1);
      gen_nminus1_2 = std::move(gen_n_2);


      std::vector<int> previously_found_winners;

      //Iterate through 3 times to get the 3 top performers
      for(int i = 0; i < 3; i++) {

         int top_genome = -1;
         double top_genome_fitness = -1.0;      //Fitness shoud never be a negative

         for(int j = 0; j < neatPop->organisms.size(); j++) {

            //Ignore previously found winners (e.g. 1st and 2nd place)
            if(std::find(previously_found_winners.begin(), previously_found_winners.end(), j) == previously_found_winners.end()) {

               if (neatPop->organisms[j]->fitness > top_genome_fitness) {

                  top_genome = j;
                  top_genome_fitness = neatPop->organisms[j]->fitness;

               }

            }

         }

         //std::cout << "Top genome: " << top_genome << std::endl;
         //std::cout << "Top genome fitness: " << top_genome_fitness << std::endl;

         previously_found_winners.push_back(top_genome);

         // std::cout << "Previously found winners: ";
         // for(int k = 0; k < previously_found_winners.size(); k++)
         //    std::cout << previously_found_winners[k] << " ";
         // std::cout << std::endl;

         //Set appropriate genomes
         NEAT::Genome* new_genome = neatPop->organisms[top_genome]->gnome->duplicate(1);

         switch(i) {

            case 0:
               gen_n_1.reset(new NEAT::Organism(top_genome_fitness, new_genome, 1));
               break;

            case 1:
               gen_n_2.reset(new NEAT::Organism(top_genome_fitness, new_genome, 1));
               break;

            case 2:
               gen_n_3.reset(new NEAT::Organism(top_genome_fitness, new_genome, 1));
               break;

         }

      }

      // std::cout << "gen_n_1 fitness: " << gen_n_1->fitness << std::endl;
      // std::cout << "gen_n_2 fitness: " << gen_n_2->fitness << std::endl;
      // std::cout << "gen_n_3 fitness: " << gen_n_3->fitness << std::endl;

   //}

   //if(!HANDWRITTEN_ENVS) test_on_training_set(trial_results, current_gen);
   test_on_training_set(trial_results, current_gen);
   //if(!HANDWRITTEN_ENVS) test_on_eval_set(current_gen);
   if(!NO_BEARING) test_on_eval_set(current_gen);

}

//Record the number of finishes on the TRAINING SET for the overall winner
void DataCollection::test_on_training_set(const std::vector<std::vector<RunResult> >& gen_results,
                                          int current_gen) {

   //Extract scores and num finishes
   std::vector<double> mean_training_scores;
   std::vector<bool> training_num_finishes;

   double max_training_score = 0.0;

   //For each individual
   for(int i = 0; i < gen_results.size(); i++) {

      //Calculate mean score and add to vector
      double total_score = 0.0;

      for(int j = 0; j < gen_results[i].size(); j++) {

         total_score += gen_results[i][j].fitness;

      }

      double mean_score = total_score / gen_results[i].size();
      mean_training_scores.push_back(mean_score);

      //Track largest score
      if(mean_score > max_training_score)
         max_training_score = mean_score;

   }

   //Calculate gen mean of scores
   double mean_gen_fitness = std::accumulate(mean_training_scores.begin(), mean_training_scores.end(), 0.0) / mean_training_scores.size();

   //Find max number of finishes and mean number of finishes
   std::vector<int> gen_num_finishes;
   int max_num_finishes = 0;
   int total_num_finishes = 0;

   for(int i = 0; i < gen_results.size(); i++) {

      int num_finishes = 0;

      for(int j = 0; j < gen_results[i].size(); j++)
         if(gen_results[i][j].got_to_tower) num_finishes++;

      gen_num_finishes.push_back(num_finishes);

      if(num_finishes > max_num_finishes)
         max_num_finishes = num_finishes;

      total_num_finishes += num_finishes;

   }

   //Calculate mean number of finishes
   double mean_num_finishes = (double)total_num_finishes / (double)gen_results.size();

   //Find min trajectories and mean trajectories
   std::vector<double> mean_gen_trajectories;
   double min_mean_trajectories = 0.0;
   double total_trajectories = 0.0;

   for(int i = 0; i < gen_results.size(); i++) {

      double total_traj = 0.0;

      for(int j = 0; j < gen_results[i].size(); j++) {
         total_traj += gen_results[i][j].traj_per_astar;
         //std::cout << gen_results[i][j].traj_per_astar << std::endl;
      }

      double mean_traj = total_traj / gen_results[i].size();

      mean_gen_trajectories.push_back(mean_traj);

      if((mean_traj < min_mean_trajectories) || i == 0)
         min_mean_trajectories = mean_traj;

      total_trajectories += mean_traj;

   }

   //Calculate mean trajectories
   double mean_trajectories = total_trajectories / (double)gen_results.size();

   //Print results
   std::ofstream outfile;

   //Create directory if it already does not exist
   if (!boost::filesystem::exists("../scores/"))
      boost::filesystem::create_directories("../scores");

   outfile.open("../scores/training_scores/training_scores.txt", std::ios_base::app);
   outfile << current_gen << ",";
   outfile << max_training_score << ",";
   outfile << mean_gen_fitness << ",";
   outfile << max_num_finishes << ",";
   outfile << mean_num_finishes << ",";
   outfile << min_mean_trajectories << ",";
   outfile << mean_trajectories << "\n";

   outfile.close();

   std::cout << "Training scores printed" << std::endl;

}

void DataCollection::test_on_eval_set(int current_gen) {

   //Create scores directory if it already does not exist
   if (!boost::filesystem::exists("../scores/"))
      boost::filesystem::create_directories("../scores");

   //Every TEST_EVAL_GEN generation, a subset of genomes are tested on the test set
   if(current_gen % TEST_EVAL_GEN == 0) {

      //Collect genomes to be tested
      //Here I can include overall winner if it has changed
      std::vector<NEAT::Organism*> genomes_to_be_tested;

      //if(RANDOMLY_GENERATED_ENVS) {

         genomes_to_be_tested.push_back(gen_n_1.get());
         genomes_to_be_tested.push_back(gen_n_2.get());
         genomes_to_be_tested.push_back(gen_n_3.get());
         genomes_to_be_tested.push_back(gen_nminus1_1.get());
         genomes_to_be_tested.push_back(gen_nminus1_2.get());
         genomes_to_be_tested.push_back(gen_nminus2_1.get());

      //}

      //Test overall_winner if it has changed
      if (overall_winner_change_since_last_eval) {

         /*Add it to be tested*/
         genomes_to_be_tested.push_back(overall_winner.get());
         overall_winner_change_since_last_eval = false;

      }

      parallel_eval(genomes_to_be_tested);

      //Collect results from shared memory
      std::vector<std::vector <RunResult> > trial_results;

      for(size_t i = 0; i < genomes_to_be_tested.size(); i++) {
         trial_results.push_back(shared_mem->get_run_result(i));

         // for(size_t j = 0; j < trial_results[i].size(); j++) {
         //
         //    std::cout << "Org: " << i << " trial: " << j << " fitness: " << trial_results[i][j].fitness << std::endl;
         //
         // }
      }



      //Collect data for each run
      for(int i = 0; i < trial_results.size(); i++) {

         int num_finishes = 0;
         double total_traj_per_astar = 0.0;

         for(size_t j = 0; j < trial_results[i].size(); j++) {

            //See how many times robot made it to tower
            if(trial_results[i][j].got_to_tower) num_finishes++;

            //Record traj_per_astar
            total_traj_per_astar += trial_results[i][j].traj_per_astar;

         }

         double mean_traj_per_astar = total_traj_per_astar / trial_results[i].size();

         //Print eval results
         std::ofstream outfile;

         std::stringstream file_name;
         file_name << "../scores/eval_scores/eval_scores_" << i << ".txt";

         outfile.open(file_name.str(), std::ios_base::app);
         outfile << current_gen << "," << num_finishes << "," << mean_traj_per_astar;

         //Gather fitnesses
         std::vector<double> indv_fitnesses(trial_results[i].size());

         for(int j = 0; j < indv_fitnesses.size(); j++)
            indv_fitnesses[j] = trial_results[i][j].fitness;

         //Find average trial score
         double mean_indv_fitness = std::accumulate(indv_fitnesses.begin(), indv_fitnesses.end(), 0.0) / indv_fitnesses.size();

         outfile << "," << mean_indv_fitness;

         // for(int j = 0; j < trial_results[i].size(); j++) {
         //
         //    outfile << "," << trial_results[i][j].fitness;
         //
         // }

         outfile << "\n";
         outfile.close();

      }

   }

}

void DataCollection::parallel_eval(const std::vector<NEAT::Organism*> genomes_to_be_tested) {

   std::cout << "Evaluating on test set.." << std::endl;

   for(int i = 0; i < NUM_TEST_ENVS; i++) {

      if((i+1) % 25 == 0)
         std::cout << "Evaluating genomes on test env: " << i+1 << std::endl;

      std::string file_name = TEST_SET_PATH + std::to_string(i+1) + ".png";

      eg.generate_env(file_name, i+1);

      unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
      unsigned int num_organisms_tested = 0;

      while(num_organisms_tested < genomes_to_be_tested.size()) {

         //std::cout << "Organisms tested: " << num_organisms_tested << std::endl;

         unsigned int num_organisms_left = genomes_to_be_tested.size() - num_organisms_tested;
         unsigned int num_threads_to_spawn = std::min(num_organisms_left, concurentThreadsSupported);

         //std::cout << "Num threads to spawn: " << num_threads_to_spawn << std::endl;

         for(size_t k = 0; k < num_threads_to_spawn; k++) {

            num_organisms_tested++;

            //Spawn slaves
            slave_PIDs.push_back(::fork());

            if(slave_PIDs.back() == 0) {

               shared_mem->set_run_result(num_organisms_tested-1, i, as->run(*genomes_to_be_tested[num_organisms_tested-1],
                                          i+1, true, true, NO_BEARING, true, (i+1), eg));

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

   std::cout << "..finished evaluating on test set" << std::endl;

}

void DataCollection::flush_winners(int current_gen) {

   //Flush overall winner every 1/3 of the way through a run with different name
   //if(std::find(flush_gens.begin(), flush_gens.end(), current_gen) != flush_gens.end()) {

   //Flush every FLUSH_EVERY gens - could to make this equal to TEST_EVAL_GEN so
   //the winner that is tested on the eval set is flushed
   if (current_gen % FLUSH_EVERY == 0) {

      //Create directory if it already does not exist
      if (!boost::filesystem::exists("../winners/"))
         boost::filesystem::create_directories("../winners");

      std::stringstream ss1, ss2;

      ss1 << "../winners/overall_winner_at_" << current_gen;
      ss2 << "../winners/overall_winner_org_at_" << current_gen;
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

   //Create eval_winners directory if it already does not exist
   if (!boost::filesystem::exists("../winners/eval_winners/"))
      boost::filesystem::create_directories("../winners/eval_winners");

   //Print out eval winners every time they are evaluated
   if(current_gen % TEST_EVAL_GEN == 0) {

      std::vector<NEAT::Organism*> genomes_to_be_printed;

      genomes_to_be_printed.push_back(gen_n_1.get());
      genomes_to_be_printed.push_back(gen_n_2.get());
      genomes_to_be_printed.push_back(gen_n_3.get());
      genomes_to_be_printed.push_back(gen_nminus1_1.get());
      genomes_to_be_printed.push_back(gen_nminus1_2.get());
      genomes_to_be_printed.push_back(gen_nminus2_1.get());

      for(int i = 0; i < genomes_to_be_printed.size(); i++) {

         std::stringstream outfile, outfileOrg;

         outfile << "../winners/eval_winners/eval_winner_" << i << "_at_gen_" << current_gen;
         outfileOrg << "../winners/eval_winners/eval_winner_org_" << i << "_at_gen_" << current_gen;

         genomes_to_be_printed[i]->gnome->print_to_filename(outfile.str().c_str());
         genomes_to_be_printed[i]->print_to_file(outfileOrg.str().c_str());

      }

   }

}
