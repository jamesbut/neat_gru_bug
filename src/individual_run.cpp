#include "individual_run.h"

IndividualRun::IndividualRun(const std::string& gf) :
   //NUM_RUNS(5),
   NUM_RUNS(209),
   RANDOM_ENVS(false),
   HANDWRITTEN_ENVS(false),
   ENV_PATH("../argos_params/environments/kim_envs/rand_env_")
   //ENV_PATH("../argos_params/environments/rand_envs_14_2/rand_env_")
   //ENV_PATH("../argos_params/environments/handwritten_envs/e")
{

   if(HANDWRITTEN_ENVS)
      as = new ARGoS_simulation("../argos_params/no_walls_10_vis.argos");
   else
      as = new ARGoS_simulation("../argos_params/no_walls.argos");

   readGenomeFile(gf);

}

IndividualRun::~IndividualRun() {

   delete as;

}

void IndividualRun::run() {

   int num_finishes = 0;
   double total_score = 0;

   std::vector<int> struggling_envs;

   for(int i = 0; i < NUM_RUNS; i++) {

      std::cout << "Run: " << (i+1) << std::endl;

      //Create file name
      std::string file_name;
      int env_num;
      bool reset = false;

      if(RANDOM_ENVS) file_name = "";
      else if (HANDWRITTEN_ENVS) {
         file_name = ENV_PATH + "15.png";
         env_num = 15;
      }
      else {
         file_name = ENV_PATH + std::to_string(i+1) + ".png";
         env_num = i+1;
         reset = true;
      }

      //Create random seed for parallel processes - it is not really needed
      //in the serial version but the method now needs one
      int rand_seed = rand();

      //double fitness = as->run(*org, file_name, env_num, reset, true, HANDWRITTEN_ENVS, (i+1));
      RunResult rr = as->run(*org, file_name, env_num, reset, true,
                              HANDWRITTEN_ENVS, (i+1), rand_seed);

      //total_score += fitness;
      total_score += rr.fitness;

      //std::cout << fitness << std::endl;
      std::cout << rr.fitness << std::endl;

      //if (fitness > 13.2) num_finishes++;
      if(rr.got_to_tower) num_finishes++;
      else struggling_envs.push_back(i);

   }

   std::cout << "Finishes: " << num_finishes << std::endl;
   std::cout << "Runs: " << NUM_RUNS << std::endl;
   std::cout << "Average score: " << (total_score / NUM_RUNS) << std::endl;

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
