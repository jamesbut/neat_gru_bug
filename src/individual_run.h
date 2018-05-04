#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <iostream>
#include <fstream>
#include <NEAT_GRU/include/genome.h>
#include <NEAT_GRU/include/organism.h>
#include "ros/ros.h"
#include "neat_ros/FinishedSim.h"
#include "std_srvs/Empty.h"
#include <boost/thread.hpp>

extern const std::string TEMP_GNOME_FILENAME;

class IndividualRun {

   public:

      IndividualRun(const std::string& gf);
      ~IndividualRun();

      void run();

   private:

      bool finishedSimCallback(neat_ros::FinishedSim::Request  &req,
                               neat_ros::FinishedSim::Response &res);
      bool stop_run(std_srvs::Empty::Request  &req,
                                   std_srvs::Empty::Request &res);

      void finishedSimServiceThread();

      void readGenomeFile();
      void evalIndividual(int run_num);

      std::string genomeFile;

      NEAT::Genome* genom;
      NEAT::Organism* org;

      boost::thread spin_thread;
      bool sim_running;

      const int NUM_RUNS;
      int num_finishes;

};

#endif
