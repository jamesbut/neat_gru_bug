#include "individual_run.h"

IndividualRun::IndividualRun(const std::string& gf) :
   NUM_RUNS(209),
   as("../argos_params/no_walls.argos"),
   ENV_PATH("../argos_params/environments/kim_envs/rand_env_")
   //ENV_PATH("../argos_params/environments/rand_envs_14_2/rand_env_")
   {

   readGenomeFile(gf);

}

IndividualRun::~IndividualRun() {}

void IndividualRun::run() {

   int num_finishes = 0;

   for(int i = 0; i < NUM_RUNS; i++) {

      std::cout << "Run: " << (i+1) << std::endl;

      //Create file name
      std::string file_name = ENV_PATH + std::to_string(i+1) + ".png";

      double fitness = as.run(*org, file_name, true, true);

      if (fitness > 13.2) num_finishes++;

   }

   //TODO: How to get number of finishes here??
   std::cout << "Finishes: " << num_finishes << std::endl;
   std::cout << "Runs: " << NUM_RUNS << std::endl;

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

   org = new NEAT::Organism(0.0, genom, 1);

   delete genom;

}
