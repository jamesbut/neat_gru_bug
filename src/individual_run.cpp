#include "individual_run.h"
#include "neat_ros/StartSim.h"

IndividualRun::IndividualRun(const std::string& gf) :
   genomeFile(gf), NUM_RUNS(209), num_finishes(0) {

   //Listens for finished_sim service
   spin_thread = boost::thread(&IndividualRun::finishedSimServiceThread, this);

}

IndividualRun::~IndividualRun() {

   spin_thread.join();

}

void IndividualRun::run() {

   // Wait for two seconds for the rest of the nodes and services to start
   ros::Duration(2).sleep();

   readGenomeFile();

   for(int i = 0; i < NUM_RUNS; i++) {

      evalIndividual(i+1);

   }

   std::cout << "Finishes: " << num_finishes << std::endl;
   std::cout << "Runs: " << NUM_RUNS << std::endl;

   num_finishes = 0;

}

void IndividualRun::readGenomeFile() {

   char curword[20];
   int id;

   std::string file_prefix = "ibug_working_directory/";
   std::string genomeFilePath = file_prefix + genomeFile;

   std::ifstream iFile(genomeFilePath.c_str());

   std::cout << "Reading in the individual" << std::endl;

   iFile >> curword;
   iFile >> id;

   std::cout << curword << std::endl;
   std::cout << id << std::endl;

   genom = new NEAT::Genome(id,iFile);
   iFile.close();

   org = new NEAT::Organism(0.0, genom, 1);

}

void IndividualRun::evalIndividual(int run_num) {

   org->gnome->print_to_filename(TEMP_GNOME_FILENAME.c_str());
   //std::cout << "Printed temp gnome file.." << std::endl;

   ros::NodeHandle n;
   ros::ServiceClient client = n.serviceClient<neat_ros::StartSim>("/start_sim");
   neat_ros::StartSim start_srv;

   start_srv.request.regenerate_env = 4;
   start_srv.request.select_env = run_num;
   start_srv.request.trial_num = 1;

   bool individual_run = true;
   start_srv.request.indv_run = individual_run;

   if (!client.call(start_srv)) {
     ROS_ERROR("Failed to call service to start simulator");
     exit(0);
   }

   sim_running = true;

   ros::Rate loop_rate(100);

   while(sim_running && ros::ok()) {

      loop_rate.sleep();

   }

}

bool IndividualRun::finishedSimCallback(neat_ros::FinishedSim::Request  &req,
                                        neat_ros::FinishedSim::Response &res) {

   sim_running = false;

}

bool IndividualRun::stop_run(std_srvs::Empty::Request  &req,
                             std_srvs::Empty::Request &res)
{
   //std::cout << "STOP RUN WAS CALLED!" << std::endl;
   num_finishes += 1;

}

//Spawns seperate thread to check for the finished simulation
void IndividualRun::finishedSimServiceThread() {

   //Handle for the node
   ros::NodeHandle n;
   ros::ServiceServer finish_sim_service = n.advertiseService("finished_sim", &IndividualRun::finishedSimCallback, this);
   ros::ServiceServer service3 = n.advertiseService("stop_run", &IndividualRun::stop_run, this);
   ros::spin();

}
