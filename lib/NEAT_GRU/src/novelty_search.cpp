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

//Calculates the current populations BCs according to their trajectories
vec2d NoveltySearch::calculate_behaviour_characteristics() {

   //BCs for each organism
   vec2d bcs;

   for(unsigned int i = 0; i < POP_SIZE; i++) {

      vec3d trajectories = read_trajectories(i);

      std::vector<double> bc = calculate_behaviour_characteristic(trajectories);

      for(int j = 0; j < bc.size(); j++)
         std::cout << bc[j] << " ";

      std::cout << std::endl;

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
double NoveltySearch::calculate_novelty() {}

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

   //TODO: Need to modify trajectories to include final time steps if the agent
   //finished before the time limit

   return trajectories;

}


void NoveltySearch::add_to_archive(NEAT::Organism &org) {}
