#include "master_loop_function.h"


MasterLoopFunction::MasterLoopFunction() : GENERATE_ENVS(true), m_env_builder() {}

MasterLoopFunction::~MasterLoopFunction() = default;

void MasterLoopFunction::Init(TConfigurationNode& t_node) {

   //Find pointers to robots and their controllers
   find_robot_pointers();

   fitness_score_loop.Init(clever_bot, dead_bot);

   //if (GENERATE_ENVS) environment_generator_loop.Init();

   m_env_builder = EnvironmentBuilder();

}

void MasterLoopFunction::Reset() {

   //Get arena size for fitness function
   CVector3 arena_size =  CSimulator::GetInstance().GetSpace().GetArenaSize();

   //Build env
   if(m_reset) m_env_builder.build_env(*m_env_generator);

   if (m_noBearing)
      SetRobotPositionBasedOnMap(m_envNum, m_trialNum);

   fitness_score_loop.Reset(m_indvRun, m_envNum, m_orgNum, m_testEnvs,
                           *m_env_generator, m_noBearing);

}

void MasterLoopFunction::PreStep() {

   fitness_score_loop.PreStep();

}

void MasterLoopFunction::PostStep() {

   fitness_score_loop.PostStep();

}

void MasterLoopFunction::PostExperiment() {

   fitness_score_loop.PostExperiment();

   // if(m_indvRun) trajectory_loop.PostExperiment();

   //DEBUGGING
   // std::vector<std::vector<double> > debug_data = clever_bot_controller->GetDebugData();
   //
   // std::ofstream debug_file;
   // std::stringstream file_name;
   // file_name << "../debug/debug"<<count<<".txt";
   // debug_file.open(file_name.str());
   //
   // for(int i = 0; i < debug_data.size(); i++) {
   //    for(int j = 0; j < debug_data[i].size(); j++) {
   //          debug_file << debug_data[i][j] << ",";
   //    }
   //    debug_file << "\n";
   // }
   // std::cout << "Printed to: " << file_name.str() << std::endl;
   // debug_file.close();
   // count++;

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

RunResult MasterLoopFunction::get_fitness_score() {

   return fitness_score_loop.get_fitness_score();

}

void MasterLoopFunction::SetRobotPositionBasedOnMap(int map, int trial_num) {

   double clever_bot_x, clever_bot_y, dead_bot_x, dead_bot_y;
   CRadians clever_bot_ori, dead_bot_ori;

   switch (map) {

      case 15:

         clever_bot_x = -2.5;
         clever_bot_y = -2.5;
         dead_bot_x = 4.0;
         dead_bot_y = 4.0;

         switch (trial_num) {

            case 1:
               // clever_bot_x = -5;
               // clever_bot_y = -5;
               // dead_bot_x = 4.0;
               // dead_bot_y = -2.0;
               clever_bot_ori = (CRadians)(0);
               break;
            case 2:
               // clever_bot_x = 5;
               // clever_bot_y = -5;
               // dead_bot_x = 4.0;
               // dead_bot_y = 4.0;
               clever_bot_ori = (CRadians)(M_PI/2);
               break;
            case 3:
               // clever_bot_x = -5;
               // clever_bot_y = 5;
               // dead_bot_x = 4.0;
               // dead_bot_y = 4.0;
               clever_bot_ori = (CRadians)(M_PI);
               break;
            case 4:
               // clever_bot_x = -1;
               // clever_bot_y = 2;
               // dead_bot_x = 4.0;
               // dead_bot_y = 4.0;
               clever_bot_ori = (CRadians)(3*M_PI/2);
               break;
            case 5:
               // clever_bot_x = 3;
               // clever_bot_y = 3;
               // dead_bot_x = 1;
               // dead_bot_y = -4;
               clever_bot_ori = (CRadians)(5*M_PI/4);
               break;
            case 6:
               clever_bot_ori = (CRadians)(M_PI/4);
               break;

         }

         break;

   }


   SInitSetup clever_bot_allocation;
   SInitSetup dead_bot_allocation;

   clever_bot_allocation.Orientation.FromEulerAngles(
      clever_bot_ori,        // rotation around Z
      CRadians::ZERO, // rotation around Y
      CRadians::ZERO  // rotation around X
      );

   dead_bot_allocation.Orientation.FromEulerAngles(
      dead_bot_ori,        // rotation around Z
      CRadians::ZERO, // rotation around Y
      CRadians::ZERO  // rotation around X
      );

   clever_bot_allocation.Position.Set(clever_bot_x, clever_bot_y, 0.0);
   dead_bot_allocation.Position.Set(dead_bot_x, dead_bot_y, 0.0);

   bool clever_placed = MoveEntity(clever_bot->GetEmbodiedEntity(), clever_bot_allocation.Position, clever_bot_allocation.Orientation, false);

   bool dead_placed = MoveEntity(
          dead_bot->GetEmbodiedEntity(),     // move the body of the robot
          dead_bot_allocation.Position,                // to this position
          dead_bot_allocation.Orientation,             // with this orientation
          false                                 // this is not a check, leave the robot there
      );

   if((!dead_placed) || (!clever_placed)) {
      std::cout << "COULD NOT PLACE ROBOT!" << std::endl;
   }

}

REGISTER_LOOP_FUNCTIONS(MasterLoopFunction, "master_loop_function");
