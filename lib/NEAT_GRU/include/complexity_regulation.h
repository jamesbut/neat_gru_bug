#ifndef _COMPLEXITY_REGULATION_H
#define _COMPLEXITY_REGULATION_H

#include <vector>

namespace NEAT {

   class Organism;

   class ComplexityRegulation {

      public:

         enum ComplexityMode{Complexifying, Simplifying};

         ComplexityRegulation(ComplexityMode starting_mode);

         void evaluate_complexity(const std::vector<Organism*>& organisms);

      private:

         void determine_complexity_mode();
         void set_params_for_complexity();

         ComplexityMode complexity_mode;

      };

}

#endif
