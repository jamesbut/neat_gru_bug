#ifndef _NASH_AVERAGER_H_
#define _NASH_AVERAGER_H_

#include <vector>
#include <eigen3/Eigen/Dense>

class NashAverager {

public:

   NashAverager();

   std::vector<double> calculate_agent_skills(const std::vector<std::vector<double> >& vec_scores,
                                              const unsigned int num_agents,
                                              const unsigned int num_envs);

private:

   Eigen::MatrixXd calculate_S(const std::vector<std::vector<double> >& vec_scores);
   Eigen::MatrixXd calculate_A(const Eigen::MatrixXd& S);

   Eigen::MatrixXd calculate_nash_gurobi(const Eigen::MatrixXd& S);
   Eigen::MatrixXd calculate_nash_bimatrix_solver_old(const Eigen::MatrixXd& A);
   std::vector<Eigen::VectorXd> calculate_maxent_nash_bimatrix_solver(const Eigen::MatrixXd& A);

   std::vector<Eigen::VectorXd> calculate_maxent_nash(const std::vector<Eigen::MatrixXd>& nash_eqs);

   void call_bimatrix_solver(const Eigen::MatrixXd& S);

   double calculate_entropy(const Eigen::VectorXd& prob_dist);

   void write_game_to_file(const Eigen::MatrixXd& A);

   //Returns two nash matrices - one for each player
   std::vector<Eigen::MatrixXd> read_nash_from_file();

   //const std::string BIMATRIX_LIB_PATH;
   const std::string GAME_FILE_PATH;
   const std::string NASH_FILE_PATH;

};

#endif
