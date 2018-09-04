#ifndef _ENVIRONMENT_BUILDER_H_
#define _ENVIRONMENT_BUILDER_H_

#include "environment_generator.h"

#include <argos3/plugins/simulator/entities/box_entity.h>

/*
   This class will take the environment generator and from that build
   the environment in argos. This will therefore be inside the loop functions
   and the individual parallel processes
*/

class EnvironmentBuilder {

public:

   EnvironmentBuilder();
   void build_env(EnvironmentGenerator& env_generator);

private:

   void putLinesInEnvironment(EnvironmentGenerator& env_generator);
   void putBlocksInEnvironment(EnvironmentGenerator& env_generator);

   void clearEnvironment();

   //Seems this isn't set but is used in an if statement
   //probably redundant
   int _map_request_type;

   std::vector<argos::CBoxEntity*> boxEntities;
   int it_box;
   int total_boxes_generated;

};

#endif
