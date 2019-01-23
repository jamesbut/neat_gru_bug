#ifndef _NOVELTY_SEARCH_H_
#define _NOVELTY_SEARCH_H_

#include <vector>
#include "organism.h"

//typedef for 3d trajectory array
typedef std::vector<std::vector<std::vector<double> > > vec3d;
typedef std::vector<std::vector<double> > vec2d;

//struct NoveltyArchive {};
struct NoveltyItem {

   //We can keep track of genotype & phenotype of novel item
   //TODO: Surely the underlying data structure gets deleted
   //after generation ends so I might have to store a copy
   //of these here
   NEAT::Genome* genotype;
   NEAT::Network* phenotype;

   //Used for analysis purposes
   double novelty;
   double fitness;
   int generation;

};

class NoveltySearch {

public:

   NoveltySearch(const double NOVELTY_THRESHOLD, const int K, const int NUM_ENVS,
                 const int POP_SIZE, const int TRAJ_SIZE);

   vec2d calculate_behaviour_characteristics();
   void add_to_archive(NEAT::Organism &org);

   double calculate_novelty();

private:

   //I think this is the novelty threshold required
   //to add a behaviour to the archive
   const double NOVELTY_THRESHOLD;

   //This is the k used in the k-nearest neighbours
   //comparison
   const int K;

   //So the class knows how many files to read and use
   const int NUM_ENVS;
   const int POP_SIZE;
   const int TRAJ_SIZE;

   std::vector<NoveltyItem> novelty_archive;

   std::vector<double> calculate_behaviour_characteristic(vec3d& trajectories);
   vec3d read_trajectories(int org_num);

};

#endif
