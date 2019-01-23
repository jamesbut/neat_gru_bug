#include "trajectory_logger.h"

#include <boost/filesystem.hpp>

void TrajectoryLogger::Init(CFootBotEntity* clever_bot) {

   m_clever_bot = clever_bot;

   //Clear previous trajectories
   //boost::filesystem::remove_all("../trajectories_temp/");      This removed directories inside
   system("exec rm ../trajectories_temp/*");

}

void TrajectoryLogger::Reset(int env_num, int org_num, bool indv_run) {

   m_envNum = env_num;
   m_orgNum = org_num;
   m_indvRun = indv_run;

   //Reset trajectory
   trajectory.clear();

   //Add initial position
   CVector3 pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   trajectory.push_back(CVector2(pos.GetX(), pos.GetY()));

}

void TrajectoryLogger::PostStep() {

   CVector3 pos = m_clever_bot->GetEmbodiedEntity().GetOriginAnchor().Position;
   trajectory.push_back(CVector2(pos.GetX(), pos.GetY()));

}

void TrajectoryLogger::PostExperiment() {

   //Create directory if it already does not exist
   if (!boost::filesystem::exists("../trajectories_temp/"))
      boost::filesystem::create_directories("../trajectories_temp");

   //Create file for Novelty Search that distinguishes between organisms
   if(!m_indvRun) {

      std::ofstream trajectory_file;
      std::stringstream ns_file_name;
      ns_file_name << "../trajectories_temp/trajectory_env" << m_envNum << "_org" << m_orgNum << ".txt";

      //boost::filesystem::copy_file(file_name.str(), ns_file_name.str());

      trajectory_file.open(ns_file_name.str());

      //Insert x and y cooridnates
      for(int i = 0; i < trajectory.size(); i++)
         trajectory_file << trajectory[i].GetX() << ", " << trajectory[i].GetY() << "\n";

      trajectory_file.close();

   } else {

      //Print trajectories to be read in from Matlab

      //Create/open file
      std::ofstream trajectory_file;
      std::stringstream file_name;
      file_name << "../trajectories_temp/trajectory_" << m_envNum << ".txt";
      trajectory_file.open(file_name.str());

      //Insert x and y cooridnates
      for(int i = 0; i < trajectory.size(); i++)
         trajectory_file << trajectory[i].GetX() << ", " << trajectory[i].GetY() << "\n";

      trajectory_file.close();

   }

}
