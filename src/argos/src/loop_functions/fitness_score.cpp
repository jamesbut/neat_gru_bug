#include "fitness_score.h"

FitnessScore::FitnessScore() {}
FitnessScore::~FitnessScore() {}

void FitnessScore::Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot) {

   m_clever_bot = clever_bot;
   m_dead_bot = dead_bot;

}

void FitnessScore::Reset() {

   robots_distance = 0;

}

void FitnessScore::PreStep() {

   calculate_bot_distance();

}

void FitnessScore::PostExperiment() {}

void FitnessScore::calculate_bot_distance() {

   //clever_bot_pos = m_clever_botGetEmbodiedEntity().GetOriginAnchor().Position

}
