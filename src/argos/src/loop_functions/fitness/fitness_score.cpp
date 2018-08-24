#include "fitness_score.h"
#include "astar_on_env.h"

#include <fstream>
#include <limits>

FitnessScore::FitnessScore() :
   //MAX_RANGE(14.2),
   no_son_of_mine(false),
   TEST_ENV_LENGTHS_PATH("../trajectories_temp/kims_envs_lengths/kims_envs_lengths.txt"),
   GENERATED_ENVS_MAP_PATH("../maps_temp/map_") {}

void FitnessScore::Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot) {

   m_clever_bot = clever_bot;
   m_dead_bot = dead_bot;

   trajectory_loop.Init(clever_bot);

}

void FitnessScore::Reset(bool indv_run, CVector3 arena_size, int env_num, std::string env_path, bool test_envs) {

   m_indvRun = indv_run;
   m_envPath = env_path;
   m_testEnvs = test_envs;
   m_envNum = env_num;

   arena_size = CVector3(arena_size.GetX(), arena_size.GetY(), 0.0);
   max_range = arena_size.Length();

   robots_distance = 0;
   no_son_of_mine = false;
   fitness_score = 0;
   hit_tower = false;

   if(indv_run) CLOSE_TO_TOWER = 1.0;
   //if(indv_run) CLOSE_TO_TOWER = 0.32;
   else CLOSE_TO_TOWER = 0.32;

   trajectory_loop.Reset(env_num);

}

void FitnessScore::PreStep() {

   //Check for collisions
   if(no_son_of_mine==false && m_clever_bot->GetEmbodiedEntity().IsCollidingWithSomething())
      no_son_of_mine = true;

   //Calculate distance between bots
   calculate_bot_distance();

   //Terminate if the robot has reached the goal
   //No need to continue - it just wastes time
   if(robots_distance < CLOSE_TO_TOWER) {

      hit_tower = true;

      argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
      cSimulator.Terminate();

   }

}

void FitnessScore::PostStep() {

   trajectory_loop.PostStep();

}

void FitnessScore::PostExperiment() {

   trajectory_loop.PostExperiment();

   const std::vector<CVector2>& trajectory = trajectory_loop.get_trajectory();

   traj_per_astar = calculate_trajectory_per_optimal_path(trajectory);

   //Calculate fitness

   double distance_from_tower = max_range - robots_distance;

   if (no_son_of_mine) distance_from_tower /= 10;

   distance_from_tower_w_crash = distance_from_tower;

   fitness_score = distance_from_tower_w_crash;

   if (fitness_score < 0) fitness_score = 0;

   //Old calculate fitness
   // fitness_score = max_range - robots_distance;
   //
   // if (no_son_of_mine) fitness_score /= 10;
   //
   // if (fitness_score < 0) fitness_score = 0;

   //std::cout << fitness_score << std::endl;

}

//Calculate the trajectory length divided by the astar length
double FitnessScore::calculate_trajectory_per_optimal_path(const std::vector<CVector2>& trajectory) {

   double astar_length;

   //If it is in test set, just read from file
   if(m_testEnvs) {

      astar_length = get_value_at_line(TEST_ENV_LENGTHS_PATH, m_envNum);

   } else {

      //Calculate A* path on environment
      m_envPath = GENERATED_ENVS_MAP_PATH + std::to_string(m_envNum) + ".png";
      //m_envPath = "../argos_params/environments/kim_envs/rand_env_" + std::to_string(m_envNum) + ".png";

      std::vector<CVector2> astar_path = astar_on_env(m_envPath);

      std::vector<CVector2> astar_path_divided_by_ten(astar_path.size());

      //Divide astar path by 10 because the image that is planned over is 140x140
      //whereas the environment is 14x14
      std::transform(astar_path.begin(), astar_path.end(), astar_path_divided_by_ten.begin(),
                     [](const CVector2& pos){return pos / 10;});

      astar_length = calculate_trajectory_length(astar_path_divided_by_ten);

   }

   //Calculate trajectory length
   double trajectory_length = calculate_trajectory_length(trajectory);
   trajectory_length += 1.0;      //Add 1 for the extra 1 meter stopped before the tower

   //For now, deposit this into kims_envs_lengths.txt
   // std::ofstream trajectory_file;
   // std::stringstream file_name;
   // file_name << "../trajectories_temp/kims_envs_lengths/kims_envs_lengths.txt";
   // trajectory_file.open(file_name.str(), std::ios_base::app);
   //
   // trajectory_file << astar_length <<std::endl;
   //
   // trajectory_file.close();

   //std::cout << trajectory_length << " " << astar_length << std::endl;

   return (double)trajectory_length / (double)astar_length;

}

double FitnessScore::calculate_trajectory_length(const std::vector<CVector2>& traj) {
   //std::cout << "Calc traj length" << std::endl;
   double length = 0.0;

   //Debug
   // int num_straights = 0;
   // int num_diags = 0;
   //std::cout << traj.size()-1 << std::endl;
   for(size_t i = 0; i < traj.size()-1; i++) {
      //std::cout << "Hello" << std::endl;
      double x_diff = std::abs(traj[i].GetX() - traj[i+1].GetX());
      double y_diff = std::abs(traj[i].GetY() - traj[i+1].GetY());

      // if((x_diff != 0) && (y_diff != 0)) num_diags += 1;
      // else if((x_diff != 0) || (y_diff != 0)) num_straights += 1;

      length += sqrt(pow(x_diff, 2) + pow(y_diff, 2));

   }

   //std::cout << "Num straights: " << num_straights << std::endl;
   //std::cout << "Num diags: " << num_diags << std::endl;
   //std::cout << length <<std::endl;
   return length;

}

void FitnessScore::calculate_bot_distance() {

   CVector3 clever_bot_pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   CVector3 dead_bot_pos = m_dead_bot->GetEmbodiedEntity().GetOriginAnchor().Position;

   robots_distance = Distance(clever_bot_pos, dead_bot_pos);

}

RunResult FitnessScore::get_fitness_score() {

   RunResult rr;
   rr.fitness = fitness_score;
   rr.got_to_tower = hit_tower;
   rr.distance_from_tower_w_crash = distance_from_tower_w_crash;
   rr.traj_per_astar = traj_per_astar;
   //std::cout << rr.traj_per_astar << std::endl;
   return rr;

}

double get_value_at_line(std::string file_name, unsigned int num) {

   std::fstream file(file_name);

   file.seekg(std::ios::beg);
   for(int i=0; i < num - 1; ++i){
     file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
   }

   double value;
   file >> value;

   return value;

}
