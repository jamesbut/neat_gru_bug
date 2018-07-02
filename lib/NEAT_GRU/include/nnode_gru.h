#ifndef _NNODE_GRU_H
#define _NNODE_GRU_H

#include "trait.h"
#include "nnode.h"
#include <vector>

namespace NEAT {

	class NNodeGRU : public NNode {

		public:

			NNodeGRU(const char *argline, std::vector<Trait*> &traits);
			NNodeGRU(int nodeid, NNodeGRU* node);
			NNodeGRU(int nodeid);

			//~NNodeGRU();

			void activate_gru(std::vector<double> inputs);
			void activate_gru_old(std::vector<double> inputs);
			void mutate(double power);
			double compatibility(NNodeGRU node);
			void added_in_link();
			void deleted_link(int weight_num);

			void print_gru_to_file(std::ofstream &outFile);
			void print_gru_to_file(std::ostream &outFile);

			void debug_print();

			void set_active_inputs(std::vector<bool> active_ins) {active_inputs = active_ins;};

			int getUSize() {return U.size();};

		private:

			//GRU weights
			double b;
			double b_u;
			double b_r;
			double w;
			double w_r;
			double w_u;
			std::vector<double> U;
			std::vector<double> U_r;
			std::vector<double> U_u;

			//Used for iterating through in mutate
			std::vector<double*> weight_vector;

			//Used to check what inputs are active(enabled)
			std::vector<bool> active_inputs;

			void create_weight_vector();

	};

}

#endif
