#include "master_loop_function.h"


MasterLoopFunction::MasterLoopFunction() : GENERATE_ENVS(true) {}

MasterLoopFunction::~MasterLoopFunction() {}

void MasterLoopFunction::Init(TConfigurationNode& t_node) {

   //Find pointers to robots and their controllers
   find_robot_pointers();

   fitness_score_loop.Init(clever_bot, dead_bot);
   if (GENERATE_ENVS) environment_generator_loop.Init();

}

void MasterLoopFunction::Reset() {

   fitness_score_loop.Reset();

   if (GENERATE_ENVS && m_reset) {
      environment_generator_loop.ClearEnvironment();
      environment_generator_loop.Reset(m_envPath);
   }

}

void MasterLoopFunction::PreStep() {

   fitness_score_loop.PreStep();

}

void MasterLoopFunction::PostExperiment() {

   fitness_score_loop.PostExperiment();

}

void MasterLoopFunction::find_robot_pointers() {

   /* Get the map of all foot-bots from the space */
   CSpace::TMapPerType& tFBMap =  CSimulator::GetInstance().GetSpace().GetEntitiesByType("foot-bot");
   /* Go through them */
   for(CSpace::TMapPerType::iterator it = tFBMap.begin();
       it != tFBMap.end();
       ++it) {

          CFootBotEntity* fb_pointer = any_cast<CFootBotEntity*>(it->second);

          if (fb_pointer->GetId() == "bot0") {
             clever_bot = fb_pointer;
             clever_bot_controller = &dynamic_cast<NEATGRUFootbotController&>(clever_bot->GetControllableEntity().GetController());
          }
          else if (fb_pointer->GetId() == "bot1") {
             dead_bot = fb_pointer;
             dead_bot_controller = &dynamic_cast<EmptyController&>(dead_bot->GetControllableEntity().GetController());
          }
          else std::cout << "COULD NOT FIND ROBOT ENTITY - CHECK .argos FILE" << std::endl;

       }

}

double MasterLoopFunction::get_fitness_score() {

   return fitness_score_loop.get_fitness_score();

}

REGISTER_LOOP_FUNCTIONS(MasterLoopFunction, "master_loop_function");
