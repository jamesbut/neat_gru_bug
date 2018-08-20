#include "data_collection.h"
#include <boost/filesystem.hpp>

DataCollection::DataCollection(const bool RANDOMLY_GENERATED_ENVS,
                               const bool HANDWRITTEN_ENVS,
                               const int TEST_EVAL_GEN,
                               const int NUM_TEST_ENVS,
                               const int FLUSH_EVERY,
                               const std::string TEST_SET_PATH,
                               ARGoS_simulation* argos_simulation) :
   RANDOMLY_GENERATED_ENVS(RANDOMLY_GENERATED_ENVS),
   HANDWRITTEN_ENVS(HANDWRITTEN_ENVS),
   TEST_EVAL_GEN(TEST_EVAL_GEN),
   NUM_TEST_ENVS(NUM_TEST_ENVS),
   FLUSH_EVERY(FLUSH_EVERY),
   TEST_SET_PATH(TEST_SET_PATH),
   as(argos_simulation) {}

void DataCollection::collect_scores(std::vector<std::vector <RunResult> > trial_results,
                                    NEAT::Population* neatPop,
                                    int current_gen) {

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
   if (current_gen == 1 || maxPopScore > overall_winner->fitness) {

      overall_winner_changed = true;
      overall_winner_change_since_last_eval = true;

      //Duplicate winner because it will be deleted by next generation
      //We need this to flush genome.
      NEAT::Genome* new_genome;
      NEAT::Organism* new_organism;

      new_genome = neatPop->organisms[maxPopOrg]->gnome->duplicate(1);

      overall_winner = new NEAT::Organism(maxPopScore, new_genome, 1);
      overall_winner->winning_gen = current_gen;

   }

   if(RANDOMLY_GENERATED_ENVS) {

      //Collect rest of genomes for evaluation on test set
      //Scan through array 3 times choosing the largest and moving down

      //Set older genomes to previous generational winners
      gen_nminus2_1 = gen_nminus1_1;
      gen_nminus1_1 = gen_n_1;
      gen_nminus1_2 = gen_n_2;


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
         NEAT::Genome* new_genome;
         new_genome = neatPop->organisms[top_genome]->gnome->duplicate(1);

         switch(i) {

            case 0:
               gen_n_1 = new NEAT::Organism(top_genome_fitness, new_genome, 1);
               break;

            case 1:
               gen_n_2 = new NEAT::Organism(top_genome_fitness, new_genome, 1);
               break;

            case 2:
               gen_n_3 = new NEAT::Organism(top_genome_fitness, new_genome, 1);
               break;

         }

      }

      // std::cout << "gen_n_1 fitness: " << gen_n_1->fitness << std::endl;
      // std::cout << "gen_n_2 fitness: " << gen_n_2->fitness << std::endl;
      // std::cout << "gen_n_3 fitness: " << gen_n_3->fitness << std::endl;

   }

   if(!HANDWRITTEN_ENVS) test_on_training_set(trial_results, current_gen);
   if(!HANDWRITTEN_ENVS) test_on_eval_set(current_gen);

}

//Record the number of finishes on the TRAINING SET for the overall winner
void DataCollection::test_on_training_set(std::vector<std::vector<RunResult> > gen_results,
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
   outfile << mean_num_finishes << "\n";

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

      if(RANDOMLY_GENERATED_ENVS) {

         genomes_to_be_tested.push_back(gen_n_1);
         genomes_to_be_tested.push_back(gen_n_2);
         genomes_to_be_tested.push_back(gen_n_3);
         genomes_to_be_tested.push_back(gen_nminus1_1);
         genomes_to_be_tested.push_back(gen_nminus1_2);
         genomes_to_be_tested.push_back(gen_nminus2_1);

      }

      //Test overall_winner if it has changed
      if (overall_winner_change_since_last_eval) {

         /*Add it to be tested*/
         genomes_to_be_tested.push_back(overall_winner);
         overall_winner_change_since_last_eval = false;

      }

      //Test each genome on all the test envs
      for(int i = 0; i < genomes_to_be_tested.size(); i++) {

         std::cout << "Evaluating " << i << " on test set.." << std::endl;

         std::vector<RunResult> run_results(NUM_TEST_ENVS);

         for(int j = 0; j < NUM_TEST_ENVS; j++) {
            //std::cout << "Test env: " << j << std::endl;
            std::string file_name = TEST_SET_PATH + std::to_string(j+1) + ".png";

            //Create random seed for parallel processes - it is not really needed
            //in the serial version but the method now needs one
            int rand_seed = rand();

            //std::cout << "Handwritten envs: " << HANDWRITTEN_ENVS << std::endl;

            run_results[j] = as->run(*genomes_to_be_tested[i], file_name, (j+1), true, true,
                                      HANDWRITTEN_ENVS, (j+1), rand_seed);

            //std::cout << "Eval set: " << scores[i] << std::endl;

         }

         //See how many times robot made it to tower
         int num_finishes = 0;

         for(int j = 0; j < run_results.size(); j++) {

            if(run_results[j].got_to_tower) num_finishes++;

         }

         //Print eval results
         std::ofstream outfile;

         std::stringstream file_name;
         file_name << "../scores/eval_scores/eval_scores_" << i << ".txt";

         outfile.open(file_name.str(), std::ios_base::app);
         outfile << current_gen << "," << num_finishes;

         for(int i = 0; i < run_results.size(); i++) {

            outfile << "," << run_results[i].fitness;

         }

         outfile << "\n";
         outfile.close();

      }

   }

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

      genomes_to_be_printed.push_back(gen_n_1);
      genomes_to_be_printed.push_back(gen_n_2);
      genomes_to_be_printed.push_back(gen_n_3);
      genomes_to_be_printed.push_back(gen_nminus1_1);
      genomes_to_be_printed.push_back(gen_nminus1_2);
      genomes_to_be_printed.push_back(gen_nminus2_1);

      for(int i = 0; i < genomes_to_be_printed.size(); i++) {

         std::stringstream outfile, outfileOrg;

         outfile << "../winners/eval_winners/eval_winner_" << i << "_at_gen_" << current_gen;
         outfileOrg << "../winners/eval_winners/eval_winner_org_" << i << "_at_gen_" << current_gen;

         genomes_to_be_printed[i]->gnome->print_to_filename(outfile.str().c_str());
         genomes_to_be_printed[i]->print_to_file(outfileOrg.str().c_str());

      }

   }

}
