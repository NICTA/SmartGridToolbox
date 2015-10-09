#ifndef __PowerTools____GurobiProgram
#define __PowerTools____GurobiProgram

#include <gurobi_c++.h>
#include <PowerTools++/Model.h>

class GurobiProgram {
private:

    GRBModel* grb_mod;
    GRBEnv* grb_env;
    std::map<int, GRBVar*> _grb_vars; /** Mapping variables to Gurobi variables */
public:
    Model *model;
    int _output;
    GurobiProgram();
    GurobiProgram(Model* m);
    ~GurobiProgram();
    void reset_model();

    void solve(bool relax);
    void prepare_model();
    void update_model();
    void relax_model();

    void fill_in_grb_vmap();
    void create_grb_constraints();
    void set_grb_objective();

    void print_constraints();
};


#endif /* defined(__PowerTools____GurobiProgram) */
