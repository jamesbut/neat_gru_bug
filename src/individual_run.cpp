#include "individual_run.h"

IndividualRun::IndividualRun(const std::string& gf, const bool handwritten) :
   ARGOS_FILE_NAME("../argos_params/no_walls.argos"),
   ARGOS_FILE_NAME_10("../argos_params/no_walls_10_vis.argos"),
   eg(),
   NUM_RUNS(handwritten ? 5 : 209),
   RANDOM_ENVS(false),
   HANDWRITTEN_ENVS(handwritten),
   ENV_PATH("../argos_params/environments/kim_envs/rand_env_")
   //ENV_PATH("../argos_params/environments/rand_envs_14_2/rand_env_")
   //ENV_PATH("../argos_params/environments/handwritten_envs/e")
{

   if(HANDWRITTEN_ENVS) {
      as = new ARGoS_simulation(ARGOS_FILE_NAME_10);
      eg.set_argos_config_file(ARGOS_FILE_NAME_10);
   } else {
      as = new ARGoS_simulation(ARGOS_FILE_NAME);
      eg.set_argos_config_file(ARGOS_FILE_NAME);
   }

   readGenomeFile(gf);

}

IndividualRun::~IndividualRun() {

   delete as;

}

void IndividualRun::run() {

   int num_finishes = 0;
   double total_score = 0;

   std::vector<int> struggling_envs;
   std::vector<double> scores(NUM_RUNS);

   for(int i = 0; i < NUM_RUNS; i++) {

      std::cout << "Run: " << (i+1) << std::endl;

      //Create file name
      std::string file_name;
      int env_num;
      bool reset = false;
      bool test_envs = false;

      if(RANDOM_ENVS) file_name = "";
      else if (HANDWRITTEN_ENVS) {
         file_name = ENV_PATH + "15.png";
         env_num = 15;
      }
      else {
         test_envs = true;
         file_name = ENV_PATH + std::to_string(i+1) + ".png";
         env_num = i+1;
         reset = true;
      }

      //Generate environment
      eg.generate_env(file_name, env_num);

      RunResult rr = as->run(*org, env_num, reset, true,
                              HANDWRITTEN_ENVS, test_envs, (i+1), eg);

      total_score += rr.fitness;
      scores[i] = rr.fitness;

      std::cout << rr.fitness << std::endl;

      if(rr.got_to_tower) num_finishes++;
      else struggling_envs.push_back(i);

   }

   std::cout << "Finishes: " << num_finishes << std::endl;
   std::cout << "Runs: " << NUM_RUNS << std::endl;
   double average_score = (total_score / NUM_RUNS);
   std::cout << "Average score: " << average_score  << std::endl;

   //Calculate variance and S.D.
   double sum_of_squares = 0.0;

   for(int i = 0; i < scores.size(); i++)
      sum_of_squares += pow((scores[i] - average_score), 2);

   double variance = sum_of_squares / (double)(scores.size() - 1);
   std::cout << "Variance: " << variance << std::endl;

   double sd = sqrt(variance);
   std::cout << "S.D.: " << sd << std::endl;

   //Show environments that the genome struggles with
   std::cout << "Struggling envs: ";
   for(int i = 0; i < struggling_envs.size(); i++)
      std::cout << struggling_envs[i]+1 << ", ";
   std::cout << std::endl;

}

void IndividualRun::readGenomeFile(const std::string& gf) {

   char curword[20];
   int id;

   std::string file_prefix = "../winners/";
   std::string genomeFilePath = file_prefix + gf;

   std::ifstream iFile(genomeFilePath.c_str());

   std::cout << "Reading in the individual" << std::endl;

   iFile >> curword;
   iFile >> id;

   std::cout << curword << std::endl;
   std::cout << id << std::endl;

   NEAT::Genome* genom = new NEAT::Genome(id,iFile);
   iFile.close();

   std::cout << "Complexity: " << genom->get_complexity() << std::endl;

   org = new NEAT::Organism(0.0, genom, 1);

   delete genom;

}
