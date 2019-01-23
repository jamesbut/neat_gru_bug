#ifndef MASTER_LOOP_FUNCTION_H_
#define MASTER_LOOP_FUNCTION_H_

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include "../controllers/neat_gru_footbot_controller.h"
#include "../controllers/empty_controller.h"
#include "fitness/fitness_score.h"
#include "environment/kim_environment_generator.h"
#include "environment/environment_builder.h"

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

   void configure_controller(NEAT::Network &net, const bool no_bearing) {clever_bot_controller->SetNEATNet(net);
                                                                          clever_bot_controller->SetNoBearing(no_bearing);};
   void set_env_path(std::string env_path) {m_envPath = env_path;};
   void set_env_num(int env_num) {m_envNum = env_num;};
   void set_org_num(int org_num) {m_orgNum = org_num;};
   void set_env_reset(bool reset) {m_reset = reset;};
   void set_indv_run(bool indv_run) {m_indvRun = indv_run;};
   void set_no_bearing(bool no_bearing) {m_noBearing = no_bearing;};
   void set_test_envs(bool test_envs) {m_testEnvs = test_envs;};
   void set_trial_num(int trial_num) {m_trialNum = trial_num;};
   void set_random_seed(int rand_seed) {m_randSeed = rand_seed;};
   void set_env_generator(EnvironmentGenerator& env_generator) {m_env_generator = &env_generator;};

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
   int m_orgNum;
   bool m_reset;
   bool m_indvRun;
   bool m_noBearing;
   bool m_testEnvs;
   int m_trialNum;
   int m_randSeed;

   FitnessScore fitness_score_loop;
   RandomEnvironmentGenerator environment_generator_loop;

   const bool GENERATE_ENVS;

   int count;

   // std::unique_ptr<EnvironmentGenerator> m_env_generator;
   EnvironmentGenerator* m_env_generator;
   EnvironmentBuilder m_env_builder;

};

#endif
