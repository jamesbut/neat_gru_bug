#ifndef NEAT_GRU_FOOTBOT_H
#define NEAT_GRU_FOOTBOT_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>

#include <NEAT_GRU/include/organism.h>

using namespace argos;

#define NUMOFBOTS 2

class NEATGRUFootbotController : public CCI_Controller {

public:

  NEATGRUFootbotController();
  virtual ~NEATGRUFootbotController() {}

  /*
   * This function initializes the controller.
   * The 't_node' variable points to the <parameters> section in the XML
   * file in the <controllers><footbot_ccw_wander_controller> section.
   */
  virtual void Init(TConfigurationNode& t_node);

  /*
   * This function is called once every time step.
   * The length of the time step is set in the XML file.
   */
  virtual void ControlStep();

  /*
   * This function resets the controller to its state right after the
   * Init().
   * It is called when you press the reset button in the GUI.
   * In this example controller there is no need for resetting anything,
   * so the function could have been omitted. It's here just for
   * completeness.
   */
  virtual void Reset();

  /*
   * Called to cleanup what done by Init() when the experiment finishes.
   * In this example controller there is no need for clean anything up,
   * so the function could have been omitted. It's here just for
   * completeness.
   */
  virtual void Destroy() {};

  void SetNEATNet(NEAT::Network &net);

private:

   double mapValueIntoRange(const double input, const double input_start,
                            const double input_end, const double output_start,
                            const double output_end);

   double mapHorizontalAngle(double angle);

   void ConvertLinVelToWheelSpeed(Real linear_x, Real angular_z);

   const double NET_INPUT_LOWER_BOUND;
   const double NET_INPUT_UPPER_BOUND;
   const double RANGE_SENSOR_LOWER_BOUND;
   const double RANGE_SENSOR_UPPER_BOUND;
   const double PROX_SENSOR_LOWER_BOUND;
   const double PROX_SENSOR_UPPER_BOUND;
   const double BEARING_SENSOR_LOWER_BOUND;
   const double BEARING_SENSOR_UPPER_BOUND;

   const double NET_OUTPUT_LOWER_BOUND;
   const double NET_OUTPUT_UPPER_BOUND;
   const double MIN_LINEAR_VEL;
   const double MAX_LINEAR_VEL;
   const double MIN_ANGULAR_VEL;
   const double MAX_ANGULAR_VEL;
   const double MAX_WHEEL_SPEED;
   const double MIN_WHEEL_SPEED;

  CCI_DifferentialSteeringActuator* m_pcWheels;
  CCI_FootBotProximitySensor* m_pcProximity;
  CCI_RangeAndBearingSensor* m_pcRangeBearing;

  // The following constant values were copied from the argos source tree from
  // the file src/plugins/robots/foot-bot/simulator/footbot_entity.cpp
  static constexpr Real HALF_BASELINE = 0.07f; // Half the distance between wheels
  //static constexpr Real WHEEL_RADIUS = 0.029112741f;

  // The number of time steps from the time step of the last callback
  // after which leftSpeed and rightSpeed will be set to zero.  Useful to
  // shutdown the robot after the controlling code on the ROS side has quit.
  int stopWithoutSubscriberCount;

  // The number of time steps since the last callback.
  int stepsSinceCallback;

  // Most recent left and right wheel speeds, converted from the ROS twist
  // message.
  Real leftSpeed, rightSpeed;

  //NEAT::Organism* neatOrg;
  NEAT::Network* m_net;

  std::vector<float> net_inputs;
  std::vector<double> net_outputs;

  int num_inputs;

  //Info for the network
  double prev_ang_vel;
  double range_tminus1;
  double range_tminus2;

  bool first_time_step;

};

#endif
