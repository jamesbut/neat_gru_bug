#ifndef MASTER_LOOP_FUNCTION_H_
#define MASTER_LOOP_FUNCTION_H_

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include "../controllers/neat_gru_footbot_controller.h"
#include "../controllers/empty_controller.h"
#include "fitness_score.h"
#include "kim_environment_generator.h"
#include "trajectory_logger.h"

using namespace argos;

class MasterLoopFunction : public CLoopFunctions {

public:

   MasterLoopFunction();
   ~MasterLoopFunction();

   virtual void Init(TConfigurationNode& t_node);
   virtual void Reset();
   virtual void PreStep();
   virtual void PostStep();
   virtual void PostExperiment();

   void configure_controller(NEAT::Network &net) {clever_bot_controller->SetNEATNet(net);};
   void set_env_path(std::string env_path) {m_envPath = env_path;};
   void set_env_num(int env_num) {m_envNum = env_num;};
   void set_env_reset(bool reset) {m_reset = reset;};
   void set_indv_run(bool indv_run) {m_indvRun = indv_run;};
   void set_handwritten_envs(bool handwritten) {m_handwritten = handwritten;};
   void set_trial_num(int trial_num) {m_trialNum = trial_num;};

   //double get_fitness_score();
   RunResult get_fitness_score();

private:

   void find_robot_pointers();

   void SetRobotPositionBasedOnMap(int map, int trial_num);

   CFootBotEntity* clever_bot;
   CFootBotEntity* dead_bot;

   NEATGRUFootbotController* clever_bot_controller;
   EmptyController* dead_bot_controller;

   std::string m_envPath;
   int m_envNum;
   bool m_reset;
   bool m_indvRun;
   bool m_handwritten;
   int m_trialNum;

   FitnessScore fitness_score_loop;
   RandomEnvironmentGenerator environment_generator_loop;
   TrajectoryLogger trajectory_loop;

   const bool GENERATE_ENVS;

   int count;

};

#endif
