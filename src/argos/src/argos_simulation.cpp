#include <iostream>

#include "argos_simulation.h"
#include <argos3/core/simulator/simulator.h>
#include "loop_functions/master_loop_function.h"

ARGoS_simulation::ARGoS_simulation() {}

ARGoS_simulation::ARGoS_simulation(const std::string& argos_file) :
   ARGOS_FILE_NAME(argos_file) {

   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
   cSimulator.SetExperimentFileName(ARGOS_FILE_NAME);

   cSimulator.LoadExperiment();

}

ARGoS_simulation::~ARGoS_simulation() {}

RunResult ARGoS_simulation::run(NEAT::Organism &org, int env_num, bool reset, bool indv_run,
                                bool no_bearing, bool test_envs, int trial_num,
                                EnvironmentGenerator& env_generator) {

   // std::cout << env_path << std::endl;
   // std::cout << env_num << std::endl;
   // std::cout << "RESET: " << reset << std::endl;
   // std::cout << indv_run << std::endl;
   // std::cout << handwritten << std::endl;
   // std::cout << trial_num << std::endl;

   argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

   MasterLoopFunction& master_loop = dynamic_cast<MasterLoopFunction&>(cSimulator.GetLoopFunctions());

   master_loop.configure_controller(*org.net, no_bearing);
   master_loop.set_env_num(env_num);
   master_loop.set_env_reset(reset);
   master_loop.set_indv_run(indv_run);
   master_loop.set_no_bearing(no_bearing);
   master_loop.set_test_envs(test_envs);
   master_loop.set_trial_num(trial_num);
   master_loop.set_env_generator(env_generator);

   cSimulator.Reset();
   cSimulator.Execute();

   return master_loop.get_fitness_score();

}
