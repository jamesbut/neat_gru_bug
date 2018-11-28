#include "nash_averaging.h"

#include <iostream>
#include <fstream>
#include <iterator>

//Gurobi
//#include <gurobi_c++.h>

//C/Python API
#include <python2.7/Python.h>

NashAverager::NashAverager() :
   //GAME_FILE_PATH("/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/lib/bimatrix_solver/tmp/tmp_game.txt"),
   //BIMATRIX_LIB_PATH("/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/lib/bimatrix_solver_mod"),
   BIMATRIX_LIB_PATH("../lib/bimatrix_solver_mod"),
   //NASH_FILE_PATH("/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/lib/bimatrix_solver_mod/tmp/nash_out_") {}
   NASH_FILE_PATH("../lib/bimatrix_solver_mod/tmp/nash_out_") {}

std::vector<double> NashAverager::calculate_agent_skills(const std::vector<std::vector<double> >& vec_scores,
                                                         const unsigned int num_agents,
                                                         const unsigned int num_envs) {

   //Delete previous results
   std::string command = "exec rm " + BIMATRIX_LIB_PATH + "/tmp/*";
   system(command.c_str());

   //Calculate S
   Eigen::MatrixXd S = calculate_S(vec_scores);

   // std::cout << S << std::endl;
   //
   // std::cout << "-----------------" << std::endl;
   //
   // std::cout << -S.transpose() << std::endl;

   //Calculate A
   Eigen::MatrixXd A = calculate_A(S);

   //std::cout << A << std::endl;

   //Find max entropy Nash Equilibrium for A
   std::vector<Eigen::VectorXd> nash_eq = calculate_maxent_nash_bimatrix_solver(S);

   /* Find skills of agents */

   // std::cout << "Nash:" << std::endl;
   // std::cout << nash_eq[0] << std::endl;

   Eigen::VectorXd agent_skills = S * nash_eq[0];
   //
   // std::cout << "Agent skills: " << std::endl;
   // std::cout << agent_skills << std::endl;

   //Eigen::VectorXd agent_skills_norm = normalise(agent_skills);

   // std::cout << "Agent skills norm: " << std::endl;
   // std::cout << agent_skills_norm << std::endl;

   //Return
   //std::vector<double> agent_skills_vec(agent_skills_norm.size());
   std::vector<double> agent_skills_vec(agent_skills.size());

   for(unsigned int i = 0; i < agent_skills_vec.size(); i++)
      //agent_skills_vec.at(i) = agent_skills_norm(i);
      agent_skills_vec.at(i) = agent_skills(i);

   return agent_skills_vec;

}

Eigen::MatrixXd NashAverager::calculate_S(const std::vector<std::vector<double> >& vec_scores) {

   //Convert scores into eigen matrix
   Eigen::MatrixXd eigen_scores(vec_scores.size(), vec_scores[0].size());

   for(unsigned int i = 0; i < vec_scores.size(); i++)
      eigen_scores.row(i) = Eigen::VectorXd::Map(&vec_scores[i][0], vec_scores[i].size());

   //std::cout << eigen_scores << std::endl;

   //Calculate S by minusing the mean of all scores from each element
   Eigen::MatrixXd S(eigen_scores.rows(), eigen_scores.cols());

   double sum_eigen_scores = 0;

   for(unsigned int i = 0; i < eigen_scores.rows(); i++)
      for(unsigned int j = 0; j < eigen_scores.cols(); j++)
         sum_eigen_scores += eigen_scores(i, j);

   double mean_eigen_scores = sum_eigen_scores / (double)(eigen_scores.cols() * eigen_scores.rows());

   //Create matrix of means
   Eigen::MatrixXd mean_matrix(S.rows(), S.cols());
   mean_matrix.fill(mean_eigen_scores);

   //std::cout << mean_matrix << std::endl;

   //Subtract means from scores
   // std::cout << eigen_scores << std::endl;
   // std::cout << "---------" << std::endl;
   // std::cout << -eigen_scores.transpose() << std::endl;
   // std::cout << "---------" << std::endl;

   //Take the mean away from S
   //S = eigen_scores - mean_matrix;
   S = eigen_scores;

   return S;

}

Eigen::MatrixXd NashAverager::calculate_A(const Eigen::MatrixXd& S) {

   Eigen::MatrixXd A((S.rows() + S.cols()), (S.rows() + S.cols()));

   Eigen::MatrixXd zeros_m = Eigen::MatrixXd::Zero(S.rows(), S.rows());
   Eigen::MatrixXd zeros_n = Eigen::MatrixXd::Zero(S.cols(), S.cols());

   Eigen::MatrixXd minusSt = S.transpose() * -1;

   //Build A
   A.topLeftCorner(zeros_m.rows(), zeros_m.cols()) = zeros_m;
   A.bottomRightCorner(zeros_n.rows(), zeros_n.cols()) = zeros_n;
   A.topRightCorner(S.rows(), S.cols()) = S;
   A.bottomLeftCorner(minusSt.rows(), minusSt.cols()) = minusSt;

   return A;

}

std::vector<Eigen::VectorXd> NashAverager::calculate_maxent_nash_bimatrix_solver(const Eigen::MatrixXd& S) {

   //These both seem to get the same nashes
   //I might just call one of these in future
   // call_bimatrix_solver(S);
   // std::vector<Eigen::MatrixXd> nash_S = read_nash_from_file();

   call_bimatrix_solver(-S.transpose());
   std::vector<Eigen::MatrixXd> nash_S_transpose = read_nash_from_file();

   // std::cout << "Nashes g1:" << std::endl;
   // std::cout << nash_S[0] << std::endl;
   // std::cout << "---------" << std::endl;
   // std::cout << nash_S[1] << std::endl;
   //
   // std::cout << "Nashes g2:" << std::endl;
   // std::cout << "---------" << std::endl;
   // std::cout << nash_S_transpose[0] << std::endl;
   // std::cout << "---------" << std::endl;
   // std::cout << nash_S_transpose[1] << std::endl;

   //Calculate the nash equilibrium with highest entropy
   std::vector<Eigen::VectorXd> max_ent_nash = calculate_maxent_nash(nash_S_transpose);

   //std::cout << S << std::endl;
   //std::cout << -S.transpose() << std::endl;
   //std::cout << max_ent_nash[0] << std::endl;

   return max_ent_nash;

}

void NashAverager::call_bimatrix_solver(const Eigen::MatrixXd& game) {

   //Write game to file
   write_game_to_file(game);

   //Initialise python
   Py_Initialize();
   //PyRun_SimpleString("import sys; sys.path.insert(0, '/home/james/Documents/PhD/researchPrograms/ARGoS/neat_gru_bug/lib/bimatrix_solver_mod')");
   PyRun_SimpleString("import sys; sys.path.insert(0, '../lib/bimatrix_solver_mod')");

   //Get python file
   PyObject* myModuleString = PyString_FromString((char*)"solve_game");
   PyObject* myModule = PyImport_Import(myModuleString);

   if(myModule == NULL) {
      std::cout << "Could not import bimatrix_solver module!" << std::endl;
      PyErr_Print();
      exit(EXIT_FAILURE);
   }

   //Get reference to function
   PyObject* myFunction = PyObject_GetAttrString(myModule, (char*)"solve_game");

   if(myFunction == NULL) {
      std::cout << "Could not find bimatrix_solver function" << std::endl;
      exit(EXIT_FAILURE);
   }

   //Build args
   PyObject* args = PyTuple_New(1);
   PyObject* string_arg = PyString_FromString(BIMATRIX_LIB_PATH.c_str());

   PyTuple_SetItem(args, 0, string_arg);

   //Solve game
   PyObject* myResult = PyObject_CallObject(myFunction, args);

}

//This method prints the games to file and calls the solvers
// std::vector<Eigen::VectorXd> NashAverager::calculate_maxent_nash_bimatrix_solver(const Eigen::MatrixXd& A) {
//
//    //Write game to file
//    write_game_to_file(A);
//
//    //Initialise python
//    Py_Initialize();
//    PyRun_SimpleString("import sys; sys.path.insert(0, '/home/james/Documents/PhD/researchPrograms/bimatrix_solver')");
//
//    //Get python file
//    PyObject* myModuleString = PyString_FromString((char*)"solve_game");
//    PyObject* myModule = PyImport_Import(myModuleString);
//
//    if(myModule == NULL) {
//       std::cout << "Could not import bimatrix_solver module!" << std::endl;
//       exit(EXIT_FAILURE);
//    }
//
//    //Get reference to function
//    PyObject* myFunction = PyObject_GetAttrString(myModule, (char*)"solve_game");
//
//    if(myFunction == NULL) {
//       std::cout << "Could not find bimatrix_solver function" << std::endl;
//       exit(EXIT_FAILURE);
//    }
//
//    //Build args
//    PyObject* args = PyTuple_New(1);
//    PyObject* string_arg = PyString_FromString(GAME_FILE_PATH.c_str());
//
//    PyTuple_SetItem(args, 0, string_arg);
//
//    //Solve game
//    PyObject* myResult = PyObject_CallObject(myFunction, args);
//
//    //Read nash results from file
//    //Vector of size 2 - one for each player
//    std::vector<Eigen::MatrixXd> all_nash_eq = read_nash_from_file();
//
//    //std::cout << all_nash_eq[0] << std::endl;
//    //std::cout << all_nash_eq[1] << std::endl;
//
//    //Calculate the nash equilibrium with highest entropy
//    std::vector<Eigen::VectorXd> max_ent_nash = calculate_maxent_nash(all_nash_eq);
//
//    //std::cout << max_ent_nash[0] << std::endl;
//
//    return max_ent_nash;
//
// }

std::vector<Eigen::VectorXd> NashAverager::calculate_maxent_nash(const std::vector<Eigen::MatrixXd>& nash_eqs) {

   double max_entropy = 0.0;
   int max_entropy_index = 0;

   for(unsigned int i = 0; i < nash_eqs[0].rows(); i++) {

      double entropy_sum = calculate_entropy(nash_eqs[0].row(i))
                         + calculate_entropy(nash_eqs[1].row(i));

      //std::cout << "Entropy " << i << ": " << entropy_sum << std::endl;

      if(entropy_sum > max_entropy) {

         max_entropy = entropy_sum;
         max_entropy_index = i;

      }

   }

   std::vector<Eigen::VectorXd> max_ent_nashes(2);

   max_ent_nashes[0] = nash_eqs[0].row(max_entropy_index);
   max_ent_nashes[1] = nash_eqs[1].row(max_entropy_index);

   return max_ent_nashes;

}

double NashAverager::calculate_entropy(const Eigen::VectorXd& prob_dist) {

   double entropy = 0.0;

   for(unsigned int i = 0; i < prob_dist.size(); i++) {

      //Check for 0.0
      if(prob_dist[i] < std::numeric_limits<double>::epsilon())
         continue;
      else
         entropy += prob_dist[i] * (log2(1 / prob_dist[i]));
   }
   //std::cout << entropy << std::endl;
   return entropy;

}

void NashAverager::write_game_to_file(const Eigen::MatrixXd& A) {

   //Write game to tmp_file for solvers to read
   std::ofstream game_file;
   game_file.open(BIMATRIX_LIB_PATH + "/tmp/tmp_game.txt");

   game_file << A.rows() << " " << A.cols() << "\n\n";

   //Write p1 game
   //Convert from double to rational of form x/y
   for(unsigned int i = 0; i < A.rows(); i++) {

      for(unsigned int j = 0; j < A.cols(); j++)
         game_file << A(i,j) << " ";
         //game_file << A(i,j) * 1e5 << "/" << 1e5 << " ";

      game_file << "\n";

   }

   game_file << "\n";

   //Write p2 game
   for(unsigned int i = 0; i < A.rows(); i++) {

      for(unsigned int j = 0; j < A.cols(); j++)
         game_file << -A(i,j) << " ";
         //game_file << -A(i,j) * 1e5 << "/" << 1e5 << " ";

      game_file << "\n";

   }

   game_file.close();

}

std::vector<Eigen::MatrixXd> NashAverager::read_nash_from_file() {

   std::vector<std::vector<double> > nash_eqs_1;
   std::vector<std::vector<double> > nash_eqs_2;

   std::string line;
   std::ifstream file_1(NASH_FILE_PATH + "1");

   while(getline(file_1, line)) {

      std::istringstream ss(line);
      nash_eqs_1.push_back(std::vector<double>(std::istream_iterator<double>(ss), std::istream_iterator<double>()));

   }

   std::ifstream file_2(NASH_FILE_PATH + "2");

   while(getline(file_2, line)) {

      std::istringstream ss(line);
      nash_eqs_2.push_back(std::vector<double>(std::istream_iterator<double>(ss), std::istream_iterator<double>()));

   }

   // for(size_t i = 0; i < nash_eqs_1.size(); i++) {
   //    for(size_t j = 0; j < nash_eqs_1[i].size(); j++)
   //       std::cout << nash_eqs_1[i][j] << " ";
   //    std::cout << std::endl;
   // }

   //Convert to 2 EigenMatrices
   std::vector<Eigen::MatrixXd> nash_pair(2);

   nash_pair[0] = Eigen::MatrixXd(nash_eqs_1.size(), nash_eqs_1[0].size());
   nash_pair[1] = Eigen::MatrixXd(nash_eqs_2.size(), nash_eqs_2[0].size());

   for(size_t i = 0; i < nash_eqs_1.size(); i++) {
      nash_pair[0].row(i) = Eigen::VectorXd::Map(&nash_eqs_1[i][0], nash_eqs_1[i].size());
   }

   for(size_t i = 0; i < nash_eqs_2.size(); i++) {
      nash_pair[1].row(i) = Eigen::VectorXd::Map(&nash_eqs_2[i][0], nash_eqs_2[i].size());
   }

   return nash_pair;

}

//Normalise a vector such that all the values are between 0 and 1
Eigen::VectorXd NashAverager::normalise(Eigen::VectorXd vec) {

   //Find smallest value
   double min;
   double max;

   for(unsigned int i = 0; i < vec.size(); i++) {

      if(i == 0) {
         min = vec(i);
         max = vec(i);
         continue;
      }

      if(vec(i) < min)
         min = vec(i);

      if(vec(i) > max)
         max = vec(i);

   }

   double range = max - min;

   //Construct normalised vector
   Eigen::VectorXd normalised_vec(vec.size());

   for(unsigned int i = 0; i < vec.size(); i++)
      normalised_vec(i) = (vec(i) - min) / range;

   return normalised_vec;

}

//This is not finished - it only seemed to find one Nash but I needed
//them all
// Eigen::MatrixXd NashAverager::calculate_nash_gurobi(const Eigen::MatrixXd& S) {
//
//    //Example game - Would have to change to S throughout to get it working
//    //in general
//    Eigen::MatrixXd M(3,2);
//    M << 1, -1,
//         -1, 1,
//         -1, 1;
//
//    try {
//
//       GRBEnv env = GRBEnv();
//
//       /********Row Player************/
//
//       //Construct linear program for row player
//       GRBModel row_model = GRBModel(env);
//
//       //Add variables
//       //TODO: lower and upper bounds need to be looked at here
//       GRBVar z = row_model.addVar(-100.0, 100.0, 0.0, GRB_CONTINUOUS, "z");
//
//       //Add probability variables
//       std::vector<GRBVar> row_probabilities(M.rows());
//
//       for(unsigned int i = 0; i < M.rows(); i++)
//          row_probabilities.at(i) = row_model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "x_" + std::to_string(i));
//
//       //Add objective
//       row_model.setObjective(GRBLinExpr(z), GRB_MAXIMIZE);
//
//       //Add constraints
//       for(unsigned int i = 0; i < M.cols(); i++) {
//
//          GRBLinExpr lin_exp;
//
//          for(unsigned int j = 0; j < M.rows(); j++)
//             lin_exp += row_probabilities.at(j) * M(j, i) * (-1);
//
//          row_model.addConstr(lin_exp, GRB_GREATER_EQUAL, z);
//
//       }
//
//
//       GRBLinExpr sum_to_one_row;
//       for(unsigned int i = 0; i < M.rows(); i++)
//          sum_to_one_row += row_probabilities.at(i);
//
//       row_model.addConstr(sum_to_one_row, GRB_EQUAL, 1);
//
//       for(unsigned int i = 0; i < M.rows(); i++)
//          row_model.addConstr(row_probabilities.at(i) >= 0);
//
//       //Optimise
//       row_model.optimize();
//
//       for(size_t i = 0; i < row_probabilities.size(); i++) {
//          std::cout << row_probabilities.at(i).get(GRB_StringAttr_VarName) << " "
//          << row_probabilities.at(i).get(GRB_DoubleAttr_X) << std::endl;
//       }
//
//       std::cout << "Obj: " << row_model.get(GRB_DoubleAttr_ObjVal) << std::endl;
//
//       /********Column Player************/
//
//       //Construct linear program for row player
//       GRBModel column_model = GRBModel(env);
//
//       //Add variables
//       //TODO: lower and upper bounds need to be looked at here
//       GRBVar w = column_model.addVar(-100.0, 100.0, 0.0, GRB_CONTINUOUS, "w");
//
//       //Add probability variables
//       std::vector<GRBVar> column_probabilities(M.cols());
//
//       for(unsigned int i = 0; i < M.cols(); i++)
//          column_probabilities.at(i) = column_model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "y_" + std::to_string(i));
//
//       //Add objective
//       column_model.setObjective(GRBLinExpr(w), GRB_MAXIMIZE);
//
//       //Add constraints
//       for(unsigned int i = 0; i < M.rows(); i++) {
//
//          GRBLinExpr lin_exp;
//
//          for(unsigned int j = 0; j < M.cols(); j++)
//             lin_exp += column_probabilities.at(j) * M(i, j);
//
//          column_model.addConstr(lin_exp, GRB_GREATER_EQUAL, w);
//
//       }
//
//
//       GRBLinExpr sum_to_one_col;
//       for(unsigned int i = 0; i < M.cols(); i++)
//          sum_to_one_col += column_probabilities.at(i);
//
//       column_model.addConstr(sum_to_one_col, GRB_EQUAL, 1);
//
//       for(unsigned int i = 0; i < M.cols(); i++)
//          column_model.addConstr(column_probabilities.at(i) >= 0);
//
//       //Optimise
//       column_model.optimize();
//
//       for(size_t i = 0; i < column_probabilities.size(); i++) {
//          std::cout << column_probabilities.at(i).get(GRB_StringAttr_VarName) << " "
//          << column_probabilities.at(i).get(GRB_DoubleAttr_X) << std::endl;
//       }
//
//       std::cout << "Obj: " << column_model.get(GRB_DoubleAttr_ObjVal) << std::endl;
//
//
//    } catch(GRBException e) {
//
//       std::cout << "Error code for Gurobi = " << e.getErrorCode() << std::endl;
//       std::cout << e.getMessage() << std::endl;
//
//    } catch(...) {
//
//       std::cout << "Exception with Gurobi!" << std::endl;
//
//    }
//
//    Eigen::MatrixXd agent_skills(2, 2);
//    return agent_skills;
//
// }

//This method calls the bimatrix solver which is written in python
//It does so by passing the game as an argument
//On reflection, it seems it will just be easier to print the
//game to file and allow the solvers to read it from there
Eigen::MatrixXd NashAverager::calculate_nash_bimatrix_solver_old(const Eigen::MatrixXd& A) {

   //Initialise python interpreter
   Py_Initialize();
   PyRun_SimpleString ("import sys; sys.path.insert(0, '/home/james/Documents/PhD/researchPrograms/bimatrix_solver')");

   //Get python file
   PyObject* myModuleString = PyString_FromString((char*)"solve_game");
   PyObject* myModule = PyImport_Import(myModuleString);

   if(myModule == NULL) {
      std::cout << "Could not import bimatrix_solver module!" << std::endl;
      exit(EXIT_FAILURE);
   }

   //Get reference to function
   PyObject* myFunction = PyObject_GetAttrString(myModule, (char*)"solve_game");

   if(myFunction == NULL) {
      std::cout << "Could not find bimatrix_solver function" << std::endl;
      exit(EXIT_FAILURE);
   }

   //Create arguments for python function
   PyObject* matrix_rows = PyInt_FromLong(A.rows());
   PyObject* matrix_cols = PyInt_FromLong(A.cols());

   //Build args
   PyObject* args_tuple = PyTuple_New(3);

   PyTuple_SetItem(args_tuple, 0, matrix_rows);
   PyTuple_SetItem(args_tuple, 1, matrix_cols);

   PyObject* A_tuple = PyTuple_New(A.rows() * A.cols());

   //Convert A into python tuple
   for(unsigned int i = 0; i < A.rows(); i++)
      for(unsigned int j = 0; j < A.cols(); j++)
         PyTuple_SetItem(A_tuple, (i * A.cols() + j), PyFloat_FromDouble(A(i, j)));

   //Add A tuple to function args
   PyTuple_SetItem(args_tuple, 2, A_tuple);

   //Solve game
   PyObject* myResult = PyObject_CallObject(myFunction, args_tuple);

   if(myResult == NULL) {
      std::cout << "No results were returned from python code" << std::endl;
      exit(EXIT_FAILURE);
   }

   std::cout << "Result: " << PyFloat_AsDouble(myResult) << std::endl;

   Eigen::MatrixXd agent_skills(2, 2);
   return agent_skills;

}
