#ifndef FITNESS_SCORE_H_
#define FITNESS_SCORE_H_

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include "trajectory_logger.h"

using namespace argos;

struct RunResult {

   double fitness;
   bool got_to_tower;

};

class FitnessScore {

public:

   FitnessScore();

   void Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot);
   void Reset(bool indv_run, CVector3 arena_size, int env_num, std::string env_path, bool test_envs);
   void PreStep();
   void PostStep();
   void PostExperiment();

   RunResult get_fitness_score();
   double calculate_trajectory_per_optimal_path(const std::vector<CVector2>& trajectory);
   double calculate_trajectory_length(const std::vector<CVector2>& traj);

private:

   void calculate_bot_distance();

   CFootBotEntity* m_clever_bot;
   CFootBotEntity* m_dead_bot;

   TrajectoryLogger trajectory_loop;

   double robots_distance;
   double fitness_score;

   bool no_son_of_mine;
   double CLOSE_TO_TOWER;
   double max_range;

   bool hit_tower;

   bool m_indvRun;
   bool m_testEnvs;
   int m_envNum;
   std::string m_envPath;

   const std::string TEST_ENV_LENGTHS_PATH;
   const std::string GENERATED_ENVS_MAP_PATH;

};

double get_value_at_line(std::string file_name, unsigned int num);

#endif
