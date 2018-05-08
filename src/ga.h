#ifndef GA_H
#define GA_H

#include <NEAT_GRU/include/population.h>

/*
 * A simple GA using NEAT.
 *
 */

class GA {

public:

   GA(std::string neat_param_file);

   ~GA();

   void run();

private:

   virtual void initNEAT(std::string neat_param_file);

   double getTrialScore(int trial, int org);

   bool loadNEATParams(std::string neat_file);

   /**
    * Returns the current population.
    */
   const NEAT::Population* getPopulation() const;

   /**
    * Returns the current generation.
    */
   int getGeneration() const;

   /**
    * Tidies up memory and close files.
    * Used internally, don't call it from user code.
    */
   virtual void cleanup();

   /**
    * Runs the trials to evaluate the current population.
    */
   virtual void epoch();

   /**
    * Executes the next generation.
    */
   virtual void nextGen();

   /**
    * Returns true if the evolution is finished, false otherwise.
    */
   virtual bool done() const;

protected:

   /** Current generation */
   int m_unCurrentGeneration;

   //Current population
   NEAT::Population* neatPop;

   //Start genome for population
   NEAT::Genome* start_genome;

   //Keep track of overall winner over all generations.
   //I do this because it is often the case that the winner of the final
   //generation is not the overall winner of all the generations.
   NEAT::Organism* winner;

   // Set up vector of scores for each trial
   std::vector<double> trialScores;

   std::vector<int> flush_gens;

   const int NUM_ROBOTS;
   const int NUM_FLUSHES;
   const int MUTATING_START;

};

#endif
