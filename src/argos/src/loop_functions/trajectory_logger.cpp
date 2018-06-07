#include "trajectory_logger.h"

#include <boost/filesystem.hpp>

void TrajectoryLogger::Init(CFootBotEntity* clever_bot) {

   m_clever_bot = clever_bot;

}

void TrajectoryLogger::Reset(int env_num) {

   m_envNum = env_num;

   //Reset trajectory
   trajectory.clear();

   //Add initial position
   trajectory.push_back(m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position);

}

void TrajectoryLogger::PostStep() {

   trajectory.push_back(m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position);

}

void TrajectoryLogger::PostExperiment() {

   //Create directory if it already does not exist
   if (!boost::filesystem::exists("../trajectories_temp/"))
      boost::filesystem::create_directories("../trajectories_temp");

   //Create/open file
   std::ofstream trajectory_file;
   std::stringstream file_name;
   file_name << "../trajectories_temp/trajectory_" << m_envNum << ".txt";
   trajectory_file.open(file_name.str());

   //Insert x and y cooridnates
   for(int i = 0; i < trajectory.size(); i++) {

      trajectory_file << trajectory[i].GetX() << ", " << trajectory[i].GetY() << "\n";

   }

   trajectory_file.close();

}
