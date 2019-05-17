#include "fitness_score.h"
#include "astar_on_env.h"

#include <fstream>
#include <limits>

FitnessScore::FitnessScore() :
   //MAX_RANGE(14.2),
   no_son_of_mine(false),
   TEST_ENV_LENGTHS_PATH("../trajectories_temp/kims_envs_lengths/kims_envs_lengths.txt"),
   GENERATED_ENVS_MAP_PATH("../maps_temp/map_"),
   REMAINING_DIST_MAX(25.0),
   C1(1.0),
   C2(1.0) {}

void FitnessScore::Init(CFootBotEntity* clever_bot, CFootBotEntity* dead_bot) {

   m_clever_bot = clever_bot;
   m_dead_bot = dead_bot;

   trajectory_loop.Init(clever_bot);

}

void FitnessScore::Reset(bool indv_run, int env_num, int org_num, bool test_envs, EnvironmentGenerator& env_generator, bool no_bearing) {

   m_indvRun = indv_run;
   m_testEnvs = test_envs;
   m_envNum = env_num;
   m_orgNum = org_num;
   m_env_generator = &env_generator;
   m_noBearing = no_bearing;

   CVector3 arena_size = CVector3(m_env_generator->get_env_width(), m_env_generator->get_env_height(), 0.0);
   max_range = arena_size.Length();

   robots_distance = 0;
   no_son_of_mine = false;
   fitness_score = 0;
   hit_tower = false;

   if(indv_run) CLOSE_TO_TOWER = 1.0;
   else CLOSE_TO_TOWER = 0.32;

   trajectory_loop.Reset(env_num, m_orgNum, m_indvRun);

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

void FitnessScore::PostStep() {

   trajectory_loop.PostStep();

}

void FitnessScore::PostExperiment() {

   trajectory_loop.PostExperiment();

   //Calculate fitness differently depending on different type of exp
   if(m_noBearing) calculate_fitness_no_bearing();
   else calculate_fitness();

}

double FitnessScore::calculate_fitness() {

   const std::vector<CVector2>& trajectory = trajectory_loop.get_trajectory();
   //std::cout << "Traj size: " << trajectory.size() << std::endl;
   // for(int i = 0; i < trajectory.size(); i++)
   //    std::cout << trajectory[i].GetX() << " " << trajectory[i].GetY() << std::endl;

   traj_per_astar = calculate_trajectory_per_optimal_path(trajectory);

   //Calculate fitness
   /*    F1   */
   //double distance_from_tower = max_range - robots_distance;

   //if (no_son_of_mine) distance_from_tower /= 10;

   //distance_from_tower_w_crash = distance_from_tower;

   //fitness_score = distance_from_tower_w_crash;

   /*    F2   */
   // double distance_from_tower = max_range - robots_distance;
   //
   // if (no_son_of_mine) distance_from_tower /= 10;
   //
   // distance_from_tower_w_crash = distance_from_tower;
   //
   // fitness_score = (C1 * distance_from_tower_w_crash) - (C2 * traj_per_astar);

   /*    F3   */
   //Calculate remaining distance according to astar
   // CVector3 clever_bot_pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   //
   // //The 7 and 10 here is switching from the simulation coordinates to the search coordinates
   // argos::CVector2 bot_pos = argos::CVector2((clever_bot_pos.GetX()+7) * 10, (clever_bot_pos.GetY()+7) * 10);
   // double remaining_distance_from_tower =  m_env_generator->calculate_remaining_distance_from(bot_pos);
   //
   // fitness_score = REMAINING_DIST_MAX - remaining_distance_from_tower;
   //
   // if(no_son_of_mine) fitness_score /= 10;

   /*    F4   */
   // CVector3 clever_bot_pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   //
   // //The 7 and 10 here is switching from the simulation coordinates to the search coordinates
   // argos::CVector2 bot_pos = argos::CVector2((clever_bot_pos.GetX()+7) * 10, (clever_bot_pos.GetY()+7) * 10);
   // double remaining_distance_from_tower =  m_env_generator->calculate_remaining_distance_from(bot_pos);
   //
   // fitness_score = (C1 * (REMAINING_DIST_MAX - remaining_distance_from_tower)) - (C2 * traj_per_astar);
   //
   // if(no_son_of_mine) fitness_score /= 10;

   /*   F5   */
   // fitness_score = hit_tower ? 1 : 0;
   //
   // std::cout << "Fitness score: " << fitness_score;
   //
   // if(no_son_of_mine)
   //    fitness_score /= 10;
   //
   // std::cout << " " << fitness_score << std::endl;

   /*   F6   */
   //Calculate remaining distance according to astar
   // CVector3 clever_bot_pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   //
   // //The 7 and 10 here is switching from the simulation coordinates to the search coordinates
   // argos::CVector2 bot_pos = argos::CVector2((clever_bot_pos.GetX()+7) * 10, (clever_bot_pos.GetY()+7) * 10);
   // double remaining_distance_from_tower =  m_env_generator->calculate_remaining_distance_from(bot_pos);
   //
   // //Normalise fitnesses between 0 and 1
   // double remaining_dist_norm = 1 / (pow(remaining_distance_from_tower, 0.5) + 0.01);
   // //double traj_per_astar_norm = 1 / (pow(traj_per_astar, 0.8) + 0.01);
   // double traj_per_astar_norm = 1 / (exp(traj_per_astar*3 - 7) + 1);
   //
   // // std::cout << "Remaining distance from tower: " << remaining_distance_from_tower << std::endl;
   // // std::cout << "Remaining distance from tower norm: " << remaining_dist_norm << std::endl;
   // // std::cout << "Traj per astar: " << traj_per_astar << std::endl;
   // // std::cout << "Traj per astar norm: " << traj_per_astar_norm << std::endl;
   //
   // fitness_score = remaining_dist_norm + traj_per_astar_norm;
   //
   // if(no_son_of_mine) fitness_score /= 10;


   /*   F7   c=1.5 */
   /*   F8   c=0.5 */
   //If the robot hits the target, reward with a function of path length
   // std::cout << "Hit tower: " << hit_tower << std::endl;
   //std::cout << "Traj per astar: " << traj_per_astar << std::endl;
   //fitness_score = hit_tower ? (-0.25*traj_per_astar + 1.25) : 0;

   // fitness_score = hit_tower ? (1 / pow(traj_per_astar, 0.5)) : 0;

   // if(no_son_of_mine)
   //    fitness_score /= 10;

   /*   F9   - alpha=0.25,  K=1   */
   /*   F10   - alpha=0.5,  K=1   */
   /*   F11  - alpha=0.25,  K=2   */
   //Calculate remaining distance according to astar
   CVector3 clever_bot_pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;

   //The 7 and 10 here is switching from the simulation coordinates to the search coordinates
   argos::CVector2 bot_pos = argos::CVector2((clever_bot_pos.GetX()+7) * 10, (clever_bot_pos.GetY()+7) * 10);
   double remaining_astar_distance =  m_env_generator->calculate_remaining_distance_from(bot_pos);

   double astar_length = m_env_generator->get_environment_optimal_length();

   //Normalise with Astar length from start
   //And it is inversely proportional to the remaining astar distance
   const double ALPHA = 0.25;
   //const double ALPHA = 0.5;
   const double K = 1;
   //const double K = 2;
   double bounded_remaining_dist = tanh(ALPHA*(astar_length / remaining_astar_distance));

   //std::cout << "A star length: " << astar_length << std::endl;
   //std::cout << "remaining_astar_distance: " << remaining_astar_distance << std::endl;
   //std::cout << "Bounded remaining distance: " << bounded_remaining_dist << std::endl;

   fitness_score = hit_tower ? ((1 / pow(traj_per_astar, 0.5)) + K) : bounded_remaining_dist;

   //std::cout << (1 / pow(traj_per_astar, 0.5)) + K << std::endl;

   //std::cout << "Fitness: " << fitness_score << std::endl;

   if(no_son_of_mine)
      fitness_score /= 10;

   //std::cout << " " << fitness_score << std::endl;

   //std::cout << "Max range: " << max_range << std::endl;
   // std::cout << "Robots distance " << robots_distance << std::endl;
   //std::cout << "Fitness score: " << fitness_score << std::endl;
   //std::cout << "Remaining distance from tower: " << remaining_distance_from_tower << std::endl;

   //Capture any fitness that is below 0
   if (fitness_score < 0) fitness_score = 0;

}

double FitnessScore::calculate_fitness_no_bearing() {

   double distance_from_tower = max_range - robots_distance;

   if (no_son_of_mine) distance_from_tower /= 10;

   /*  F1  */
   //fitness_score = distance_from_tower;
   //std::cout << fitness_score << std::endl;
   //std::cout << distance_from_tower << std::endl;
   //std::cout << pow(distance_from_tower, 3) << std::endl;
   //std::cout << "--------" << std::endl;

   /*  F2  */
   fitness_score = pow(distance_from_tower, 3);

   if (fitness_score < 0) fitness_score = 0;

}

double FitnessScore::calculate_trajectory_per_optimal_path(const std::vector<CVector2>& trajectory) {

   double astar_length = m_env_generator->get_environment_optimal_length();

   //std::cout << "astar length: " << astar_length << std::endl;

   //Calculate trajectory length
   double trajectory_length = calculate_trajectory_length(trajectory);
   trajectory_length += 1.0;      //Add 1 for the extra 1 meter stopped before the tower

   //std::cout << "Traj length: " << trajectory_length << std::endl;

   return (double)trajectory_length / (double)astar_length;

}


double FitnessScore::calculate_trajectory_length(const std::vector<CVector2>& traj) {

   double length = 0.0;

   for(size_t i = 0; i < traj.size()-1; i++) {

      double x_diff = std::abs(traj[i].GetX() - traj[i+1].GetX());
      double y_diff = std::abs(traj[i].GetY() - traj[i+1].GetY());

      length += sqrt(pow(x_diff, 2) + pow(y_diff, 2));

   }

   return length;

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
   rr.distance_from_tower_w_crash = distance_from_tower_w_crash;
   rr.traj_per_astar = traj_per_astar;
   //std::cout << rr.traj_per_astar << std::endl;
   return rr;

}
