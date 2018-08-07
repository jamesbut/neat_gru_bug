#include "fitness_score.h"

FitnessScore::FitnessScore() :
   //MAX_RANGE(14.2),
   no_son_of_mine(false) {}

void FitnessScore::Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot) {

   m_clever_bot = clever_bot;
   m_dead_bot = dead_bot;

}

void FitnessScore::Reset(bool indv_run, CVector3 arena_size) {

   arena_size = CVector3(arena_size.GetX(), arena_size.GetY(), 0.0);
   max_range = arena_size.Length();

   robots_distance = 0;
   no_son_of_mine = false;
   fitness_score = 0;
   hit_tower = false;

   if(indv_run) CLOSE_TO_TOWER = 1.0;
   //if(indv_run) CLOSE_TO_TOWER = 0.32;
   else CLOSE_TO_TOWER = 0.32;

}

void FitnessScore::PreStep() {

   //Check for collisions
   if(no_son_of_mine==false && m_clever_bot->GetEmbodiedEntity().IsCollidingWithSomething())
      no_son_of_mine = true;

   //Calculate distance between bots
   calculate_bot_distance();

   //Terminate if the robot has reached the goal
   //No need to continue - it just wastes time
   if(robots_distance < CLOSE_TO_TOWER) {

      hit_tower = true;

      argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
      cSimulator.Terminate();

   }

}

void FitnessScore::PostExperiment() {

   //Calculate fitness
   //fitness_score = MAX_RANGE - robots_distance;
   fitness_score = max_range - robots_distance;

   if (no_son_of_mine) fitness_score /= 10;

   if (fitness_score < 0) fitness_score = 0;

   //std::cout << fitness_score << std::endl;

}

void FitnessScore::calculate_bot_distance() {

   CVector3 clever_bot_pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   CVector3 dead_bot_pos = m_dead_bot->GetEmbodiedEntity().GetOriginAnchor().Position;

   robots_distance = Distance(clever_bot_pos, dead_bot_pos);

}

RunResult FitnessScore::get_fitness_score() {

   RunResult rr;
   rr.fitness = fitness_score;
   rr.got_to_tower = hit_tower;

   return rr;

}
