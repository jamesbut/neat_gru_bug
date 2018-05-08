#ifndef MASTER_LOOP_FUNCTION_H_
#define MASTER_LOOP_FUNCTION_H_

#include <argos3/core/simulator/loop_functions.h>
//#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

using namespace argos;

class MasterLoopFunction : public CLoopFunctions {

public:

   MasterLoopFunction();
   ~MasterLoopFunction();

   virtual void Init(TConfigurationNode& t_node);

private:

   //CFootBotEntity* clever_bot;
   //CFootBotEntity* dead_bot;

};

#endif
