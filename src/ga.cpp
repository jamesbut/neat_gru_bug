#include "ga.h"

GA::GA(std::string neat_param_file) :
   m_unCurrentGeneration(0),
   NUM_ROBOTS(1),
   NUM_FLUSHES(3),
   MUTATING_START(true)
   {

   initNEAT(neat_param_file);

   //Determine when to flush overall winner to file
   flush_gens.resize(NUM_FLUSHES);
   std::cout << NEAT::num_gens << std::endl;
   std::cout << "Flush Gens: " << std::endl;

   for(int i = 0; i < NUM_FLUSHES; i++) {

      flush_gens[i] = (double)(i+1)/(double)NUM_FLUSHES * NEAT::num_gens;
      std::cout << flush_gens[i] << std::endl;

   }

}

GA::~GA() {}

void GA::initNEAT(std::string neat_param_file) {

   std::cout << "Initialising NEAT.." << std::endl;

   if(!loadNEATParams(neat_param_file)) exit (EXIT_FAILURE);

   char curword[20];
   int id;

   std::ifstream iFile ("../starting_genomes/start_genome");

   iFile >> curword;
   iFile >> id;

   std::cout << "Reading in Genome id " << id << std::endl;

   //Initialise starting genome
   start_genome = new NEAT::Genome(id,iFile);
   iFile.close();

   //Spawn the Population
   std::cout << "Spawning Population off Genome" << std::endl;

   if(MUTATING_START) neatPop = new NEAT::Population(start_genome,NEAT::pop_size);
   else neatPop = new NEAT::Population(start_genome,NEAT::pop_size,0.0);

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

const NEAT::Population* GA::getPopulation() const {
   return neatPop;
}

int GA::getGeneration() const {
   return m_unCurrentGeneration;
}

void GA::run() {

   //Start main loop....

   //While the current generations is less than the maximum generation
   while(!done()) {

      //Evaluate current generation
      epoch();

      //Print organism scores
      std::cout << "Generation #" << getGeneration() << "...";
      std::cout << " scores:";

      for(int i = 0; i < getPopulation()->organisms.size(); ++i) {

         std::cout << " " << getPopulation()->organisms[i]->fitness;

      }

      std::cout << std::endl;

      nextGen();

    }

}

// Evaluate 1 population
void GA::epoch() {

   //Create shared memory block for master and slaves
   //The population size might change throughout so account for this
   shared_mem = new SharedMem(neatPop->organisms.size());

   //Scores of max organism
   int maxOrg;
   double maxScore;

   NEAT::Organism* maxOrgan;

   //Run individual fitness tests
   for(size_t i = 0; i < NEAT::num_trials; i++) {

      for(size_t j = 0; j < neatPop->organisms.size(); j++) {

         std::cout << "Organism num: " << j << std::endl;
         std::cout << "Trial num: " << i << std::endl;

         std::cout << "Env: " << i+1 << std::endl;

         //TODO: Do parallel stuff here

         as.run(*(neatPop->organisms[j]));

         //TODO: Populate trial scores as well here

      }

   }


   // for(int i = 0; i < NEAT::pop_size; i++) {
   //
   //    std::vector<double> individual_trial_scores(NEAT::num_trials);
   //
   //    for(int j = 0; j < NEAT::num_trials; j++) {
   //
   //       individual_trial_scores[j] = getTrialScore(j,i);
   //
   //    }
   //
   //    //double minTrial = *std::min_element(individual_trial_scores.begin(), individual_trial_scores.end());
   //    double meanTrial = std::accumulate(individual_trial_scores.begin(), individual_trial_scores.end(), 0.0) / individual_trial_scores.size();
   //    //double sumTrial = std::accumulate(individual_trial_scores.begin(), individual_trial_scores.end(), 0.0);
   //
   //    //neatPop->organisms[i]->fitness = minTrial;
   //    neatPop->organisms[i]->fitness = meanTrial;
   //    //neatPop->organisms[i]->fitness = sumTrial;
   //
   //    // Keep track of populations best organism
   //    if (i == 0 || neatPop->organisms[i]->fitness > maxScore) {
   //
   //       maxScore = neatPop->organisms[i]->fitness;
   //       //std::cout << "Organism winner number: " << i << std::endl;
   //       maxOrg = i;
   //       //maxOrgan = neatPop->organisms[i];
   //
   //    }
   //
   // }
   //
   // //Compare generational winner with overall winner
   // //And then update overall winner
   // if (m_unCurrentGeneration == 0 || maxScore > winner->fitness) {
   //
   //    //std::cout << "Updating winner!" << std::endl;
   //    //winner = neatPop->organisms[maxOrg];
   //    //winner = maxOrgan;
   //    NEAT::Genome* new_genome;
   //    NEAT::Organism* new_organism;
   //    //std::cout << "Max org: " << maxOrg << std::endl;
   //    new_genome = neatPop->organisms[maxOrg]->gnome->duplicate(1);
   //
   //    winner = new NEAT::Organism(maxScore,new_genome,1);
   //    winner->winning_gen = (m_unCurrentGeneration+1);
   //    //winner = new_organism;
   //
   // }

   //Flush final generation winner
   // if(m_unCurrentGeneration == NEAT::num_gens) {
   //
   //    std::stringstream ss;
   //    ss << "ibug_working_directory/gen_" << m_unCurrentGeneration << "_winner";
   //    std::string outFile = ss.str();
   //
   //    std::cout << " [Flushing final gen genome... ";
   //
   //    // Flush scores of best individual
   //    neatPop->organisms[maxOrg]->gnome->print_to_filename(outFile.c_str());
   //
   //    std::cout << "done.]";
   //
   // }
   //std::cout << "Vector size: " << flush_gens.size() << std::endl;

   //Flush overall winner every 1/3 of the way through a run with different name
   // if(std::find(flush_gens.begin(), flush_gens.end(), m_unCurrentGeneration) != flush_gens.end()) {
   //
   //    std::stringstream ss1;
   //    std::stringstream ss2;
   //
   //    //TODO: Change these directories
   //    ss1 << "ibug_working_directory/overall_winner_at_" << m_unCurrentGeneration;
   //    ss2 << "ibug_working_directory/overall_winner_org_at_" << m_unCurrentGeneration;
   //    std::string outfile = ss1.str();
   //    std::string outfileOrg = ss2.str();
   //
   //    std::cout << " [Flushing winner from flush gens... ";
   //
   //    winner->gnome->print_to_filename(outfile.c_str());
   //    winner->print_to_file(outfileOrg.c_str());
   //
   //    std::cout << " done.]" <<std::endl;
   //
   // }
   //
   // //Print out overall winner every generation
   // std::string outfile = "ibug_working_directory/overall_winner";
   // std::string outfileOrg = "ibug_working_directory/overall_winner_org";
   //
   // std::cout << " [Flushing winner... ";
   //
   // winner->gnome->print_to_filename(outfile.c_str());
   // winner->print_to_file(outfileOrg.c_str());
   //
   // std::cout << " done.]" <<std::endl;
   //
   // std::cout << "Winning organism generation: " << (winner->winning_gen) << std::endl;

   //Delete shared memory
   delete shared_mem;

}

double GA::getTrialScore(int trial, int org) {

   return trialScores[NEAT::pop_size*trial + org];

}

void GA::nextGen() {

   ++m_unCurrentGeneration;
   trialScores.resize(0);
   neatPop->epoch(m_unCurrentGeneration);

}

bool GA::done() const {
   return m_unCurrentGeneration >= NEAT::num_gens;
}



/* Function implementations for SharedMem class */

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
//#include <sys/stat.h>
//#include <sys/types.h>

GA::SharedMem::SharedMem(int population_size) : SHARED_MEMORY_FILE("/SHARED_MEMORY") {

   m_popSize = population_size;

   //Create shared mem file descriptor
   m_sharedMemFD = ::shm_open(SHARED_MEMORY_FILE.c_str(),
                               O_RDWR | O_CREAT,
                               S_IRUSR | S_IWUSR);

   //Check it has been initialised correctly
   if(m_sharedMemFD < 0) {
      ::perror(SHARED_MEMORY_FILE.c_str());
      exit(1);
   }

   //Resize
   size_t mem_size = m_popSize * sizeof(double);
   ::ftruncate(m_sharedMemFD, mem_size);

   //Get pointer
   m_sharedMem = reinterpret_cast<double*>(
      ::mmap(NULL,
             mem_size,
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             m_sharedMemFD,
             0));

   //Check for failure
   if(m_sharedMem == MAP_FAILED) {
      ::perror("shared memory");
      exit(1);
   }

}

GA::SharedMem::~SharedMem() {

   munmap(m_sharedMem, m_popSize * sizeof(double));
   close(m_sharedMemFD);
   shm_unlink(SHARED_MEMORY_FILE.c_str());

}

double GA::SharedMem::get_fitness(int individual) {

   return m_sharedMem[individual];

}
void GA::SharedMem::set_fitness(int individual, double fitness) {

   m_sharedMem[individual] = fitness;

}
