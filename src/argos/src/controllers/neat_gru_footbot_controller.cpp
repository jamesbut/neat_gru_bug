#include "neat_gru_footbot_controller.h"

#define BEARING_SENSOR_ON false
#define PROX_SENSOR_ON false
#define LIN_VEL_ON true

/****************************************/
/****************************************/

NEATGRUFootbotController::NEATGRUFootbotController() :
      m_pcWheels(NULL),
      m_pcProximity(NULL),
      m_pcRangeBearing(NULL),
      stopWithoutSubscriberCount(10),
      stepsSinceCallback(0),
      leftSpeed(0),
      rightSpeed(0),
      prev_ang_vel(0),
      range_tminus1(0),
      range_tminus2(0),
      first_time_step(true),
      NET_INPUT_LOWER_BOUND(0.0),
      NET_INPUT_UPPER_BOUND(1.0),
      RANGE_SENSOR_LOWER_BOUND(0.0),
      RANGE_SENSOR_UPPER_BOUND(1350.0),
      NET_OUTPUT_LOWER_BOUND(0.0),
      NET_OUTPUT_UPPER_BOUND(1.0),
      MIN_LINEAR_VEL(0.0),
      MAX_LINEAR_VEL(10),
      MIN_ANGULAR_VEL(-50),
      MAX_ANGULAR_VEL(50),
      PROX_SENSOR_LOWER_BOUND(0.0),
      PROX_SENSOR_UPPER_BOUND(0.1),
      BEARING_SENSOR_LOWER_BOUND(-M_PI),
      BEARING_SENSOR_UPPER_BOUND(M_PI),
      MAX_WHEEL_SPEED(10),
      MIN_WHEEL_SPEED(-10)
{}

void NEATGRUFootbotController::Init(TConfigurationNode& t_node) {

  // Get sensor/actuator handles
  m_pcWheels = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
  m_pcProximity = GetSensor<CCI_FootBotProximitySensor>("footbot_proximity");
  m_pcRangeBearing = GetSensor<CCI_RangeAndBearingSensor>("range_and_bearing");

  /*
   * Parse the configuration file
   *
   * The user defines this part. Here, the algorithm accepts three
   * parameters and it's nice to put them in the config file so we don't
   * have to recompile if we want to try other settings.
   */
  GetNodeAttributeOrDefault(t_node, "stopWithoutSubscriberCount", stopWithoutSubscriberCount, stopWithoutSubscriberCount);

}
//int count = 0;
void NEATGRUFootbotController::ControlStep() {
   // std::cout << "---------------------------------" << std::endl;
   // std::cout << "Time Step: " << count << std::endl;
   // std::cout << "---------------------------------" << std::endl;
   //count++;

   /* Get readings from proximity sensor */
   const CCI_FootBotProximitySensor::TReadings& tProxReads = m_pcProximity->GetReadings();

   // Get readings from range and bearing sensor
   const CCI_RangeAndBearingSensor::TReadings& tRabReads = m_pcRangeBearing->GetReadings();

   //std::cout << net_inputs.size() << std::endl;
   //std::cout << tRabReads.size() + tProxReads.size() + 1 << std::endl;


   net_inputs[1] = mapValueIntoRange(tRabReads[0].Range,
                                      RANGE_SENSOR_LOWER_BOUND, RANGE_SENSOR_UPPER_BOUND,
                                      NET_INPUT_LOWER_BOUND, NET_INPUT_UPPER_BOUND);

   if(BEARING_SENSOR_ON) {
      //Bearing sensor 1-0.5-1

      //   net_inputs[(i*2)+2] = mapValueIntoRange(tRabReads[i].HorizontalBearing.GetValue(),
      //                                       BEARING_SENSOR_LOWER_BOUND, BEARING_SENSOR_UPPER_BOUND,
      //                                       NET_INPUT_LOWER_BOUND, NET_INPUT_UPPER_BOUND);

      //Bearing sensor 1-0-1

      //net_inputs[(i*2)+2] = mapHorizontalAngle(tRabReads[i].HorizontalBearing.GetValue());

      //Bearing sensor - left and right bearing sensors
      double bearing = tRabReads[0].HorizontalBearing.GetValue();
      double left_bearing_angle, right_bearing_angle;

      //Get the angular distances for each of the bearing sensors and normalise
      if(bearing < 0) {
         left_bearing_angle = mapValueIntoRange(fabs(bearing),
                                                0, 2*M_PI,
                                                NET_INPUT_LOWER_BOUND, NET_INPUT_UPPER_BOUND);
         right_bearing_angle = 1 - left_bearing_angle;
      } else {
         right_bearing_angle = mapValueIntoRange(bearing,
                                                0, 2*M_PI,
                                                NET_INPUT_LOWER_BOUND, NET_INPUT_UPPER_BOUND);
         left_bearing_angle = 1 - right_bearing_angle;
      }

      net_inputs[2] = right_bearing_angle;
      net_inputs[3] = left_bearing_angle;

   } else {

      //Take previous angular velocity as input
      // net_inputs[2] = prev_ang_vel;
      //
      // //If it is the first time step, set the previous ranges to the
      // //same as the first range rather than 0, otherwise the derivative
      // //makes it seem like the range has instantly changed.
      // if(first_time_step) {
      //
      //    range_tminus1 = net_inputs[1];
      //    range_tminus2 = net_inputs[1];
      //
      //    first_time_step = false;
      //
      // }
      //
      // //Take first derivative of range as input
      // net_inputs[3] = net_inputs[1] - range_tminus1;
      //
      // //Take second derivative of range as input
      // net_inputs[4] = (net_inputs[1] - range_tminus1) - (range_tminus1 - range_tminus2);

      // std::cout << "R: " << net_inputs[1] << std::endl;
      // std::cout << "Rt-1: " << range_tminus1 << std::endl;
      // std::cout << "Rt-2: " << range_tminus2 << std::endl;
      // std::cout << "R1deriv: " << net_inputs[1] - range_tminus1 << std::endl;
      // std::cout << "R2deriv: " << (net_inputs[1] - range_tminus1) - (range_tminus1 - range_tminus2) << std::endl;
      // std::cout << "----------------" << std::endl;

   }

   //Proximity sensor inputs
   if(PROX_SENSOR_ON) {

      int net_input_index;

      for(int i = 0; i < tProxReads.size(); i++) {

         if(BEARING_SENSOR_ON) net_input_index = i+4;
         else net_input_index = i+2;

         //Inverted laser
         double reading = tProxReads[i].Value;
         if(reading == 0) net_inputs[net_input_index] = 0;
         else net_inputs[net_input_index] = NET_INPUT_UPPER_BOUND - mapValueIntoRange(tProxReads[i].Value,
                                                                  PROX_SENSOR_LOWER_BOUND, PROX_SENSOR_UPPER_BOUND,
                                                                  NET_INPUT_LOWER_BOUND, NET_INPUT_UPPER_BOUND);

      }
   }

   //  std::cout << "----------" <<std::endl;
   //Net input testing
   // for(int i = 0; i < net_inputs.size(); i++) {
   //     std::cout << net_inputs[i] << std::endl;
   // }
   // std::cout << "----------" <<std::endl;

   m_net->load_sensors(net_inputs);

   //TODO: CHANGE THIS BACK!!
   //if (!(m_net->activate())) std::cout << "Inputs disconnected from output!";
   m_net->activate();

   //Get outputs

   if(LIN_VEL_ON) {

      //Linear velocity - mapped to a maximum speed
      //std::cout << m_net->outputs[0]->activation << std::endl;
      double mapped_lin_vel = mapValueIntoRange(m_net->outputs[0]->activation,
                                         NET_OUTPUT_LOWER_BOUND, NET_OUTPUT_UPPER_BOUND,
                                         MIN_LINEAR_VEL, MAX_LINEAR_VEL);
      //std::cout << mapped_lin_vel << std::endl;
      //mapped_lin_vel = 10;
      //Angular velocity - mapped to a maximum turning speed
      //std::cout << m_net->outputs[1]->activation << std::endl;
      double mapped_ang_vel = mapValueIntoRange(m_net->outputs[1]->activation,
                                         NET_OUTPUT_LOWER_BOUND, NET_OUTPUT_UPPER_BOUND,
                                         MIN_ANGULAR_VEL, MAX_ANGULAR_VEL);
      //std::cout << m_net->outputs[1]->activation << std::endl;
      //mapped_ang_vel = 100;
      //This function changes leftSpeed and rightSpeed
      ConvertLinVelToWheelSpeed(mapped_lin_vel, mapped_ang_vel);

      //Set currently angular vel as previous angular vel
      prev_ang_vel = m_net->outputs[1]->activation;
      //std:cout << "------------" << std::endl;
      //std::cout << mapped_ang_vel << std::endl;
      // std::cout << prev_ang_vel << std::endl;
      // std::cout << "--------------" << std::endl;

      //Set previous ranges
      range_tminus2 = range_tminus1;
      range_tminus1 = net_inputs[1];

   } else {

      leftSpeed = mapValueIntoRange(m_net->outputs[0]->activation,
                                          NET_OUTPUT_LOWER_BOUND, NET_OUTPUT_UPPER_BOUND,
                                          MIN_WHEEL_SPEED, MAX_WHEEL_SPEED);

      rightSpeed = mapValueIntoRange(m_net->outputs[1]->activation,
                                          NET_OUTPUT_LOWER_BOUND, NET_OUTPUT_UPPER_BOUND,
                                          MIN_WHEEL_SPEED, MAX_WHEEL_SPEED);

   }


   // std::vector<double> debug_vec;
   //
   // for(int i = 0; i < net_inputs.size(); i++) {
   //    debug_vec.push_back(net_inputs[i]);
   // }
   // debug_vec.push_back(m_net->outputs[0]->activation);
   // debug_vec.push_back(m_net->outputs[1]->activation);
   // debug_vec.push_back(leftSpeed);
   // debug_vec.push_back(rightSpeed);
   //
   // debug_data.push_back(debug_vec);

   //debug_data.push_back(net_inputs[1]);
   //debug_data.push_back(m_net->outputs[0]->activation);
   //debug_data.push_back(m_net->outputs[1]->activation);

   //debug_data.push_back(leftSpeed);
   //debug_data.push_back(rightSpeed);

   //leftSpeed = 10.0;
   //rightSpeed = 10.0;
   // std::cout << leftSpeed << " " << rightSpeed << std::endl;
   // std::cout << "------------" << std::endl;

   m_pcWheels->SetLinearVelocity(leftSpeed, rightSpeed);

}


double NEATGRUFootbotController::mapHorizontalAngle(double angle) {

   return abs(1/M_PI * angle);

}

void NEATGRUFootbotController::ConvertLinVelToWheelSpeed(Real linear_x, Real angular_z) {

  Real v = linear_x;// Forward speed
  Real w = angular_z; // Rotational speed

  // Use the kinematics of a differential-drive robot to derive the left
  // and right wheel speeds.
  leftSpeed = v - HALF_BASELINE * w;
  rightSpeed = v + HALF_BASELINE * w;

  //stepsSinceCallback = 0;
}

double NEATGRUFootbotController::mapValueIntoRange(const double input, const double input_start,
                                           const double input_end, const double output_start,
                                           const double output_end) {

   if(input > input_end) return 1.0;

   double slope = (output_end - output_start) / (input_end - input_start);
   double output = output_start + slope * (input - input_start);

   return output;

}

//int count = 0;

void NEATGRUFootbotController::Reset() {
   //count = 0;
   // delete neatOrg;

   //std::cout << "debug data size: " << debug_data.size() << std::endl;

   // if(debug_data.size() != 0) {
   //
   //    std::ofstream debug_file;
   //    std::stringstream file_name;
   //    file_name << "../debug/debug"<<count<<".txt";
   //    debug_file.open(file_name.str());
   //
   //    for(int i = 0; i < debug_data.size(); i++) {
   //       for(int j = 0; j < debug_data[i].size(); j++) {
   //             debug_file << debug_data[i][j] << ",";
   //       }
   //       debug_file << "\n";
   //    }
   //    std::cout << "Printed to: " << file_name.str() << std::endl;
   //    debug_file.close();
   //    count++;
   //    debug_data.clear();
   // }
   //debug_data.clear();

   net_inputs.resize(m_net->inputs.size());
   net_outputs.resize(m_net->outputs.size());

   net_inputs[0] = 1.0;                            //Bias node

   m_net->flush();

   //prev_ang_vel = 0;

}

void NEATGRUFootbotController::SetNEATNet(NEAT::Network &net) {

   m_net = &net;

}

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as
 * second argument.
 * The string is then usable in the configuration file to refer to this
 * controller.
 * When ARGoS reads that string in the configuration file, it knows which
 * controller class to instantiate.
 * See also the configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(NEATGRUFootbotController, "neat_gru_footbot_controller")
