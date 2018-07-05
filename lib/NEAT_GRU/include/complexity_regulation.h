#ifndef _COMPLEXITY_REGULATION_H
#define _COMPLEXITY_REGULATION_H

#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

//Used for moving average
namespace ba = boost::accumulators;
namespace bt = ba::tag;
//typedef ba::accumulator_set < uint32_t, ba::stats <bt::rolling_mean > > MeanAccumulator;
typedef ba::accumulator_set < double, ba::stats <bt::rolling_mean > > MeanAccumulator;

namespace NEAT {

   class Organism;

   class ComplexityRegulation {

      public:

         enum ComplexityMode{Complexifying, Simplifying};

         ComplexityRegulation(ComplexityMode starting_mode);

         void evaluate_complexity(const std::vector<Organism*>& organisms, int generation);

      private:

         void calculate_mean_complexity(const std::vector<Organism*>& organisms);

         void determine_complexity_mode();

         void set_params_for_complexity();

         const int MIN_SIMPLIFICATION_GENERATIONS;

         int complexity_ceiling;

         ComplexityMode complexity_mode;
         double mean_complexity;
         int current_generation;
         int last_transition_generation;

         MeanAccumulator complexity_accumulator;
         double previous_complexity_ma;
         double current_complexity_ma;

         const double COMPLEXIFYING_CONNECTION_WEIGHT_MUT_PROB;
         const double COMPLEXIFYING_ADD_NODE_MUT_PROB;
         const double COMPLEXIFYING_ADD_GRU_NODE_MUT_PROB;
         const double COMPLEXIFYING_ADD_CONNECTION_MUT_PROB;
         const double COMPLEXIFYING_DELETE_CONNECTION_MUT_PROB;

         const double SIMPLIFYING_CONNECTION_WEIGHT_MUT_PROB;
         const double SIMPLIFYING_ADD_NODE_MUT_PROB;
         const double SIMPLIFYING_ADD_GRU_NODE_MUT_PROB;
         const double SIMPLIFYING_ADD_CONNECTION_MUT_PROB;
         const double SIMPLIFYING_DELETE_CONNECTION_MUT_PROB;

      };

}

#endif
