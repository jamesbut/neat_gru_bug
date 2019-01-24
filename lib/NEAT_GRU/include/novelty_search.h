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
   NEAT::Organism* org;

   std::vector<double> bc;

   //Used for analysis purposes
   double novelty;
   double fitness;
   int generation;

   bool tested_on_eval_set;
   bool printed;

};

class NoveltySearch {

public:

   NoveltySearch(const double NOVELTY_THRESHOLD, const int K, const int NUM_ENVS,
                 const int POP_SIZE, const int TRAJ_SIZE);

   void evaluate_population(NEAT::Population& pop);

   inline std::vector<NoveltyItem>& get_novelty_archive() {return novelty_archive;};
   inline double get_gen_novelty(int org_num) {return gen_novelties[org_num];};
   inline void set_tested(int archive_num) {novelty_archive[archive_num].tested_on_eval_set = true;};

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

   //Keeps track of all novelties for each generation
   std::vector<double> gen_novelties;

   vec2d calculate_behaviour_characteristics();
   std::vector<double> calculate_behaviour_characteristic(vec3d& trajectories);
   double calculate_novelty(std::vector<double> bc);

   void add_to_archive(NEAT::Organism& org, double novelty, std::vector<double> bc);
   void add_to_archive(NEAT::Organism& org, std::vector<double> bc);

   vec3d read_trajectories(int org_num);

   double euclidean_distance(std::vector<double> v1, std::vector<double> v2);

};

#endif
