#ifndef FITNESS_SCORE_H_
#define FITNESS_SCORE_H_

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

using namespace argos;

class FitnessScore {

public:

   FitnessScore();
   ~FitnessScore();

   virtual void Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot);
   virtual void Reset();
   virtual void PreStep();
   virtual void PostExperiment();

private:

   void calculate_bot_distance();

   CFootBotEntity* m_clever_bot;
   CFootBotEntity* m_dead_bot;

   double robots_distance;

};

#endif
