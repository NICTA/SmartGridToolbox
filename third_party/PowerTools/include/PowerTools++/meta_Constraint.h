//
//  meta_Constraint.h
//  PowerTools++
//
//  Created by Hassan on 12/05/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#ifndef __PowerTools____meta_Constraint__
#define __PowerTools____meta_Constraint__

#include <stdio.h>
#include "Constraint.h"
#include "meta_var.h"
#include "meta_constant.h"

class meta_Constraint :public Constraint{
    
public:

    int _nb_concrete; /**< Number of concretised versions of this meta-constraint */
    meta_Constraint();
    

    meta_Constraint& operator=(double rhs);
    meta_Constraint& operator<=(double rhs);
    meta_Constraint& operator>=(double rhs);
    meta_Constraint& operator=(const Function& f);
};
#endif /* defined(__PowerTools____meta_Constraint__) */
