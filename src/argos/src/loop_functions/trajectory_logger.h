#ifndef TRAJECTORY_LOGGER_H_
#define TRAJECTORY_LOGGER_H_

#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

using namespace argos;

class TrajectoryLogger {

   //TrajectoryLogger();
   //~TrajectoryLogger();

public:

   void Init(CFootBotEntity* clever_bot);
   void Reset();
   void PostStep();
   void PostExperiment();

private:

   CFootBotEntity* m_clever_bot;

   std::vector<CVector3> trajectory;

};


#endif
