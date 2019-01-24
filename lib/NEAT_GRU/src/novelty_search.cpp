#include "novelty_search.h"

#include <sstream>

NoveltySearch::NoveltySearch(const double NOVELTY_THRESHOLD, const int K, const int NUM_ENVS,
                             const int POP_SIZE, const int TRAJ_SIZE):
   NOVELTY_THRESHOLD(NOVELTY_THRESHOLD),
   K(K),
   NUM_ENVS(NUM_ENVS),
   POP_SIZE(POP_SIZE),
   TRAJ_SIZE(TRAJ_SIZE) {



}

void NoveltySearch::evaluate_population(NEAT::Population& pop) {

   std::cout << "Starting Novelty Search.." << std::endl;

   //Clear previous generational novelties
   gen_novelties.clear();

   //Calcualate behaviour characteristics of current population
   vec2d bcs = calculate_behaviour_characteristics();

   //Initialise novelty archive if it is not initialised yet
   if(novelty_archive.size() == 0)
      add_to_archive(*(pop.organisms[0]), bcs[0]);

   //Calculate individual novelties and add to archive if above the
   //novelty threshold
   for(unsigned int i = 0; i < bcs.size(); i++) {

      double novelty = calculate_novelty(bcs[i]);
      gen_novelties.push_back(novelty);
      //std::cout << "Novelty: " << novelty << std::endl;

      if(novelty > NOVELTY_THRESHOLD)
         add_to_archive(*(pop.organisms[i]), novelty, bcs[i]);

   }



   // for(unsigned int i = 0; i < bcs.size(); i++) {
   //    std::cout << "----------" << std::endl;
   //    // for(unsigned int j = 0; j < bcs[i].size(); j++) {
   //    //    std::cout << bcs[i][j] << ", " << std::endl;
   //    // }
   //    std::cout << novelties[i] << std::endl;
   //    std::cout << "----------" << std::endl;
   // }

   std::cout << "Archive Size: " << novelty_archive.size() << std::endl;

   std::cout << "Finished Novelty Search!" << std::endl;

}

//Calculates the current populations BCs according to their trajectories
vec2d NoveltySearch::calculate_behaviour_characteristics() {

   //BCs for each organism
   vec2d bcs;

   for(unsigned int i = 0; i < POP_SIZE; i++) {

      vec3d trajectories = read_trajectories(i);

      std::vector<double> bc = calculate_behaviour_characteristic(trajectories);

      // for(int j = 0; j < bc.size(); j++)
      //    std::cout << bc[j] << " ";
      //
      // std::cout << std::endl;

      bcs.push_back(bc);

   }

   return bcs;

}

//Calculates an individuals BC
std::vector<double> NoveltySearch::calculate_behaviour_characteristic(vec3d& trajectories) {

   std::vector<double> bc;

   //Will use a similar BC to Shorten 2005 for now
   //I will sample every 300 time steps giving a BC
   //of size 600 for 10 mazes
   //I can reduce this in future if needed

   // std::cout << trajectories.size() << std::endl;
   // std::cout << trajectories[0].size() << std::endl;
   // std::cout << trajectories[0][31].size() << std::endl;

   const int SAMPLE_TIME = 300;

   for(unsigned int i = 0; i < NUM_ENVS; i++) {

      for(unsigned int j = SAMPLE_TIME; j <= TRAJ_SIZE; j += SAMPLE_TIME) {

         // std::cout << "i: " << i << " j: " << j << std::endl;
         // std::cout << "x: " << trajectories[i][j][0] << " y: " << trajectories[i][j][1] << std::endl;

         bc.push_back(trajectories[i][j][0]);
         bc.push_back(trajectories[i][j][1]);

      }

   }

   return bc;

}

//Need to calculate sparseness of each point with respect to archive and
//current population
double NoveltySearch::calculate_novelty(std::vector<double> bc) {

   std::vector<double> distances;

   //Calculate novelties with relation to archive
   for(unsigned int i = 0; i < novelty_archive.size(); i++) {
      distances.push_back(euclidean_distance(bc, novelty_archive[i].bc));
      // for(unsigned int j = 0; j < bc.size(); j++){
      //    std::cout << bc[j] << " ";
      // }
      // std::cout << std::endl;
      // for(unsigned int j = 0; j < novelty_archive[i].bc.size(); j++){
      //    std::cout << novelty_archive[i].bc[j] << " ";
      // }
      // std::cout << std::endl;
      // std::cout << distances[i] << std::endl;
   }

   //std::cout << "--------------" << std::endl;

   //std::cout << "Distances size: " << distances.size() << std::endl;

   //Order distances
   std::sort(distances.begin(), distances.end());

   //Find sparseness of point
   //Find average distance to k nearest neighbours
   double distance_sum = 0.0;

   //If there are less than K neighbours in the archive then
   //just average over the number in the archive
   int num_neighbours = (distances.size() < K) ? distances.size() : K;

   for(unsigned int i = 0; i < num_neighbours; i++)
      distance_sum += distances[i];

   //std::cout << "Distance sum: " << distance_sum << std::endl;

   double avg_dist = distance_sum / (double)num_neighbours;

   //Now I think this is now the novelty score
   return avg_dist;

}

//Read trajectories from file
//For each organism, there are a number of trajectories for each environment
vec3d NoveltySearch::read_trajectories(int org_num) {

   vec3d trajectories;

   for(unsigned int i = 0; i < NUM_ENVS; i++) {

      std::stringstream file_name;
      file_name << "../trajectories_temp/trajectory_env" << i+1 << "_org" << org_num << ".txt";
      std::ifstream file(file_name.str());
      std::string line;

      vec2d trajectory;

      while(getline(file, line)) {

         std::stringstream ss(line);
         std::string data;
         std::vector<double> traj_point;

         while(getline(ss, data, ',')) {

            traj_point.push_back(stod(data));

         }

         trajectory.push_back(traj_point);

      }

      trajectories.push_back(trajectory);

   }

   //Modify trajectories to include final time steps if the agent
   //finished before the time limit
   for(unsigned int i = 0; i < trajectories.size(); i++)
      while(trajectories[i].size() < TRAJ_SIZE+1)
         trajectories[i].push_back(trajectories[i].back());

   return trajectories;

}

double NoveltySearch::euclidean_distance(std::vector<double> v1, std::vector<double> v2) {

   //Check for strange behaviour
   if(v1.size() != v2.size()) {
      std::cout << "Novelty search vectors are different sizes! Terminating.." <<std::endl;
      exit(EXIT_FAILURE);
   }

   double sum_of_square_diffs = 0.0;

   for(unsigned int i = 0; i < v1.size(); i++)
      sum_of_square_diffs += pow((v1[i] - v2[i]), 2);

   return sqrt(sum_of_square_diffs);

}

void NoveltySearch::add_to_archive(NEAT::Organism& org, double novelty, std::vector<double> bc) {

   NoveltyItem new_item;

   new_item.org = new NEAT::Organism(org);
   new_item.bc = bc;
   new_item.novelty = novelty;
   new_item.tested_on_eval_set = false;
   new_item.printed = false;

   novelty_archive.push_back(new_item);

}

void NoveltySearch::add_to_archive(NEAT::Organism& org, std::vector<double> bc) {

   NoveltyItem new_item;

   new_item.org = new NEAT::Organism(org);
   new_item.bc = bc;
   new_item.novelty = -1.0;
   new_item.tested_on_eval_set = false;
   new_item.printed = false;

   novelty_archive.push_back(new_item);

}
