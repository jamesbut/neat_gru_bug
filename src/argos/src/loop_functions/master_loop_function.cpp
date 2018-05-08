#include "master_loop_function.h"

MasterLoopFunction::MasterLoopFunction() {}

MasterLoopFunction::~MasterLoopFunction() {}

void MasterLoopFunction::Init(TConfigurationNode& t_node) {

   // Find pointers to robots and their controllers
   find_robot_pointers();

}

void MasterLoopFunction::Reset() {}

void MasterLoopFunction::configure_controller(NEAT::Network &net) {

   clever_bot_controller->SetNEATNet(net);

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
          else if (fb_pointer->GetId() == "bot1") dead_bot = fb_pointer;
          else std::cout << "COULD NOT FIND ROBOT ENTITY - CHECK .argos FILE" << std::endl;

       }

}

REGISTER_LOOP_FUNCTIONS(MasterLoopFunction, "master_loop_function");
