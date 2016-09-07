//
//  Model.cpp
//  PowerTools++
//
//  Created by Hassan on 5/01/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#include "PowerTools++/Model.h"

using namespace std;

/** Constructor */
    //@{
Model::Model(){
    _name = NULL;
    _obj = NULL;
    _nnz_g = 0;
    _nnz_h = 0;
    _idx_var = 0;
    _idx_con = 0;
};
    //@}

/* Destructor */
Model::~Model(){
    for (auto it:_cons) {
        delete it;
    }
    for (auto it:_meta_cons) {
        delete it.second;
    }
    for (auto it:_meta_vars) {
        delete it.second;
    }
    delete _obj;
};


/* Accessors */

int get_hess_index(var_* vi, var_* vj){
    return vi->_hess_id[vj->get_idx()];
//    return _hess_index.find(to_string(vid)+','+to_string(vjd))->second;
}

int get_hess_index(var_* vi, int vjd){
    return vi->_hess_id[vjd];
        //    return _hess_index.find(to_string(vid)+','+to_string(vjd))->second;
}

int Model::get_nb_vars() const{
    return (int)_vars.size();
};

int Model::get_nb_cons() const{
    return (int)_cons.size();
};


int Model::get_nb_nnz_g() const{
//    return _nnz_g;
    int idx = 0;
    for(auto& c: _cons)
    {
        idx += c->get_nb_vars();
    }
//    cout << "Jacobian nnz = " << idx << endl;
    return idx;
};

void Model::update_hess_link(var_ *v){
    int vid = v->get_idx();
    /* Hessian links in the objective */
    if(_obj->_hess.count(vid)==1) {
        for (int vjd:*_obj->_hess[vid]){
            v->_hess.insert(vjd);
        }
    }
    /* Hessian links in the constraints */
    for (auto c_it:v->_cstrs){
        if(c_it.second->_hess.count(vid)==1) {
            for (int vjd:*c_it.second->_hess[vid]){
                v->_hess.insert(vjd);
            }
        }
    }
}

//Split "mem" into "parts", e.g. if mem = 10 and parts = 4 you will have: 0,2,4,6,10
//if possible the function will split mem into equal chuncks, if not
//the last chunck will be slightly larger

std::vector<int> bounds(int parts, int mem) {
    std::vector<int>bnd;
    int delta = mem / parts;
    int reminder = mem % parts;
    int N1 = 0, N2 = 0;
    bnd.push_back(N1);
    for (int i = 0; i < parts; ++i) {
        N2 = N1 + delta;
        if (i == parts - 1)
            N2 += reminder;
        bnd.push_back(N2);
        N1 = N2;
    }
    return bnd;
}


void Model::update_hess_link(){
    for (auto v_it:_vars){
        update_hess_link(v_it);
    }
}

int Model::get_nb_nnz_h() const{
    int idx=0;
    int vid = 0, vjd = 0;
    /* return the structure of the hessian */
    for(auto& v: _vars)
    {
        vid = v->get_idx();
//        v->print();
//        cout << "hessian link :\n";
        for(auto itv = v->_hess.cbegin(); itv != v->_hess.cend(); ++itv)
        {
            vjd = *itv;
            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
//                vj = this->_vars.find(vjd)->second;
//                vj->print();
//                cout << " , ";
                idx++;
            }
        }
//        cout << "\n";
    }
//        cout << "Hessian nnz = " << idx << endl;
//    exit(-1);
    return idx;
};


bool Model::hasVar(var_* v) const{
    return (v->get_idx()<_vars.size());
};

var_* Model::getVar(int vid){
    return _vars[vid];
};

/* Modifiers */

void Model::addVar(var_& v){
    _vars.push_back(&v);
    v.set_idx(_idx_var++);
//    v.print();
};

void Model::addMetaVar(var_& v){
    v.set_idx((int)_meta_vars.size());
    _meta_vars.insert(pair<int, var_*>(_meta_vars.size(), &v));
};

void Model::addMetaConstant(var_& c){
    c.set_idx((int)_meta_vars.size());
    _meta_vars.insert(pair<int, var_*>(_meta_vars.size(), &c));
};


void Model::delVar(var_* v){
//    _vars.erase(v->get_idx());
    assert(false);
};

void Model::addConstraint(Constraint c_){
    Constraint* c = new Constraint(c_);
    _cons.push_back(c);
    c->set_idx(_idx_con++);
    var_* vi = NULL;
    int vid = 0;
//    Function* dfdx = NULL;
    for(auto it = c->_vars.cbegin(); it != c->_vars.end(); it++) {
        vid = it->first;
        vi = it->second;
        vi->addConstraint(c);
//        if (c->get_ftype()==nlin_) {
//            dfdx = c->get_dfdx(vi);
//                    c->print();
//                    vi->print();
//                    cout << " : ";
//                    dfdx->print(true);
//            c->set_dfdx(vid, dfdx);
//                    if (c->_hess.count(vid)==1) {
//                        for (int vjd : *c->_hess[vid]){
////                            if(dfdx->has_var(vjd) && dfdx->_exp && !dfdx->_exp->is_leaf() && dfdx->_exp->has_var(vjd))
//                            if(dfdx->has_var(vjd))
//                                dfdx->set_dfdx(vjd, dfdx->get_dfdx(vjd));
////                                    c->print();
////                                    vi->print();
////                                    cout << " : ";
////                                    dfdx->print(true);
////                                    getVar(vjd)->print();
////                                    cout << " : ";
////                                    dfdx->get_dfdx(vjd)->print(true);
//
//
//                        }
//                    }

//        }
    }
    _nnz_g+=c->get_nb_vars();

//    c->print();
};

void Model::addConstraint(Constraint* c){
    _cons.push_back(c);
    c->set_idx(_idx_con++);
    var_* vi = NULL;
    int vid = 0;
    //    Function* dfdx = NULL;
    for(auto it = c->_vars.cbegin(); it != c->_vars.end(); it++) {
        vid = it->first;
        vi = it->second;
        vi->addConstraint(c);
        //        if (c->get_ftype()==nlin_) {
        //            dfdx = c->get_dfdx(vi);
        //                    c->print();
        //                    vi->print();
        //                    cout << " : ";
        //                    dfdx->print(true);
        //            c->set_dfdx(vid, dfdx);
        //                    if (c->_hess.count(vid)==1) {
        //                        for (int vjd : *c->_hess[vid]){
        ////                            if(dfdx->has_var(vjd) && dfdx->_exp && !dfdx->_exp->is_leaf() && dfdx->_exp->has_var(vjd))
        //                            if(dfdx->has_var(vjd))
        //                                dfdx->set_dfdx(vjd, dfdx->get_dfdx(vjd));
        ////                                    c->print();
        ////                                    vi->print();
        ////                                    cout << " : ";
        ////                                    dfdx->print(true);
        ////                                    getVar(vjd)->print();
        ////                                    cout << " : ";
        ////                                    dfdx->get_dfdx(vjd)->print(true);
        //
        //
        //                        }
        //                    }
        
        //        }
    }
    _nnz_g+=c->get_nb_vars();
    
        c->print();
};


int Model::has_function(shared_ptr<Function> f){
    int idx = 0;
    for (auto& f0: _functions){
        if (*f==*f0) {
            return idx;
        }
        idx++;
    }
    return -1;
}

void Model::init_functions(int size){
    for (auto& f: _functions){
        f->_val.resize(size);
        f->_evaluated.resize(size);
    }
    for (auto& mc: _meta_cons){
        mc.second->_val.resize(size);
        mc.second->_evaluated.resize(size);
    }
}

void Model::add_functions(shared_ptr<Function> f){
    if (f->is_constant()) {
        return;
    }
    int idx = has_function(f);
    if (idx!=-1) {
        _functions[idx]->merge_dfdx(*f);        
    }
    else{
        _functions.push_back(f);
    }
    if (f->_lparent) {
        int idx = has_function(f->_lparent);
        if (idx!=-1) {
            _functions[idx]->merge_dfdx(*f->_lparent);
            f->_lparent = (_functions[idx]);
        }
        else {
            add_functions(f->_lparent);
        }
    }
    if (f->_rparent) {
        int idx = has_function(f->_rparent);
        if (idx!=-1) {
            _functions[idx]->merge_dfdx(*f->_rparent);
            f->_rparent = (_functions[idx]);
        }
        else {
            add_functions(f->_rparent);
        }
    }
}

void Model::concretise(meta_Constraint& mc, int meta_link, string name){
    Constraint* c = new Constraint();
    c->_name = name;
    c->_ctype = mc._ctype;
    c->_ftype = mc._ftype;
    c->_rhs = mc._rhs;
    c->_meta_link = meta_link;
    
    //    c += this->Function::concretise();
//    int cid = 0;
    var_* vi = NULL;
    meta_var* m_vi = NULL;
    meta_constant* m_ci = NULL;
    int vid = 0;
    for (auto it:mc._vars) {
        vid = it.first;
        vi = it.second;
        if (vi->is_constant()) {
            m_ci = (meta_constant*)(vi);
            c->_meta_coeff.insert(pair<int,double>(vid, m_ci->val));
        }
        else {
            m_vi = (meta_var*)(vi);
//            cid = _idx_con;
//            m_vi->var->_meta_var.resize(cid+1);
//            m_vi->var->_meta_var[cid] = vid;
            c->_meta_vars.insert(pair<int,var_*>(vid, m_vi->var));
            c->_meta_ids.insert(pair<int,int>(m_vi->var->get_idx(), vid));
            c->_vars.insert(pair<int,var_*>(m_vi->var->get_idx(), m_vi->var));
        }
    }
    for(auto& it:mc._hess)
    {
        vid = it.first;
        if (mc.get_var(vid)->is_constant()) {
            continue;
        }
        for(int vjd:*it.second)
        {
            if (!mc.get_var(vjd)->is_constant()) {
                c->add_hess_link(c->_meta_vars[vid]->get_idx(), c->_meta_vars[vjd]->get_idx());
            }
        }
    }
    c->_meta_constr = &mc;
    addConstraint(c);
    //    c.Function::print(true);
    mc._nb_concrete++;
//    return *c;
}


void Model::addMetaConstraint(meta_Constraint& c){
    int idx = 0;
    _meta_cons.insert(pair<int, meta_Constraint*>(_meta_cons.size(), &c));
    c.set_idx((int)_meta_cons.size()-1);
    if (c._lparent) {
        idx = has_function(c._lparent);
        if (idx!=-1) {
            _functions[idx]->merge_dfdx(*c._lparent);
            c._lparent = (_functions[idx]);
        }
        else
            add_functions(c._lparent);
    }
    if (c._rparent) {
        idx = has_function(c._rparent);
        if (idx!=-1) {
            _functions[idx]->merge_dfdx(*c._rparent);
            c._rparent = (_functions[idx]);
        }
        else
            add_functions(c._rparent);
    }
    var_* vi = NULL;
    var_* vj = NULL;
    int vid = 0;
    for(auto it = c._vars.cbegin(); it != c._vars.end(); it++) {
        vid = it->first;
        vi = it->second;
        if (vi->get_type()==constant) {
            continue;
        }
        vi->addConstraint(&c);
        if (c.get_ftype()==nlin_ || c.has_meta()) {
            
            c.compute_dfdx(vi);
//            if (c._dfdx[vid]->_lparent) {
//                add_functions(*c._dfdx[vid]->_lparent);
//            }
//            if (c._dfdx[vid]->_rparent) {
//                add_functions(*c._dfdx[vid]->_rparent);
//            }
            idx = has_function(c._dfdx[vid]);
            if (idx!=-1) {
                _functions[idx]->merge_dfdx(*c._dfdx[vid]);
                c._dfdx[vid] = (_functions[idx]);
            }
            else
                add_functions(c._dfdx[vid]);
//                                c->print();
//                                vi->print();
//                                cout << " : ";
//                                dfdx->print(true);
            if (c._hess.count(vid)==1) {
                for (int vjd : *c._hess[vid]){
                    ////                            if(dfdx->has_var(vjd) && dfdx->_exp && !dfdx->_exp->is_leaf() && dfdx->_exp->has_var(vjd))
                    //                                        if(dfdx->has_var(vjd))
                    vj = c.get_var(vjd);
                    if(vj->get_type()==constant) {
                        continue;
                    }
                    c._dfdx[vid]->compute_dfdx(c.get_var(vjd));
//                    if (c._dfdx[vid]->_dfdx[vjd]->_lparent) {
//                        add_functions(*(c._dfdx[vid]->_dfdx[vjd]->_lparent));
//                    }
//                    if (c._dfdx[vid]->_dfdx[vjd]->_rparent) {
//                        add_functions(*(c._dfdx[vid]->_dfdx[vjd]->_rparent));
//                    }

                    idx = has_function(c._dfdx[vid]->_dfdx[vjd]);
                    if (idx!=-1) {
//                        if (_functions[idx]->_dfdx.size()==0) {
                            _functions[idx]->merge_dfdx(*c._dfdx[vid]->_dfdx[vjd]);
//                        }
                        c._dfdx[vid]->_dfdx[vjd] = (_functions[idx]);
                    }
                    else
                        add_functions(c._dfdx[vid]->_dfdx[vjd]);
//                    c->print();
//                    vi->print();
//                    cout << " : ";
//                    dfdx->print(true);
//                    vj = c->get_var(vjd);
//                    vj->print();
//                    cout << " : ";
//                    dfdx->get_dfdx(vjd)->print(true);
                    //
                    
                }
            }
            
        }
    }
};

void Model::on_off(Constraint c, var<bool>& on){
    if (c.get_ftype() != lin_) {
        cerr << "Nonlinear constraint.\n";
        exit(-1);
    }
    var<>* v;
    const Quadratic *orig_q;
    Constraint res(c.get_name() + "_on/off");
    double b;
    orig_q = c.get_quad();
    b = c.get_rhs() - orig_q->get_const();
    for(auto it: orig_q->_coefs) {
        v = getVar_<double>(it.first);
        if (!v->is_bounded_below() || !v->is_bounded_above()) {
            cerr << "Variable does not have finite bounds.\n";
            exit(1);
        }
        if (c.get_type() == leq || c.get_type() == eq) {
            if (it.second < 0) res -= it.second*v->get_lb_off() - (0+on)*it.second*v->get_lb_off();
            else res -= it.second*v->get_ub_off() - it.second*v->get_ub_off()*on;
        }
        else{ // geq
            if (it.second < 0) res -= it.second*v->get_ub_off() - (0+on)*it.second*v->get_ub_off();
            else res -= it.second*v->get_lb_off() - it.second*v->get_lb_off()*on;
        }
        /*Constraint xi1(c.get_name() + "_on/off_" + v->_name);
         xi1 += *v;
         xi1 -= (0+on)*v->get_ub() + v->get_ub_off() - (0+on)*v->get_ub_off();
         xi1 <= 0;
         addConstraint(xi1);
         Constraint xi2(c.get_name() + "_on/off_" + v->_name);
         xi2 += *v;
         xi2 -= (0+on)*v->get_lb() + v->get_lb_off() - (0+on)*v->get_lb_off();
         xi2 >= 0;
         addConstraint(xi2);*/
    }
    if (c.get_type() == eq) {
        Constraint res2(c.get_name() + "_on/off2");
        for(auto it: orig_q->_coefs) {
            v = getVar_<double>(it.first);
            if (it.second < 0) res2 -= it.second*v->get_ub_off() - it.second*v->get_ub_off()*on;
            else res2 -= it.second*v->get_lb_off() - it.second*v->get_lb_off()*on;
        }
        res2 += *orig_q;
        res2 -= b*on;
        res2 >= 0;
        addConstraint(res2);
    }
    res += *orig_q;
    res -= orig_q->get_const();
    res -= b*on;
    if (c.get_type() == eq or c.get_type() == leq) res <= 0;
    else res >= 0;
    addConstraint(res);
}

void Model::on_off(var<>& v, var<bool>& on){
    Constraint UB(v._name+"_UB_on/off");
    UB += v - v.get_ub()*on - (1-on)*v.get_ub_off();
    UB <= 0;
    addConstraint(UB);
    Constraint LB(v._name+"_LB_on/off");
    LB += v - v.get_lb()*on - (1-on)*v.get_lb_off();
    LB >= 0;
    addConstraint(LB);
}

void Model::add_McCormick(std::string name, var<>& v, var<>& v1, var<>& v2) {
    Constraint MC1(name+"_McCormick1");
    MC1 += v;
    MC1 -= v1.get_lb()*v2 + v2.get_lb()*v1 - v1.get_lb()*v2.get_lb();
    MC1 >= 0;
    addConstraint(MC1);
    Constraint MC2(name+"_McCormick2");
    MC2 += v;
    MC2 -= v1.get_ub()*v2 + v2.get_ub()*v1 - v1.get_ub()*v2.get_ub();
    MC2 >= 0;
    addConstraint(MC2);
    Constraint MC3(name+"_McCormick3");
    MC3 += v;
    MC3 -= v1.get_lb()*v2 + v2.get_ub()*v1 - v1.get_lb()*v2.get_ub();
    MC3 <= 0;
    addConstraint(MC3);
    Constraint MC4(name+"_McCormick4");
    MC4 += v;
    MC4 -= v1.get_ub()*v2 + v2.get_lb()*v1 - v1.get_ub()*v2.get_lb();
    MC4 <= 0;
    addConstraint(MC4);
}


void Model::add_on_off_McCormick(std::string name, var<>& v, var<>& v1, var<>& v2, var<bool>& on) {
    Constraint MC1(name+"_McCormick1");
    MC1 += v;
    MC1 -= v1.get_lb()*v2 + v2.get_lb()*v1 - v1.get_lb()*v2.get_lb();
    MC1 >= 0;
    on_off(MC1, on);
    Constraint MC2(name+"_McCormick2");
    MC2 += v;
    MC2 -= v1.get_ub()*v2 + v2.get_ub()*v1 - v1.get_ub()*v2.get_ub();
    MC2 >= 0;
    on_off(MC2, on);
    Constraint MC3(name+"_McCormick3");
    MC3 += v;
    MC3 -= v1.get_lb()*v2 + v2.get_ub()*v1 - v1.get_lb()*v2.get_ub();
    MC3 <= 0;
    on_off(MC3, on);
    Constraint MC4(name+"_McCormick4");
    MC4 += v;
    MC4 -= v1.get_ub()*v2 + v2.get_lb()*v1 - v1.get_ub()*v2.get_lb();
    MC4 <= 0;
    on_off(MC4, on);
}


void Model::delConstraint(Constraint* c){
//    _cons.erase(c->get_idx());
    assert(false);
};

void Model::setObjective(Function* f) {
    _obj = f;
}

void Model::setObjectiveType(ObjectiveType t) {
    _objt = t;
}


void Model::check_feasible(const double* x){
    int vid = 0;
//    var_* v = NULL;
    var<>* var = NULL;
    /* return the structure of the hessian */
    for(auto& v: _vars)
    {
        vid = v->get_idx();
        var = getVar_<double>(vid);
        if ((x[vid] - var->get_ub())>1e-6) {
            cerr << "violated upper bound constraint: ";
            var->print();
        }
        if ((x[vid] - var->get_lb())<-1e-6) {
            cerr << "violated lower bound constraint: ";
            var->print();
        }
    }
    int cid = 0;
    for(auto& c: _cons)
    {
        cid = c->get_idx();
        switch (c->get_type()) {
            case eq:
                if(fabs(c->eval(x)-c->_rhs) > 1e-6) {
                    cerr << "violated constraint: ";
                    c->print();
                    printf ("violation = %.10f;\n",(c->eval(x)-c->_rhs));
                }
                break;
            case leq:
                if((c->eval(x)-c->_rhs) > 1e-6) {
                    cerr << "violated constraint: ";
                    c->print();
                    printf ("violation = %.10f;\n",(c->eval(x)-c->_rhs));
                }
                break;
            case geq:
                if((c->eval(x)-c->_rhs) < -1e-6) {
                    cerr << "violated constraint: ";
                    c->print();
                    printf ("violation = %.10f;\n",(c->eval(x)-c->_rhs));
                }
                break;

            default:
                break;
        }
    }
}

template<typename Number> var<Number>* Model::getVar_(int idx) const{
    if(idx < _vars.size()){
        var_* v = _vars[idx];
        if(typeid(Number)==typeid(float) && v->is_real())
            return (var<Number>*)v;
        if(typeid(Number)==typeid(double) && v->is_longreal())
            return (var<Number>*)v;
        if(typeid(Number)==typeid(bool) && v->is_binary())
            return (var<Number>*)v;
        if(typeid(Number)==typeid(int) && v->is_int())
            return (var<Number>*)v;
    }
    else {
        cerr << "Error: variable not in function.\n";
        exit(-1);
    }
    cerr << "Error: wrong type for variable.\n";
    exit(-1);
    return NULL;
    
}

template var<bool>* Model::getVar_(int idx) const;
template var<int>* Model::getVar_(int idx) const;
template var<float>* Model::getVar_(int idx) const;
template var<>* Model::getVar_(int idx) const;


void Model::fill_in_var_bounds(double* x_l ,double* x_u) {
    var<int>* int_var = NULL;
    var<bool>* bin_var = NULL;
    var<float>* real_var = NULL;
    var<>* long_real_var = NULL;
    int idx=0;
    for(auto& v: _vars)
    {
        switch (v->get_type()) {
            case real:
                real_var = (var<float>*)v;
                x_l[idx] = (double)real_var->get_lb();
                x_u[idx] = (double)real_var->get_ub();
                break;
            case longreal:
                long_real_var = (var<>*)v;
                x_l[idx] = long_real_var->get_lb();
                x_u[idx] = long_real_var->get_ub();
                break;
            case integ:
                int_var = (var<int>*)v;
                x_l[idx] = (double)int_var->get_lb();
                x_u[idx] = (double)int_var->get_ub();
                break;
            case binary:
                bin_var = (var<bool>*)v;
                x_l[idx] = 0.0;
                x_u[idx] = 1.0;
                break;
            default:
                break;
        } ;
        idx++;
    }
//    cout << "idx = " << idx << endl;
}

void Model::fill_in_obj(const double* x , double& res){
    res = _obj->eval(x);
}

void Model::fill_in_cstr(const double* x , double* res, bool new_x){
    int idx=0;
    int cid = 0;
//    Constraint* c = NULL;
//    int nr_threads = 1;
//    std::vector<std::thread> threads;
//    //Split constraints into nr_threads parts
//    std::vector<int> limits = bounds(nr_threads, (int)_cons.size());
//    vector<double*> sub_res;
////    double* res_ = NULL;
//    //Launch nr_threads threads:
//    for (int i = 0; i < nr_threads; ++i) {
////        eval_funcs_parallel(x, 0, _cons.size());
//        threads.push_back(std::thread(&Model::eval_funcs_parallel, this, x, limits[i], limits[i+1]));
//    }
//    //Join the threads with the main thread
//    for(auto &t : threads){
//        t.join();
//    }

    int meta_link = -1, c_meta_link = -1;
    for(auto& c: _cons)
    {
        cid = c->get_idx();
        c_meta_link = c->_meta_link;
//        assert(cid==c->get_idx());
        if (c->has_meta()) {
//            if (new_x) {
                c->_meta_constr->_evaluated[c_meta_link] = false;
//            }
            if (meta_link!=c_meta_link) {
                meta_link = c_meta_link;
//                if (new_x) {
                    for (auto& f: _functions) {
                        f->_evaluated[meta_link] = false;
                    }
//                }
//                for (auto& f: _functions) {
//                    f->eval_meta(x, c->_meta_coeff, c->_meta_vars, c_meta_link);
//                }

            }
//            if (c->_meta_constr->_evaluated) {
//                cout << "oops\n";
//                assert(false);
//            }
//            if (c->is_nonlinear()) {
//                assert(c->_meta_constr->_lparent->_evaluated[c_meta_link]);
//                if (c->_rparent) {
//                    assert(c->_meta_constr->_rparent->_evaluated[c_meta_link]);
//                }
//            }
//            assert(c->_meta_constr->_evaluated[c_meta_link]);
            res[idx] = c->_meta_constr->eval_meta(x, c->_meta_coeff, c->_meta_vars, c_meta_link);
        }
        else
            res[idx] = c->eval(x);
        idx++;
    }
}

void Model::fill_in_jac(const double* x , double* res, bool new_x){
    int idx=0;
    int cid = 0;
    int vid = 0;
//    Constraint* c = NULL;
    var_* v = NULL;
    Function* dfdx = NULL;
    int meta_link = -1;
//    int nr_threads = 4;
//    std::vector<std::thread> threads;
//    //Split constraints into nr_threads parts
//    std::vector<int> limits = bounds(nr_threads, (int)_cons.size());
//    vector<double*> sub_res;
//    //    double* res_ = NULL;
//    //Launch nr_threads threads:
//    for (int i = 0; i < nr_threads; ++i) {
//        threads.push_back(std::thread(&Model::eval_funcs_parallel, this, x, limits[i], limits[i+1]));
//    }
//    //Join the threads with the main thread
//    for(auto &t : threads){
//        t.join();
//    }

    int c_meta_link = -1;
        /* return the values of the jacobian of the constraints */
    for(auto& c :_cons)
        {
            cid = c->get_idx();
            c_meta_link = c->_meta_link;
            if (c->has_meta() && meta_link!=c_meta_link) {
//            if (c->has_meta()){
//                c->_meta_constr->_evaluated[c->_meta_link] = false;
//                if (meta_link!=c->_meta_link) {
                    meta_link = c_meta_link;
//                    if (new_x) {
                        for (auto& f: _functions) {
                            f->_evaluated[meta_link] = false;
                        }
//                    }
//                    for (auto& f: _functions) {
//                        f->eval_meta(x, c->_meta_coeff, c->_meta_vars, c_meta_link);
//                    }
//                }
////                c->_meta_constr->eval_meta(x, c->_meta_coeff, c->_meta_vars);
            }
            for(auto itv = c->_vars.cbegin(); itv != c->_vars.cend(); ++itv)
            {
                vid = itv->first;
                v = itv->second;
//                assert(vid==v->get_idx());
//                if (c->get_idx()==12)
//                    cout << "ok\n";
//                if (!c->has_dfdx(vid)) {
//                    c->set_dfdx(vid, dfdx);
//                }
//                c->print();
//                v->print();
//                cout << " : ";
//                dfdx->print(true);
                if(c->has_meta()){
//                    if (c->has_meta()) {// c was created using a meta-constraint, calling a concretise() function.
//                        dfdx = c->get_meta_dfdx(v);
                    dfdx = c->_meta_constr->_dfdx[c->_meta_ids[vid]].get();
//                    if (!dfdx->is_constant()) {
//                        assert(dfdx->_evaluated[c_meta_link]);
//                    }
//                        dfdx->_evaluated = false;
//                        v->print();
//                        cout <<": ";
//                        dfdx->print(false);
                        res[idx] = dfdx->eval_meta(x, c->_meta_coeff, c->_meta_vars, c_meta_link);
//                    }
//                    else{
//                        assert(false);
//                        res[idx] = c->eval_dfdx(vid, x);
//                    }
                }
                else {
                    res[idx] = c->eval_dfdx(vid, x);
                }
//                res[idx] = c->eval_dfdx(vid, x);
                idx++;
            }
        }
}


void Model::fill_in_jac_nnz(int* iRow , int* jCol){
    int idx=0;
    int cid = 0;
    int vid = 0;
//    Constraint* c = NULL;
    var_* v = NULL;
    /* return the structure of the jacobian */
    for(auto& c :_cons)
    {
        cid = c->get_idx();
//        assert(cid==c->get_idx());
        for(auto itv = c->_vars.cbegin(); itv != c->_vars.cend(); ++itv)
        {
            vid = itv->first;
            v = itv->second;
//            assert(vid==v->get_idx());
            iRow[idx] = cid;
            jCol[idx] = vid;
            idx++;
        }
    }
}

void Model::fill_in_hess_nnz(int* iRow , int* jCol){
    int idx=0;
    int vid = 0, vjd = 0;
//    var_* v = NULL;
    /* return the structure of the hessian */
    for(auto& v: _vars)
    {
        vid = v->get_idx();
        for(auto itv = v->_hess.cbegin(); itv != v->_hess.cend(); ++itv)
        {
            vjd = *itv;
            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
                iRow[idx] = vid;
                jCol[idx] = vjd;
                v->_hess_id.insert(pair<int, int>(vjd, idx));
                getVar(vjd)->_hess_id.insert(pair<int, int>(vid, idx));
//                _hess_index.insert(pair<string, int>(to_string(vid)+','+to_string(vjd), idx));
                idx++;
            }
        }
    }
}

void Model::fill_in_var_linearity(Ipopt::TNLP::LinearityType* var_types){
    int vid = 0, cid = 0;
//    var_* vi = NULL;
    Constraint* c = NULL;
    bool linear = true;
    for(auto& vi: _vars)
    {
        vid = vi->get_idx();
        linear = true;
        for(auto itc = vi->_cstrs.cbegin(); itc != vi->_cstrs.cend(); ++itc)
        {
            cid = itc->first;
            c = itc->second;
            if (c->get_ftype()>=lin_) {
                linear=false;
            }
            
        }
        if (linear) {
            var_types[vid]=Ipopt::TNLP::LINEAR;
        }
        else
            var_types[vid]=Ipopt::TNLP::NON_LINEAR;
    }

}

void Model::fill_in_cstr_linearity(Ipopt::TNLP::LinearityType* const_types){
    int cid = 0;
    for(auto& c :_cons)
    {
        cid = c->get_idx();
            if (c->get_ftype()<=lin_) {
                const_types[cid]=Ipopt::TNLP::LINEAR;
            }
            else
                const_types[cid]=Ipopt::TNLP::NON_LINEAR;
            
        }
}


void Model::fill_in_hess1(const double* x , double obj_factor, const double* lambda, double* res){
    int vid = 0, vjd = 0, idx = 0;
    var_* vi = NULL;
    var_* vj = NULL;
    Function* obj_dfdx = NULL;
    double hess = 0;
    for(auto& v: _vars)
    {
        vid = v->get_idx();

        for(auto itv = v->_hess.cbegin(); itv != v->_hess.cend(); ++itv)
        {
            vjd = *itv;
            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
                res[idx] = 0;
                idx++;
            }
        }
    }
    
    for (auto it1:_obj->_vars){
        vid = it1.first;
        vi = it1.second;
        if (_obj->get_type()==nlin_) {
            //            obj_dfdx = _obj->get_dfdx(vi);
            //            if (obj_dfdx && !_obj->has_dfdx(vid)) {
            //                _obj->set_dfdx(vid, obj_dfdx);
            //            }
            //        }
            _obj->compute_dfdx(vi);
        }
        for (auto it2:_obj->_vars){
            vjd = it2.first;
            vj = it2.second;
            if (vjd<=vid) {
                if(_obj->_hess.count(vid)==1 && _obj->_hess.count(vjd)==1) {
                    if(_obj->get_type()==nlin_ && obj_dfdx){
                        if(obj_dfdx->has_var(vjd))
                            hess = obj_dfdx->eval_dfdx(vjd, x);
                        else
                            hess = 0;
                    }
                    else{
                        hess = _obj->get_q_coeff(vid, vjd);
                        if (vid==vjd)
                            hess *= 2;
                    }
                    if(hess != 0)
                        res[get_hess_index(vi, vj)] = obj_factor * hess;
                }
                else
                    res[get_hess_index(vi, vj)] = 0;
//                else
//                    res[get_hess_index(vid, vjd)] = 0;
            }
        }
    }
//    cout << _cons.size();
//    exit(-1);
//    assert(4*(int)floor(_cons.size()/4)+2==(int)(_cons.size()));
//    thread t1(&Model::multi_thread_hess, this, &_cons, x, lambda, res, 0, (int)floor(_cons.size()/2));
//    thread t2(&Model::multi_thread_hess, this, &_cons, x, lambda, res, (int)floor(_cons.size()/2)+1, (int)(_cons.size())-1);
    int nr_threads = 4;
    std::vector<std::thread> threads;
    //Split constraints into nr_threads parts
    std::vector<int> limits = bounds(nr_threads, (int)_cons.size());
    vector<double*> sub_res;
    double* res_ = NULL;
        //Launch nr_threads threads:
    for (int i = 0; i < nr_threads; ++i) {
        res_ = new double[_nnz_h];
        for (int idx = 0; idx < _nnz_h; idx++) {
            res_[idx] = 0;
        }
        sub_res.push_back(res_);
//        threads.push_back(std::thread(&Model::multi_thread_hess, this, &_cons, x, lambda, res_, limits[i], limits[i+1]));
    }
    //Join the threads with the main thread
    for(auto &t : threads){
        t.join();
    }
    for (int i = 0; i < nr_threads; ++i) {
        res_ = sub_res[i];
        for (int idx = 0; idx < _nnz_h; idx++) {
            res[idx] += res_[idx];
        }
        delete res_;
    }
//    for (auto it:_cons) {
//        cid = it.first;
//        c = it.second;
//        for (auto it1:c->_vars){
//            vid = it1.first;
//            if(c->_hess.count(vid)==0)
//                continue;
//            vi = it1.second;
//            if(c->get_ftype()==nlin_){
//                if (c->has_meta()) {
//                    dfdx = c->get_meta_dfdx(vi);
//                }
//                else {
//                    assert(false);
//                    dfdx = c->get_dfdx(vi);
//                }
//            }
//            for (auto it2:c->_vars){
//                vjd = it2.first;
//                if(c->_hess.count(vjd)==0)
//                    continue;
//                vj = it2.second;
//                if (vjd<=vid && vi->_hess.count(vjd)>0) {
//                    if(c->is_nonlinear()){
//                        if (c->has_meta()) {
//                            if(!dfdx->has_var(c->_meta_ids[vjd])){
//                                hess = 0;
//                            }
//                            else {
//                                hess = dfdx->get_dfdx(c->_meta_ids[vjd])->eval_meta(x, c->_meta_coeff, c->_meta_vars);
//                            }
//                        }
//                        else {
//                            if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0){
//                                continue;
//                            }
//                            if(!dfdx->has_var(vjd)){
//                                hess = 0;
//                            }
//                            else {
//                                hess = dfdx->eval_dfdx(vjd, x);
//                            }
//                        }
//                    }
//                    else {
//                        if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0){
//                            continue;
//                        }
//                        hess = c->get_q_coeff(vid, vjd);
//                        if (vid==vjd)
//                            hess *= 2;
//                    }
//                    if(hess != 0)
//                        res[get_hess_index(vi, vj)] += lambda[cid] * hess;
//                }
//            }
//        }
//    }
}

void Model::eval_funcs_parallel(const double* x , int start, int end){
    shared_ptr<Function> f = NULL;
    Constraint* c = nullptr;
    int meta_link = -1;
    for (int i = start; i < end; i++) {
        c = _cons[i];
//        if (c->has_meta() && meta_link != c->_meta_link){
        if (c->has_meta()){
//            meta_link = c->_meta_link;
            meta_link = c->get_idx();
            for (auto& f: _functions) {
                f->_evaluated[meta_link] = false;
            }
            for (auto& f: _functions) {
                f->eval_meta(x, c->_meta_coeff, c->_meta_vars, meta_link);
            }
        }
    }
}

void Model::fill_in_hess(const double* x , double obj_factor, const double* lambda, double* res, bool new_x){
    int vid = 0, vjd = 0, cid = 0, idx = 0;
    var_* vi = NULL;
    Function* obj_dfdx = NULL;
    Function* dfdx = NULL;
//    Constraint* c = NULL;
    double hess = 0;
//    int nr_threads = 1;
//    std::vector<std::thread> threads;
//    //Split constraints into nr_threads parts
//    std::vector<int> limits = bounds(nr_threads, (int)_cons.size());
//    vector<double*> sub_res;
//    //    double* res_ = NULL;
//    //Launch nr_threads threads:
//    for (int i = 0; i < nr_threads; ++i) {
//        threads.push_back(std::thread(&Model::eval_funcs_parallel, this, x, limits[i], limits[i+1]));
//    }
//    //Join the threads with the main thread
//    for(auto &t : threads){
//        t.join();
//    }

    for(auto& v: _vars)
    {
        vid = v->get_idx();

        for(auto itv = v->_hess.cbegin(); itv != v->_hess.cend(); ++itv)
        {
            vjd = *itv;
            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
                res[idx] = 0;
                idx++;
            }
        }
    }
    
    for (auto it1:_obj->_vars){
        vid = it1.first;
        vi = it1.second;
        if (_obj->get_type()==nlin_) {
                //            obj_dfdx = _obj->get_dfdx(vi);
                //            if (obj_dfdx && !_obj->has_dfdx(vid)) {
                //                _obj->set_dfdx(vid, obj_dfdx);
                //            }
                //        }
            _obj->compute_dfdx(vi);
        }
        for (auto it2:_obj->_vars){
            vjd = it2.first;
            if (vjd<=vid) {
                if(_obj->_hess.count(vid)==1 && _obj->_hess.count(vjd)==1) {
                    if(_obj->get_type()==nlin_ && obj_dfdx){
                        if(obj_dfdx->has_var(vjd))
                            hess = obj_dfdx->eval_dfdx(vjd, x);
                        else
                            hess = 0;
                    }
                    else{
                        hess = _obj->get_q_coeff(vid, vjd);
                        if (vid==vjd)
                            hess *= 2;
                    }
                    if(hess != 0)
                        res[get_hess_index(vi, vjd)] = obj_factor * hess;
                }
                else
                    res[get_hess_index(vi, vjd)] = 0;
                    //                else
                    //                    res[get_hess_index(vid, vjd)] = 0;
            }
        }
    }
    int meta_link = -1;
            for (auto& c:_cons) {
                if (c->has_meta() && meta_link!=c->_meta_link){
//                    c->_meta_constr->_evaluated = false;
                    meta_link = c->_meta_link;
//                    if (new_x) {
//                    if (c->has_meta()) {
//                        for (auto& f: _functions) {
//                            f->_evaluated[meta_link] = false;
//                        }
//                    }
////                    c->_meta_constr->eval_meta(x, c->_meta_coeff, c->_meta_vars);
                }
                cid = c->get_idx();
                for (auto it1:c->_vars){
                    vid = it1.first;
                    if(c->_hess.count(vid)==0)
                        continue;
                    vi = it1.second;
                    if(c->has_meta()){
//                        if (c->has_meta()) {
//                            dfdx = c->get_meta_dfdx(vi);
                            dfdx = c->_meta_constr->_dfdx[c->_meta_ids[vid]].get();
//                            dfdx->eval_meta(x, c->_meta_coeff, c->_meta_vars);
//                        }
//                        else {
//                            assert(false);
//                            c->compute_dfdx(vi);
//                        }
                    }
                    for (auto& it2:*c->_hess[vid]){
                        vjd = it2;
//                        if(c->_hess.count(vjd)==0)
//                            continue;
                        if (vjd<=vid) {
                            if(c->has_meta()){
//                                if (c->has_meta()) {
                                    if(!dfdx->has_var(c->_meta_ids[vjd])){
                                        hess = 0;
                                    }
                                    else {
//                                        dfdx->_dfdx[c->_meta_ids[vjd]]->_evaluated[meta_link] = false;
                                        hess = dfdx->_dfdx[c->_meta_ids[vjd]]->eval_meta(x, c->_meta_coeff, c->_meta_vars, meta_link);
//                                        hess = dfdx->_dfdx[vj->_meta_var[cid]]->eval_meta(x, c->_meta_coeff, c->_meta_vars);
                                    }
//                                }
//                                else {
//                                    assert(false);
//                                    if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0){
//                                        continue;
//                                    }
//                                    if(!dfdx->has_var(vjd)){
//                                        hess = 0;
//                                    }
//                                    else {
//                                        hess = dfdx->eval_dfdx(vjd, x);
//                                    }
//                                }
                            }
                            else {
//                                if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0){
//                                    continue;
//                                }
                                hess = c->get_q_coeff(vid, vjd);
                                if (vid==vjd)
                                    hess *= 2;
                            }
                            if(hess != 0)
                                res[get_hess_index(vi, vjd)] += lambda[cid] * hess;
                        }
                    }
                }
            }
}


void Model::multi_thread_hess(std::map<int, Constraint*>* cons, const double* x , const double* lambda, double* res, int start, int end){
    int vid = 0, vjd = 0, cid = 0;
    var_* vi = NULL;
//    var_* vj = NULL;
    Function* dfdx = NULL;
    Constraint* c = NULL;
    double hess = 0;
    for (cid = start; cid<end; cid++) {
        c = cons->at(cid);
        for (auto& it1:(*c)._vars){
            vid = it1.first;
            if((*c)._hess.count(vid)==0)
                continue;
            vi = it1.second;
            if((*c).has_meta()){
                if ((*c).has_meta()) {
                    dfdx = (*c).get_meta_dfdx(vi);
                }
                else {
                    assert(false);
                    c->compute_dfdx(vi);
                }
            }
            for (auto& it2:vi->_hess){
                vjd = it2;
                if((*c)._hess.count(vjd)==0)
                    continue;
//                vj = it2.second;
                if (vjd<=vid && (*vi)._hess.count(vjd)>0) {
                    if((*c).has_meta()){
                        if ((*c).has_meta()) {
                            if(!(*dfdx).has_var((*c)._meta_ids[vjd])){
                                hess = 0;
                            }
                            else {
                                hess = dfdx->_dfdx[c->_meta_ids[vjd]]->eval_meta(x, c->_meta_coeff, c->_meta_vars, cid);
                            }
                        }
                        else {
                            if((*c)._hess.count(vid)==0 || (*c)._hess.count(vjd)==0){
                                continue;
                            }
                            if(!(*dfdx).has_var(vjd)){
                                hess = 0;
                            }
                            else {
                                hess = (*dfdx).eval_dfdx(vjd, x);
                            }
                        }
                    }
                    else {
                        if((*c)._hess.count(vid)==0 || (*c)._hess.count(vjd)==0){
                            continue;
                        }
                        hess = (*c).get_q_coeff(vid, vjd);
                        if (vid==vjd)
                            hess = hess*2;
                    }
                    if(hess != 0){
                        res[get_hess_index(vi, vjd)] += lambda[cid] * hess;
                    }
                }
            }
        }
    }
}

void Model::fill_in_hess_multithread(const double* x , double obj_factor, const double* lambda, double* res, int start, int end){
    int vid = 0, vjd = 0, cid = 0;
    var_* vi = NULL;
    Function* obj_dfdx = NULL;
    Function* dfdx = NULL;
    Constraint* c = NULL;
    double hess = 0;
    /* return the structure of the hessian */
    for(vid = start; vid < end; ++vid)
    {
        vi = _vars[vid];
        if (_obj->get_type()==nlin_) {
            _obj->compute_dfdx(vi);
        }
        for(auto itv = vi->_hess.cbegin(); itv != vi->_hess.cend(); ++itv)
        {
            vjd = *itv;
            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
                if(_obj->_hess.count(vid)==1 && _obj->_hess.count(vjd)==1) {
                    if(_obj->get_type()==nlin_ && obj_dfdx){
                        if(obj_dfdx->has_var(vjd))
                            hess = obj_dfdx->eval_dfdx(vjd, x);
                        else
                            hess = 0;
                    }
                    else{
                        hess = _obj->get_q_coeff(vid, vjd);
                        if (vid==vjd)
                            hess *= 2;
                    }
                    if(hess != 0)
                        res[get_hess_index(vi, vjd)] = obj_factor * hess;
                }
                else
                    res[get_hess_index(vi, vjd)] = 0;
                    // Constraints part
                for(auto itc = vi->_cstrs.cbegin(); itc != vi->_cstrs.cend(); ++itc)
                {
                    cid = itc->first;
                    c = itc->second;
                    
                    /* fill the constraints portion */
                        //                    if(c->get_ftype()==nlin_ && !c->_exp->is_leaf()){
                    if(c->has_meta()){
                        if (c->has_meta()) {
                            dfdx = c->get_meta_dfdx(vi);
                            if(!dfdx->has_var(c->_meta_ids[vjd])){
                                hess = 0;
                            }
                            else {
                                    //                                if(c->get_idx()==0) {
                                    //                                cout << "accessing stored second derivative" << endl;
                                    //                                cout << "(";
                                    //                                vi->print();
                                    //                                cout << ", ";
                                    //                                getVar(vjd)->print();
                                    //                                cout << "): ";
                                    //                                dfdx->get_dfdx(c->_meta_ids[vjd])->print(false);
                                    //                                    cout << endl;
                                    //                                }
                                hess = dfdx->_dfdx[c->_meta_ids[vjd]]->eval_meta(x, c->_meta_coeff, c->_meta_vars, cid);
                            }
                        }
                        else {
                            if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0)
                                continue;
                            c->compute_dfdx(vi);
                            if(!c->_dfdx[vid]->has_var(vjd)){
                                hess = 0;
                            }
                            else {
                                hess = c->_dfdx[vid]->eval_dfdx(vjd, x);
                            }
                        }
                            //                        hess = dfdx->eval_dfdx(vjd, x);
                            //                        if(!dfdx->has_var(vjd)){
                            //                            hess = 0;
                            //                        }
                            //                        else {
                            //                            if(dfdx->_exp && !dfdx->_exp->is_leaf() && dfdx->_exp->has_var(vjd)){
                            //                                assert(dfdx->_dfdx.count(vjd)==1);
                            //                                cout << "accessing stored derivative" << endl;
                            //                                hess = dfdx->get_dfdx(vjd)->eval(x);
                        
                            //                            }
                            //                            else
                            //                                hess = dfdx->eval_dfdx(vjd, x);
                            //                        }
                    }
                    else {
                        if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0)
                            continue;
                        hess = c->get_q_coeff(vid, vjd);
                        if (vid==vjd)
                            hess *= 2;
                    }
                        //                    hess = dfdx->get_dfdx(vjd)->eval(x);
                    if(hess != 0)
                        res[get_hess_index(vi, vjd)] += lambda[cid] * hess;
                }
            }
        }
        
    }
        //    for (auto it:_cons) {
        //        <#statements#>
        //    }
}

const vector<Constraint*>& Model::get_cons() const {
    return _cons;
}

void Model::fill_in_hess2(const double* x , double obj_factor, const double* lambda, double* res){
    int idx=0;
    int vid = 0, vjd = 0, cid = 0;
//    var_* vi = NULL;
    Function* obj_dfdx = NULL;
    Function* dfdx = NULL;
    Constraint* c = NULL;
    double hess = 0;
    /* return the structure of the hessian */
    for(auto& vi: _vars)
    {
        vid = vi->get_idx();

        if (_obj->get_type()==nlin_) {
            _obj->compute_dfdx(vi);
        }
        for(auto itv = vi->_hess.cbegin(); itv != vi->_hess.cend(); ++itv)
        {
            vjd = *itv;
            if (vjd <= vid) { // This is a symmetric matrix, fill the lower left triangle only.
                if(_obj->_hess.count(vid)==1 && _obj->_hess.count(vjd)==1) {
                    if(_obj->get_type()==nlin_ && obj_dfdx){
                        if(obj_dfdx->has_var(vjd))
                            hess = obj_dfdx->eval_dfdx(vjd, x);
                        else
                            hess = 0;
                    }
                    else{
                        hess = _obj->get_q_coeff(vid, vjd);
                        if (vid==vjd)
                            hess *= 2;
                    }
                    if(hess != 0)
                        res[idx] = obj_factor * hess;
                }
                else
                    res[idx] = 0;
                // Constraints part
                for(auto itc = vi->_cstrs.cbegin(); itc != vi->_cstrs.cend(); ++itc)
                {
                    cid = itc->first;
                    c = itc->second;
                    
                    /* fill the constraints portion */
//                    if(c->get_ftype()==nlin_ && !c->_exp->is_leaf()){
                    if(c->get_ftype()==nlin_){
                        if (c->has_meta()) {
                            dfdx = c->get_meta_dfdx(vi);
                            if(!dfdx->has_var(c->_meta_ids[vjd])){
                                hess = 0;
                            }
                            else {
//                                if(c->get_idx()==0) {
//                                cout << "accessing stored second derivative" << endl;
//                                cout << "(";
//                                vi->print();
//                                cout << ", ";
//                                getVar(vjd)->print();
//                                cout << "): ";
//                                dfdx->get_dfdx(c->_meta_ids[vjd])->print(false);
//                                    cout << endl;
//                                }
                                hess = dfdx->_dfdx[c->_meta_ids[vjd]]->eval_meta(x, c->_meta_coeff, c->_meta_vars, cid);
                            }
                        }
                        else {
                            if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0)
                                continue;
                            c->compute_dfdx(vi);
                            if(!c->_dfdx[vid]->has_var(vjd)){
                                hess = 0;
                            }
                            else {
                                hess = c->_dfdx[vid]->eval_dfdx(vjd, x);
                            }
                        }
//                        hess = dfdx->eval_dfdx(vjd, x);
//                        if(!dfdx->has_var(vjd)){
//                            hess = 0;
//                        }
//                        else {
//                            if(dfdx->_exp && !dfdx->_exp->is_leaf() && dfdx->_exp->has_var(vjd)){
//                                assert(dfdx->_dfdx.count(vjd)==1);
//                                cout << "accessing stored derivative" << endl;
//                                hess = dfdx->get_dfdx(vjd)->eval(x);

//                            }
//                            else
//                                hess = dfdx->eval_dfdx(vjd, x);
//                        }
                    }
                    else {
                        if(c->_hess.count(vid)==0 || c->_hess.count(vjd)==0)
                            continue;
                        hess = c->get_q_coeff(vid, vjd);
                        if (vid==vjd)
                            hess *= 2;
                    }
//                    hess = dfdx->get_dfdx(vjd)->eval(x);
                    if(hess != 0)
                        res[idx] += lambda[cid] * hess;
                }
                idx++;
            }
        }
        
    }
//    for (auto it:_cons) {
//        <#statements#>
//    }
}



void Model::fill_in_grad_obj(const double* x , double* res){
    int idx=0;
//    var_* v = NULL;
    int vid = 0;
    for(auto& vi: _vars)
    {
        vid = vi->get_idx();

        if (!_obj->has_var(vid)) {
            res[idx] = 0;
            idx++;
            continue;
        }
        res[idx] = _obj->eval_dfdx(vid, x);
        idx++;
    }
}


void Model::fill_in_var_init(double* x) {
    var<int>* int_var = NULL;
    var<bool>* bin_var = NULL;
    var<float>* real_var = NULL;
    var<>* long_real_var = NULL;
    int idx=0;
    int vid = 0;
    for(auto& vi: _vars)
    {
        vid = vi->get_idx();

        switch (vi->get_type()) {
            case real:
                real_var = (var<float>*)vi;
                x[idx] = (double)real_var->get_value();
                break;
            case longreal:
                long_real_var = (var<>*)vi;
                x[idx] = long_real_var->get_value();
                break;
            case integ:
                int_var = (var<int>*)vi;
                x[idx] = (double)int_var->get_value();
                break;
            case binary:
                bin_var = (var<bool>*)vi;
                x[idx] = (double)bin_var->get_value();
                break;
            default:
                exit(-1);
                break;
        } ;
        idx++;
    }
}

void Model::fill_in_cstr_bounds(double* g_l ,double* g_u) {
    int idx=0;
    int cid = 0;
//    Constraint* c = NULL;
    for(auto& c : _cons)
    {
        cid = c->get_idx();
        switch (c->get_type()) {
            case eq:
                g_l[idx] = c->_rhs;
                g_u[idx] = c->_rhs;
//                g_l[idx] = 0;
//                g_u[idx] = 0;
                break;
            case leq:
                g_l[idx] = -INFINITY;
                g_u[idx] = c->_rhs;
//                g_u[idx] = -c->get_const();
//                cout << "gl[idx] = " <<g_l[idx];
//                cout << ", gu[idx] = " <<g_u[idx] << endl;
                break;
            case geq:
                g_l[idx] = c->_rhs;
//                g_l[idx] = -c->get_const();
                g_u[idx] = INFINITY;
                break;                
                
            default:
                exit(-1);
                break;
        }
        idx++;
    }
}

void Model::print_functions() const{
    cout << " Number of atomic functions = " << _functions.size();
    cout << endl;
//    for (auto& f: _functions){
//        f->print(false);
//        cout << endl;
//    }
//    cout << endl;
}

void Model::print_solution() const{
    var<int>* int_var = NULL;
    var<bool>* bin_var = NULL;
    var<float>* real_var = NULL;
    var<double>* long_real_var = NULL;
    int idx=0;
    int vid = 0;
    for(auto& v: _vars ) {
        vid = v->get_idx();
//        assert(vid==idx);
        switch (v->get_type()) {
            case real:
                real_var = (var<float>*)v;
                real_var->print();
                break;
            case longreal:
                long_real_var = (var<double>*)v;
                long_real_var->print();
                break;
            case integ:
                int_var = (var<int>*)v;
                int_var->print();
                break;
            case binary:
                bin_var = (var<bool>*)v;
                bin_var->print();
                break;
            default:
                break;
        } ;
        idx++;
    }
}


