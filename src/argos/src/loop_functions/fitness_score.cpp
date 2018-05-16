#include "fitness_score.h"

FitnessScore::FitnessScore() :
   CLOSE_TO_TOWER(1.0),    //TODO: Change this based on individual run or not
   MAX_RANGE(14.2),
   no_son_of_mine(false) {}

FitnessScore::~FitnessScore() {}

void FitnessScore::Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot) {

   m_clever_bot = clever_bot;
   m_dead_bot = dead_bot;

}

void FitnessScore::Reset() {

   robots_distance = 0;
   no_son_of_mine = false;
   fitness_score = 0;

}

void FitnessScore::PreStep() {

   //Calculate distance between bots
   calculate_bot_distance();

   //Terminate if the robot has reached the goal
   //No need to continue - it just wastes time
   if(robots_distance < CLOSE_TO_TOWER) {

      argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
      cSimulator.Terminate();

   }

}

void FitnessScore::PostExperiment() {

   //Calculate fitness
   fitness_score = MAX_RANGE - robots_distance;

   if (no_son_of_mine) fitness_score /= 10;

   if (fitness_score < 0) fitness_score = 0;

}

void FitnessScore::calculate_bot_distance() {

   CVector3 clever_bot_pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   CVector3 dead_bot_pos = m_dead_bot->GetEmbodiedEntity().GetOriginAnchor().Position;

   robots_distance = Distance(clever_bot_pos, dead_bot_pos);

}

double FitnessScore::get_fitness_score() {

   return fitness_score;

}
