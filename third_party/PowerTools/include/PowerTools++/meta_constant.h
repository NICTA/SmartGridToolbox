//
//  meta_constant.h
//  PowerTools++
//
//  Created by Hassan on 12/05/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#ifndef __PowerTools____meta_constant__
#define __PowerTools____meta_constant__
#include "var_.h"
#include <stdio.h>

class meta_constant :public var_{
    
public:
    double val;
    meta_constant();
    meta_constant(Model* model);
    meta_constant(string name, Model* model);
    meta_constant& operator=(double v);
    
};
#endif /* defined(__PowerTools____meta_constant__) */
