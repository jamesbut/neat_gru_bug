#ifndef TRAJECTORY_LOGGER_H_
#define TRAJECTORY_LOGGER_H_

#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

using namespace argos;

class TrajectoryLogger {

public:

   void Init(CFootBotEntity* clever_bot);
   void Reset(int env_num, int org_num, bool indv_run);
   void PostStep();
   void PostExperiment();

   inline const std::vector<CVector2>& get_trajectory() {return trajectory;}

private:

   CFootBotEntity* m_clever_bot;

   std::vector<CVector2> trajectory;

   int m_envNum;
   int m_orgNum;
   bool m_indvRun;

};


#endif
