#include "nnode_gru.h"
#include <iostream>
#include <sstream>
#include <math.h>
#include <iomanip>

using namespace NEAT;

NNodeGRU::NNodeGRU(const char *argline, std::vector<Trait*> &traits) : NNode(GRU, -1) {

	//std::cout << "GRU Node created" << std::endl;

	std::stringstream ss(argline);
	int nodepl;

	ss >> node_id >> nodepl >> b >> b_u >> b_r >> w >> w_r >> w_u;
	gen_node_label = (nodeplace)nodepl;
	//std::cout << "NODE ID:" << node_id << std::endl;

	//Test print outs
	//std::cout << node_id << " " << b << " " << b_u << " " << b_r << " " << w << " " << w_r << " " << w_u << std::endl;

	//This is fine because this is just the number size
	char curword[128];  //max word size of 128 characters

	//Get U vector from file
	ss >> curword >> curword;
	while (strcmp(curword,"]")!=0) {

		U.push_back(atof(curword));
		ss >> curword;

	}

	//Get U_r vector from file
	ss >> curword >> curword;
	while (strcmp(curword,"]")!=0) {

		U_r.push_back(atof(curword));
		ss >> curword;

	}

	//Get U_u vector from file
	ss >> curword >> curword;
	while (strcmp(curword,"]")!=0) {

		U_u.push_back(atof(curword));
		ss >> curword;

	}

	create_weight_vector();

	//Test print outs
	// for(int i = 0; i < U.size(); i++) {
	// 	std::cout << U[i] << " ";
	// }
	// std::cout << std::endl;
	//
	// for(int i = 0; i < U_r.size(); i++) {
	// 	std::cout << U_r[i] << " ";
	// }
	// std::cout << std::endl;
	//
	// for(int i = 0; i < U_u.size(); i++) {
	// 	std::cout << U_u[i] << " ";
	// }
	// std::cout << std::endl;

}

//Copy a GRU node
NNodeGRU::NNodeGRU(int nodeid, NNodeGRU* node) : NNode(GRU, nodeid) {

	gen_node_label=node->gen_node_label;

	b=node->b;
	b_u=node->b_u;
	b_r=node->b_r;
	w=node->w;
	w_r=node->w_r;
	w_u=node->w_u;
	U=node->U;
	U_r=node->U_r;
	U_u=node->U_u;

	active_inputs=node->active_inputs;

	create_weight_vector();

};

//Default constructor for new GRU node
NNodeGRU::NNodeGRU(int nodeid) : NNode(GRU, nodeid) {
	//std::cout << "GRU Node created" << std::endl;
	//TODO: We want these values such that the node just lets through
	//the same info at first
	b=0;
	b_u=0;
	b_r=0;
	w=0;
	w_r=0;
	w_u=0;

	U=std::vector<double>(1, 0.0);
	U_r=std::vector<double>(1, 0.0);
	U_u=std::vector<double>(1, 0.0);

	active_inputs=std::vector<bool>(1, true);

	create_weight_vector();

}

void NNodeGRU::activate_gru(std::vector<double> inputs) {

	//Check number of inputs is correct
	int num_of_active_inputs = 0;

	for(int i = 0; i < active_inputs.size(); i++)
		num_of_active_inputs += (int)active_inputs[i];

	if(num_of_active_inputs != inputs.size()) {

		std::cout << "Inputs not the same size as num of active inputs!!" << std::endl;
		std::cout << "Inputs size: " << inputs.size() << std::endl;
		std::cout << "Num of active inputs: " << num_of_active_inputs << std::endl;
		std::exit(0);

	}

	//std::cout << "Activation before: " << activation << std::endl;

	//std::cout << inputs[0] << std::endl;

	/* Reset Gate */
	double input_sum_r = 0;
	for(int i = 0; i < inputs.size(); i++) {

		if(active_inputs[i])
			input_sum_r += inputs[i] * U_r[i];

	}

	//std::cout << "Inputs sum r: " << input_sum_r << std::endl;

	//std::cout << "Activation: " << activation << std::endl;
	//std::cout << "w_r: " << w_r << std::endl;

	// //Recurrent input
	// //TODO: Check activation here is definitely what I want
	input_sum_r += activation * w_r;

	//std::cout << "b_r: " << b_r << std::endl;

	//Bias input
	input_sum_r += 1.0 * b_r;

	//std::cout << "Reset gate sum: " << input_sum_r << std::endl;

	//Reset gate output
	//Might be interesting to play with the sigmoid parameters
	double reset_output = NEAT::fsigmoid(input_sum_r,4.924273,2.4621365);

	//std::cout << "Reset gate output: " << reset_output << std::endl;

	double reset_mult = reset_output * w * activation;

	/* tanh gate */
	double tanh_input_sum = 0;
	//std::cout << "Inputs and U size: " << inputs.size() << " " << U.size() << std::endl;
	for(int i = 0; i < inputs.size(); i++) {
		//std::cout << inputs[i] << " " << U[i] << std::endl;
		if(active_inputs[i])
			tanh_input_sum += inputs[i] * U[i];

	}

	tanh_input_sum = tanh_input_sum + reset_mult + (1.0 * b);
	//std::cout << tanh_input_sum << " " << reset_mult << " " << (1.0 * b) << std::endl;
	//std::cout << "Tanh input sum: " << tanh_input_sum << std::endl;

	double h_tilda = tanh(tanh_input_sum);

	//std::cout << "Tanh gate output: " << h_tilda << std::endl;

	/* Update gate */
	double input_sum_u = 0;
	for(int i = 0; i < inputs.size(); i++) {

		if(active_inputs[i])
			input_sum_u += inputs[i] * U_u[i];

	}

	//Recurrent input
	//TODO: Check activation here is definitely what I want
	input_sum_u += activation * w_u;

	//Bias input
	input_sum_u += 1.0 * b_u;

	//std::cout << "Update gate sum: " << input_sum_u << std::endl;

	//Reset gate output
	//Sigmoid seems really narrow here, maybe mess with parameters
	double update_output = NEAT::fsigmoid(input_sum_u,4.924273,2.4621365);

	//std::cout << "Update gate output: " << update_output << std::endl;

	/* Final computation steps */
	double h_tilda_mult = h_tilda * (1 - update_output);

	//std::cout << "h_tilda_mult: " << h_tilda_mult << std::endl;

	double update_output_w_history = update_output * activation;

	//std::cout << "update_output_w_history: " << update_output_w_history << std::endl;

	activation = h_tilda_mult + update_output_w_history;

	//std::cout << "activation: " << activation << std::endl;

}

void NNodeGRU::activate_gru_old(std::vector<double> inputs) {
	//std::cout << "Inputs size: " << inputs.size() << std::endl;
	if(U.size() != inputs.size()) {

		std::cout << "Inputs not the same size as U!!" << std::endl;
		std::cout << "Inputs size: " << inputs.size() << std::endl;
		std::cout << "U size: " << U.size() << std::endl;
		std::exit(0);

	}

	//std::cout << "Activation before: " << activation << std::endl;

	//std::cout << inputs[0] << std::endl;

	/* Reset Gate */
	double input_sum_r = 0;
	for(int i = 0; i < inputs.size(); i++) {

		input_sum_r += inputs[i] * U_r[i];

	}

	//std::cout << "Inputs sum r: " << input_sum_r << std::endl;

	//std::cout << "Activation: " << activation << std::endl;
	//std::cout << "w_r: " << w_r << std::endl;

	// //Recurrent input
	// //TODO: Check activation here is definitely what I want
	input_sum_r += activation * w_r;

	//std::cout << "b_r: " << b_r << std::endl;

	//Bias input
	input_sum_r += 1.0 * b_r;

	//std::cout << "Reset gate sum: " << input_sum_r << std::endl;

	//Reset gate output
	//Might be interesting to play with the sigmoid parameters
	double reset_output = NEAT::fsigmoid(input_sum_r,4.924273,2.4621365);

	//std::cout << "Reset gate output: " << reset_output << std::endl;

	double reset_mult = reset_output * w * activation;

	/* tanh gate */
	double tanh_input_sum = 0;
	//std::cout << "Inputs and U size: " << inputs.size() << " " << U.size() << std::endl;
	for(int i = 0; i < inputs.size(); i++) {
		//std::cout << inputs[i] << " " << U[i] << std::endl;
		tanh_input_sum += inputs[i] * U[i];

	}

	tanh_input_sum = tanh_input_sum + reset_mult + (1.0 * b);
	//std::cout << tanh_input_sum << " " << reset_mult << " " << (1.0 * b) << std::endl;
	//std::cout << "Tanh input sum: " << tanh_input_sum << std::endl;

	double h_tilda = tanh(tanh_input_sum);

	//std::cout << "Tanh gate output: " << h_tilda << std::endl;

	/* Update gate */
	double input_sum_u = 0;
	for(int i = 0; i < inputs.size(); i++) {

		input_sum_u += inputs[i] * U_u[i];

	}

	//Recurrent input
	//TODO: Check activation here is definitely what I want
	input_sum_u += activation * w_u;

	//Bias input
	input_sum_u += 1.0 * b_u;

	//std::cout << "Update gate sum: " << input_sum_u << std::endl;

	//Reset gate output
	//Sigmoid seems really narrow here, maybe mess with parameters
	double update_output = NEAT::fsigmoid(input_sum_u,4.924273,2.4621365);

	//std::cout << "Update gate output: " << update_output << std::endl;

	/* Final computation steps */
	double h_tilda_mult = h_tilda * (1 - update_output);

	//std::cout << "h_tilda_mult: " << h_tilda_mult << std::endl;

	double update_output_w_history = update_output * activation;

	//std::cout << "update_output_w_history: " << update_output_w_history << std::endl;

	activation = h_tilda_mult + update_output_w_history;

	//std::cout << "activation: " << activation << std::endl;

}

//TODO: TEST THIS!!
double NNodeGRU::compatibility(NNodeGRU node) {

	double cumm_diff = 0;

	cumm_diff += fabs(b - node.b);
	cumm_diff += fabs(b_u - node.b_u);
	cumm_diff += fabs(b_r - node.b_r);
	cumm_diff += fabs(w - node.w);
	cumm_diff += fabs(w_u - node.w_u);
	cumm_diff += fabs(w_r - node.w_r);

	//Sometimes the vectors will be different sizes
	//You have to add zeros to the end of the smaller
	//vector

	NNodeGRU own_node_copy = NNodeGRU(1, this);

	if(U.size() > node.U.size()) {

		int diff = U.size() - node.U.size();
		for(int i = 0; i < diff; i++) {
			//std::cout << "Pushing1" << std::endl;
			node.U.push_back(0.0);
			node.U_u.push_back(0.0);
			node.U_r.push_back(0.0);

		}

	} else {

		int diff = node.U.size() - U.size();
		for(int i = 0; i < diff; i++) {
			//std::cout << "Pushing2" << std::endl;
			own_node_copy.U.push_back(0.0);
			own_node_copy.U_u.push_back(0.0);
			own_node_copy.U_r.push_back(0.0);

		}

	}

	//std::cout << own_node_copy.U.size() << " " << node.U.size() << std::endl;
	for(int i; i < node.U.size(); i++) {

		cumm_diff += fabs(own_node_copy.U[i] - node.U[i]);

	}

	//std::cout << U_u.size() << " " << node.U_u.size() << std::endl;
	for(int i = 0; i < node.U_u.size(); i++) {

		cumm_diff += fabs(own_node_copy.U_u[i] - node.U_u[i]);

	}

	//std::cout << U_r.size() << " " << node.U_r.size() << std::endl;
	for(int i = 0; i < node.U_r.size(); i++) {

		cumm_diff += fabs(own_node_copy.U_r[i] - node.U_r[i]);

	}

	return cumm_diff;

}

//TODO: TEST THIS!!
//Mutate link weights in GRU cell
void NNodeGRU::mutate(double power, double rate) {

	double randnum;
	double randchoice; //Decide what kind of mutation to do on a gene

	double gausspoint;
	double coldgausspoint;

	bool severe;  //Once in a while really shake things up

	//if (randfloat()>0.8) severe=true;
	if (randfloat()>0.9) severe=true;
	//if (randfloat()>0.96) severe=true;
	else severe=false;

	for(int i = 0; i < weight_vector.size(); i++) {

		//The following if determines the probabilities of doing cold gaussian
		//mutation, meaning the probability of replacing a link weight with
		//another, entirely random weight. The gausspoint and coldgausspoint
		//represent values above which a random float will signify that kind
		//of mutation.

		// if (severe) {
		// 	gausspoint=0.3;
		// 	coldgausspoint=0.1;
		// } else {
		// 	gausspoint=0.7;
		// 	//gausspoint=0.5;
		// 	coldgausspoint=0.95;		//This now makes no sense
		// }

		if (severe) {
			gausspoint=0.3;
			coldgausspoint=0.1;
		} else {
			//Half the time don't do any cold mutations
			if (randfloat()>0.5) {
				gausspoint=1.0-rate;
				coldgausspoint=1.0-rate-0.1;
			}
			else {
				gausspoint=1.0-rate;
				coldgausspoint=1.0-rate;
			}
		}


		randnum=randposneg()*randfloat()*power;
		//std::cout << randnum << std::endl;
		randchoice=randfloat();
		if (randchoice>gausspoint)		//If random float is higher than gausspoint, mutate link weight
			*weight_vector[i]+=randnum;
		else if (randchoice>coldgausspoint)		//If random number is higher than coldgausspoint, replace link weight
			*weight_vector[i]=randnum;


		//Cap the weights at 8.0
		if (*weight_vector[i] > 8.0) *weight_vector[i] = 8.0;
		else if (*weight_vector[i] < -8.0) *weight_vector[i] = -8.0;

	} //end for loop

}

void NNodeGRU::create_weight_vector() {

	weight_vector.clear();

	weight_vector.push_back(&b);
	weight_vector.push_back(&b_u);
	weight_vector.push_back(&b_r);
	weight_vector.push_back(&w);
	weight_vector.push_back(&w_u);
	weight_vector.push_back(&w_r);

	for(int i = 0; i < U.size(); i++) {

		weight_vector.push_back(&U[i]);

	}

	for(int i = 0; i < U.size(); i++) {

		weight_vector.push_back(&U_u[i]);

	}

	for(int i = 0; i < U.size(); i++) {

		weight_vector.push_back(&U_r[i]);

	}

	//std::cout << "CREATE WEIGHT VECTOR: " << std::endl;
	//std::cout << *weight_vector[0] << std::endl;

}

//When link is mutated in, add additional weights
void NNodeGRU::added_in_link() {

	//TODO: Determine what to set these initial weights to

	U.push_back(0.0);
	U_u.push_back(0.0);
	U_r.push_back(0.0);

	//std::cout << "U size after adding link: " <<U.size() << std::endl;

}

void NNodeGRU::deleted_link(int weight_num) {

	U.erase(U.begin()+weight_num);
	U_u.erase(U_u.begin()+weight_num);
	U_r.erase(U_r.begin()+weight_num);

}

void NNodeGRU::print_gru_to_file(std::ofstream &outFile) {

	if(!PRE_PRECISION_VERSION)
		outFile<<std::setprecision(std::numeric_limits<double>::max_digits10);

	outFile<<"nodegru "<<node_id<<" ";
	outFile<<gen_node_label<<" ";
	outFile<<b<<" ";
	outFile<<b_u<<" ";
	outFile<<b_r<<" ";
	outFile<<w<<" ";
	outFile<<w_r<<" ";
	outFile<<w_u<<" [ ";

	for(int i = 0; i < U.size(); i++) {

		outFile<<U[i]<<" ";

	}

	outFile<<"] [ ";

	for(int i = 0; i < U_r.size(); i++) {

		outFile<<U_r[i]<<" ";

	}

	outFile<<"] [ ";

	for(int i = 0; i < U_u.size(); i++) {

		outFile<<U_u[i]<<" ";

	}

	outFile<<"]"<<std::endl;

}

void NNodeGRU::print_gru_to_file(std::ostream &outFile) {

	if(!PRE_PRECISION_VERSION)
		outFile<<std::setprecision(std::numeric_limits<double>::max_digits10);

	// char tempbuf[128];
	// sprintf(tempbuf, "nodegru %d %d ", node_id, gen_node_label);
	// outFile << tempbuf;
	//
	// char tempbuf2[128];
	// sprintf(tempbuf2, " %d %d %d ", b, b_u, b_r);
	// outFile << tempbuf2;
	//
	// char tempbuf3[128];
	// sprintf(tempbuf3, " %d %d %d [", w, w_r, w_u);
	// outFile << tempbuf3;
	//
	// char tempbuf4[128];
	// for(int i = 0; i < U.size(); i++) {
	//
	// 	sprintf(tempbuf4, " %d ", U[i]);
	//
	// }
	// sprintf(tempbuf4, "] [");
	// outFile << tempbuf4;
	//
	// char tempbuf5[128];
	// for(int i = 0; i < U_r.size(); i++) {
	//
	// 	sprintf(tempbuf5, " %d ", U_r[i]);
	//
	// }
	// sprintf(tempbuf5, "] [");
	// outFile << tempbuf5;
	//
	// char tempbuf6[128];
	// for(int i = 0; i < U_u.size(); i++) {
	//
	// 	sprintf(tempbuf6, " %d ", U_u[i]);
	//
	// }
	// sprintf(tempbuf6, "]\n");
	// outFile << tempbuf6;

	outFile<<"nodegru "<<node_id<<" ";
	outFile<<gen_node_label<<" ";
	outFile<<b<<" ";
	outFile<<b_u<<" ";
	outFile<<b_r<<" ";
	outFile<<w<<" ";
	outFile<<w_r<<" ";
	outFile<<w_u<<" [ ";

	for(int i = 0; i < U.size(); i++) {

		outFile<<U[i]<<" ";

	}

	outFile<<"] [ ";

	for(int i = 0; i < U_r.size(); i++) {

		outFile<<U_r[i]<<" ";

	}

	outFile<<"] [ ";

	for(int i = 0; i < U_u.size(); i++) {

		outFile<<U_u[i]<<" ";

	}

	outFile<<"]"<<std::endl;

}

void NNodeGRU::debug_print() {

	std::cout<<"nodegru "<<node_id<<" ";
	std::cout<<gen_node_label<<" ";
	std::cout<<b<<" ";
	std::cout<<b_u<<" ";
	std::cout<<b_r<<" ";
	std::cout<<w<<" ";
	std::cout<<w_r<<" ";
	std::cout<<w_u<<" [ ";

	for(int i = 0; i < U.size(); i++) {

		std::cout<<U[i]<<" ";

	}

	std::cout<<"] [ ";

	for(int i = 0; i < U_r.size(); i++) {

		std::cout<<U_r[i]<<" ";

	}

	std::cout<<"] [ ";

	for(int i = 0; i < U_u.size(); i++) {

		std::cout<<U_u[i]<<" ";

	}

	std::cout<<"]"<<std::endl;

}
