#ifndef FITNESS_SCORE_H_
#define FITNESS_SCORE_H_

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include "trajectory_logger.h"
#include "../environment/environment_generator.h"

using namespace argos;

struct RunResult {

   double fitness;
   bool got_to_tower;
   double distance_from_tower_w_crash;
   double traj_per_astar;

};

class FitnessScore {

public:

   FitnessScore();

   void Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot);
   // void Reset(bool indv_run, CVector3 arena_size, int env_num, std::string env_path, bool test_envs);
   void Reset(bool indv_run, int env_num, bool test_envs, EnvironmentGenerator& env_generator, bool handwritten);
   void PreStep();
   void PostStep();
   void PostExperiment();

   RunResult get_fitness_score();
   double calculate_trajectory_per_optimal_path(const std::vector<CVector2>& trajectory);
   double calculate_trajectory_length(const std::vector<CVector2>& traj);

   double calculate_fitness();
   double calculate_fitness_handwritten();

private:

   void calculate_bot_distance();

   CFootBotEntity* m_clever_bot;
   CFootBotEntity* m_dead_bot;

   TrajectoryLogger trajectory_loop;
   //std::unique_ptr<EnvironmentGenerator> m_env_generator;
   EnvironmentGenerator* m_env_generator;

   double robots_distance;
   double fitness_score;
   double distance_from_tower_w_crash;
   double traj_per_astar;

   bool no_son_of_mine;
   double CLOSE_TO_TOWER;
   double max_range;

   bool hit_tower;

   bool m_indvRun;
   bool m_testEnvs;
   int m_envNum;
   std::string m_envPath;
   bool m_handwritten;

   const std::string TEST_ENV_LENGTHS_PATH;
   const std::string GENERATED_ENVS_MAP_PATH;

   //Fitness score constants
   const float C1;
   const float C2;

   const double REMAINING_DIST_MAX;

};

#endif
