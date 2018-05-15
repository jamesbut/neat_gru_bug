#include <iostream>

#include "argos_simulation.h"
#include <argos3/core/simulator/simulator.h>
#include "loop_functions/master_loop_function.h"

#include <signal.h>

ARGoS_simulation::ARGoS_simulation(const std::string& argos_file) :
   ARGOS_FILE_NAME(argos_file) {

   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
   cSimulator.SetExperimentFileName(ARGOS_FILE_NAME);
   std::cout << "Loading.." << std::endl;
   cSimulator.LoadExperiment();
   std::cout << "Loaded" << std::endl;

}

ARGoS_simulation::~ARGoS_simulation() {}

double ARGoS_simulation::run(NEAT::Organism &org, int env_num) {

   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

   MasterLoopFunction& master_loop = dynamic_cast<MasterLoopFunction&>(cSimulator.GetLoopFunctions());

   master_loop.configure_controller(*org.net);
   master_loop.set_env_num(env_num);

   cSimulator.Reset();
   cSimulator.Execute();

   return master_loop.get_fitness_score();

}
