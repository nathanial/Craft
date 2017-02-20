//
// Created by nathan on 2/19/17.
//

#ifndef CRAFT_OAKTREE_H
#define CRAFT_OAKTREE_H

#include "../TerrainFeature.h"

class OakTree : public TerrainFeature {
public:
    virtual Blocks create() override;
};


#endif //CRAFT_OAKTREE_H
