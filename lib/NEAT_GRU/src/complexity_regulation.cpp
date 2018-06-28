#include "complexity_regulation.h"

using namespace NEAT;

#include "organism.h"

ComplexityRegulation::ComplexityRegulation(ComplexityMode starting_mode) {

   complexity_mode = starting_mode;

}

void ComplexityRegulation::evaluate_complexity(const std::vector<Organism*>& organisms) {

   if(complexity_mode == Complexifying) {

      //If the mean complexity is greater than complexity ceiling, simplify

   } else {

      //If enough generation haves occured since simplify began,
      //   the mean complexity is less than the complexity ceiling
      //   simplification has stalled
      //Then
      //   swap to complexifying

   }

}

void ComplexityRegulation::determine_complexity_mode() {}
void ComplexityRegulation::set_params_for_complexity() {}
