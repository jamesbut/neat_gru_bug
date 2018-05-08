#include <iostream>

#include "argos_simulation.h"
#include <argos3/core/simulator/simulator.h>

ARGoS_simulation::ARGoS_simulation() {

   ARGOS_FILE_NAME = "../argos_params/no_walls.argos";

   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
   cSimulator.SetExperimentFileName(ARGOS_FILE_NAME);
   std::cout << "Loading.." << std::endl;
   cSimulator.LoadExperiment();
   std::cout << "Loaded" << std::endl;

}

ARGoS_simulation::~ARGoS_simulation() {}

void ARGoS_simulation::run() {

   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
   cSimulator.Reset();
   cSimulator.Execute();

   // Call terminate?
   cSimulator.Terminate();

}
