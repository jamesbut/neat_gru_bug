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

   double get_fitness_score();

private:

   void calculate_bot_distance();

   CFootBotEntity* m_clever_bot;
   CFootBotEntity* m_dead_bot;

   double robots_distance;
   double fitness_score;

   bool no_son_of_mine;
   const double CLOSE_TO_TOWER;
   const double MAX_RANGE;

};

#endif