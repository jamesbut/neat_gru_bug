#include "complexity_regulation.h"

using namespace NEAT;

#include "organism.h"

ComplexityRegulation::ComplexityRegulation(ComplexityMode starting_mode) :
   complexity_ceiling(100),
   MIN_SIMPLIFICATION_GENERATIONS(20),
   last_transition_generation(1),
   complexity_accumulator(bt::rolling_window::window_size = 50),
   complexity_mode(starting_mode),
   COMPLEXIFYING_CONNECTION_WEIGHT_MUT_PROB(NEAT::mutate_link_weights_prob),
   COMPLEXIFYING_ADD_NODE_MUT_PROB(NEAT::mutate_add_node_prob),
   COMPLEXIFYING_ADD_GRU_NODE_MUT_PROB(NEAT::mutate_add_gru_node_prob),
   COMPLEXIFYING_ADD_CONNECTION_MUT_PROB(NEAT::mutate_add_link_prob),
   COMPLEXIFYING_DELETE_CONNECTION_MUT_PROB(0.0),
   SIMPLIFYING_CONNECTION_WEIGHT_MUT_PROB(0.6),
   SIMPLIFYING_ADD_NODE_MUT_PROB(0.0),
   SIMPLIFYING_ADD_GRU_NODE_MUT_PROB(0.0),
   SIMPLIFYING_ADD_CONNECTION_MUT_PROB(0.0),
   SIMPLIFYING_DELETE_CONNECTION_MUT_PROB(0.4) {}

void ComplexityRegulation::evaluate_complexity(const std::vector<Organism*>& organisms, int generation) {

   current_generation = generation;

   calculate_mean_complexity(organisms);
   determine_complexity_mode();
   set_params_for_complexity();

}

void ComplexityRegulation::calculate_mean_complexity(const std::vector<Organism*>& organisms) {

   int total_complexity = 0;

   for(int i = 0; i < organisms.size(); i++) {

      total_complexity += organisms[i]->gnome->get_complexity();

   }

   mean_complexity = (double)total_complexity / (double)organisms.size();

   previous_complexity_ma = current_complexity_ma;
   complexity_accumulator(mean_complexity);
   current_complexity_ma = ba::rolling_mean(complexity_accumulator);

   std::cout << "Mean complexity: " << mean_complexity << std::endl;
   std::cout << "Complexity MA: " << current_complexity_ma << std::endl;

}

void ComplexityRegulation::determine_complexity_mode() {

   if(complexity_mode == Complexifying) {

      //If the mean complexity is greater than complexity ceiling, simplify
      if(mean_complexity > complexity_ceiling) {

         std::cout << "|-----------------------------------|" << std::endl;
         std::cout << "|          SIMPLIFYING              |" << std::endl;
         std::cout << "|-----------------------------------|" << std::endl;

         complexity_mode = Simplifying;
         last_transition_generation = current_generation;

      }

   } else {

      //If enough generation haves occured since simplify began,
      //   the mean complexity is less than the complexity ceiling
      //   simplification has stalled
      if((current_generation - last_transition_generation > MIN_SIMPLIFICATION_GENERATIONS)
      && (mean_complexity < complexity_ceiling)
      && (current_complexity_ma >= previous_complexity_ma)) {

         std::cout << "|-----------------------------------|" << std::endl;
         std::cout << "|          COMPLEXIFYING            |" << std::endl;
         std::cout << "|-----------------------------------|" << std::endl;

         //Then swap to complexifying
         complexity_mode = Complexifying;
         last_transition_generation = current_generation;

      }

   }

}

//Swap parameters based on complexity mode
void ComplexityRegulation::set_params_for_complexity() {

   if(complexity_mode == Complexifying) {

      NEAT::mutate_link_weights_prob = COMPLEXIFYING_CONNECTION_WEIGHT_MUT_PROB;
      NEAT::mutate_add_node_prob = COMPLEXIFYING_ADD_NODE_MUT_PROB;
      NEAT::mutate_add_gru_node_prob = COMPLEXIFYING_ADD_GRU_NODE_MUT_PROB;
      NEAT::mutate_add_link_prob = COMPLEXIFYING_ADD_CONNECTION_MUT_PROB;
      NEAT::mutate_delete_link_prob = COMPLEXIFYING_DELETE_CONNECTION_MUT_PROB;

   } else {

      NEAT::mutate_link_weights_prob = SIMPLIFYING_CONNECTION_WEIGHT_MUT_PROB;
      NEAT::mutate_add_node_prob = SIMPLIFYING_ADD_NODE_MUT_PROB;
      NEAT::mutate_add_gru_node_prob = SIMPLIFYING_ADD_GRU_NODE_MUT_PROB;
      NEAT::mutate_add_link_prob = SIMPLIFYING_ADD_CONNECTION_MUT_PROB;
      NEAT::mutate_delete_link_prob = SIMPLIFYING_DELETE_CONNECTION_MUT_PROB;

   }


}
