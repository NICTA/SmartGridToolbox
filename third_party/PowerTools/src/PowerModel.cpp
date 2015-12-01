//
//  PowerModel.cpp
//  PowerTools++
//
//  Created by Hassan Hijazi on 30/01/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#include "PowerTools++/PowerModel.h"

PowerModel::PowerModel():PowerModel(ACPOL, new Net(), ipopt){};

PowerModel::PowerModel(PowerModelType type, Net* net, SolverType stype):_type(type), _net(net), _stype(stype){
    _model = NULL;
};


PowerModel::~PowerModel(){
    delete _model;
    delete _solver;
    //    delete _net;
};


void PowerModel::build(){
    _model = new Model();
    _solver = new Solver(_model, _stype);
    switch (_type) {
        case ACPOL:
            add_AC_Pol_vars();
            post_AC_Polar();
            break;
        case ACRECT:
            add_AC_Rect_vars();
            post_AC_Rect();
            break;
        case QC:
            add_QC_vars();
            post_QC();
            break;
        case QC_SDP:
            add_QC_vars();
            post_QC();
            add_SDP_cuts(3);
            break;
        case OTS:
            add_AC_OTS_vars();
            post_AC_OTS();
            break;
        case SOCP:
            add_AC_SOCP_vars();
            post_AC_SOCP();
            break;
        case SDP:
            add_AC_SOCP_vars();
            post_AC_SOCP();
            add_SDP_cuts(3);
            break;
        case DC:
            add_DC_vars();
            post_DC();
            break;
        case QC_OTS_L:
            add_QC_OTS_vars();
            post_QC_OTS(true,false);
            break;
        case QC_OTS_N:
            add_QC_OTS_vars();
            post_QC_OTS(true,true);
            break;
        case QC_OTS_O:
            add_QC_OTS_vars();
            post_QC_OTS(false,true);
            break;
        case SOCP_OTS:
            add_SOCP_OTS_vars();
            post_SOCP_OTS();
            break;
        case GRB_TEST:
            //run_grb_lin_test();
            run_grb_quad_test();
            break;
        default:
            break;
    }
}

void PowerModel::reset(){
    delete _solver;
    delete _model;
    _solver = NULL;
    _model = NULL;
};

void PowerModel::run_grb_lin_test(){
    var<bool> x, y, z;
    x.init("x",0.0,1.0);
    y.init("y",0.0,1.0);
    z.init("z",0.0,1.0);
    
    _model->addVar(x);
    _model->addVar(y);
    _model->addVar(z);
    
    Constraint C1("C1");
    C1 += x + 2*y + 3*z;
    C1 <= 4;
    _model->addConstraint(C1);
    
    Constraint C2("C2");
    C2 += x + y;
    C2 >= 1;
    _model->addConstraint(C2);
    
    Function* obj = new Function();
    *obj += x + y + 2*z;
    _model->setObjective(obj);
    solve();
}

void PowerModel::run_grb_quad_test(){
    var<double> x, y, z;
    x.init("x",0.0,1.0);
    y.init("y",0.0,1.0);
    z.init("z",0.0,1.0);
    
    _model->addVar(x);
    _model->addVar(y);
    _model->addVar(z);
    
    Constraint C1("C1");
    C1 += x + 2*y + 3*z;
    C1 >= 4;
    _model->addConstraint(C1);
    
    Constraint C2("C2");
    C2 += x + y;
    C2 >= 1;
    _model->addConstraint(C2);
    
    Function* obj = new Function();
    *obj += x*x + x*y + y*y + y*z + z*z + 2*x;
    _model->setObjective(obj);
    solve();
}


/** Accessors */
Function* PowerModel::objective(){
    return _model->_obj;
}

/** Objective Functions */
void PowerModel::min_cost(){
    _objective = MinCost;
    Function* obj = new Function();
    for (auto g:_net->gens) {
        if (!g->_active)
            continue;
        *obj += _net->bMVA*g->_cost->c1*(g->pg) + pow(_net->bMVA,2)*g->_cost->c2*(g->pg^2) + g->_cost->c0;
    }
    _model->setObjective(obj);
    _model->setObjectiveType(minimize); // currently for gurobi
    //    obj->print(true);
    //    _solver->run();
}

void PowerModel::min_var(var<> v){
    _objective = MinDelta;
    Function* obj = new Function();
    *obj += v;
    _model->setObjective(obj);
    _model->setObjectiveType(minimize);
}

void PowerModel::max_var(var<> v){
    _objective = MaxDelta;
    Function* obj = new Function();
    *obj += v;
    _model->setObjective(obj);
    _model->setObjectiveType(maximize);
}



int PowerModel::solve(int output, bool relax){
    return _solver->run(output,relax);
}


void PowerModel::add_AC_gen_vars(){
    for (auto g:_net->gens) {
        if (!g->_active)
            continue;
        g->pg.init("pg"+g->_name+":"+g->_bus->_name, g->pbound.min, g->pbound.max);
//        g->pg = g->ps;
        g->pg = g->pbound.max;
        _model->addVar(g->pg);
        g->qg.init("qg"+g->_name+":"+g->_bus->_name, g->qbound.min, g->qbound.max);
//        g->qg = g->qs;
        _model->addVar(g->qg);
        g->init_complex();
    }
}

void PowerModel::add_AC_Pol_vars(){
    for (auto n:_net->nodes) {
        n->theta.init("t"+n->_name);
        n->v.init("v"+n->_name, n->vbound.min, n->vbound.max);
        n->v = n->vs;
//                n->v = 1;
        _model->addVar(n->v);
        _model->addVar(n->theta);
        n->init_complex(true);
        
    }
    add_AC_gen_vars();
    for (auto a:_net->arcs) {
        if (a->status==0) {
            continue;
        }
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->qi.init("q("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        a->qj.init("q("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->pi);
        _model->addVar(a->pj);
        _model->addVar(a->qi);
        _model->addVar(a->qj);
        a->init_complex();
    }
}

void PowerModel::add_AC_Rect_vars(){
    for (auto a:_net->arcs) {
        if (a->status==0) {
            continue;
        }
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        _model->addVar(a->pi);
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->pj);
        a->qi.init("q("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        _model->addVar(a->qi);
        a->qj.init("q("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->qj);
        a->init_complex();
    }
    for (auto n:_net->nodes) {
        n->vr.init("vr"+n->_name, -n->vbound.max, n->vbound.max);
        n->vr = n->vs;
//                n->vr = 1;
        _model->addVar(n->vr);
        n->vi.init("vi"+n->_name, -n->vbound.max, n->vbound.max);
        _model->addVar(n->vi);
        n->init_complex(false);
        
    }
    add_AC_gen_vars();
}

void PowerModel::add_QC_vars(){
    for (auto n:_net->nodes) {
        n->theta.init("t"+n->_name);
        n->v.init("v"+n->_name, n->vbound.min, n->vbound.max);
        n->v = n->vs;
        n->w.init("w"+n->_name,pow(n->vbound.min,2), pow(n->vbound.max,2));
        n->w = n->vs*n->vs;
        //        n->v = 1;
        _model->addVar(n->v);
        _model->addVar(n->theta);
        _model->addVar(n->w);
        n->init_lifted_complex();
    }
    add_AC_gen_vars();
    
    for (auto a:_net->arcs) { // vivjcos, vivjsin, vivj, cos, sin, c
        if (a->status==0) {
//            assert(false);
            continue;
        }
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->qi.init("q("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        a->qj.init("q("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->pi);
        _model->addVar(a->pj);
        _model->addVar(a->qi);
        _model->addVar(a->qj);
        if (a->tbound.min < 0 && a->tbound.max > 0)
            a->cs.init("cs("+a->_name+","+a->src->_name+","+a->dest->_name+")",min(cos(a->tbound.min), cos(a->tbound.max)), 1.);
        else
            a->cs.init("cs("+a->_name+","+a->src->_name+","+a->dest->_name+")",min(cos(a->tbound.min), cos(a->tbound.max)), max(cos(a->tbound.min),cos(a->tbound.max)));
        a->vv.init("vv("+a->_name+","+a->src->_name+","+a->dest->_name+")",a->src->vbound.min*a->dest->vbound.min,a->src->vbound.max*a->dest->vbound.max);
        a->vcs.init("vcs("+a->_name+","+a->src->_name+","+a->dest->_name+")",a->vv.get_lb()*a->cs.get_lb(), a->vv.get_ub()*a->cs.get_ub());
        if(a->tbound.min < 0 && a->tbound.max > 0)
            a->vsn.init("vsn("+a->_name+","+a->src->_name+","+a->dest->_name+")",a->vv.get_ub()*sin(a->tbound.min), a->vv.get_ub()*sin(a->tbound.max));
        if (a->tbound.min >= 0)
            a->vsn.init("vsn("+a->_name+","+a->src->_name+","+a->dest->_name+")",a->vv.get_lb()*sin(a->tbound.min), a->vv.get_ub()*sin(a->tbound.max));
        if (a->tbound.max <= 0)
            a->vsn.init("vsn("+a->_name+","+a->src->_name+","+a->dest->_name+")",a->vv.get_ub()*sin(a->tbound.min), a->vv.get_lb()*sin(a->tbound.max));
        a->sn.init("sn("+a->_name+","+a->src->_name+","+a->dest->_name+")",sin(a->tbound.min), sin(a->tbound.max));
        a->ci.init("ci("+a->_name+","+a->src->_name+","+a->dest->_name+")", 0.0, INFINITY);
        a->delta.init("delta("+a->_name+","+a->src->_name+","+a->dest->_name+")",a->tbound.min,a->tbound.max);
        _model->addVar(a->vcs);
        a->vcs = 1;
        _model->addVar(a->vsn);
        _model->addVar(a->vv);
        _model->addVar(a->cs);
        _model->addVar(a->sn);
        _model->addVar(a->ci);
        _model->addVar(a->delta);
        a->init_complex();
    }
    
}

void PowerModel::add_AC_OTS_vars() {
    add_AC_gen_vars();
    for (auto n:_net->nodes) { //
        n->theta.init("t"+n->_name);
        n->v.init("v"+n->_name, n->vbound.min, n->vbound.max);
        //n->v = n->vs;
        n->v = 1;
        _model->addVar(n->v);
        _model->addVar(n->theta);
        n->init_complex(true);
    }
    for (auto a:_net->arcs) {
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->qi.init("q("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        a->qj.init("q("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        a->on.init("on("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->pi);
        _model->addVar(a->pj);
        _model->addVar(a->qi);
        _model->addVar(a->qj);
        _model->addVar(a->on);
        a->on=1;
        a->init_complex();
    }
}

void PowerModel::add_QC_OTS_vars() {
    double m_theta_ub = 0, m_theta_lb = 0;
    for (auto a:_net->arcs) {
        m_theta_ub += a->tbound.max;
        m_theta_lb += a->tbound.min;
    }
    _net->m_theta_ub = m_theta_ub;
    _net->m_theta_lb = m_theta_lb;
    add_AC_gen_vars();
    for (auto n:_net->nodes) {
        n->v.init("v"+n->_name, n->vbound.min, n->vbound.max);
        n->v = 1;
        n->theta.init("t"+n->_name);
        n->w.init("w"+n->_name, pow(n->vbound.min,2), pow(n->vbound.max,2));
        _model->addVar(n->v);
        //n->w = pow(n->vs,2);
        n->w = 1.001;
        _model->addVar(n->w);
        n->theta = 0;
        _model->addVar(n->theta);
        n->init_lifted_complex();
    }
    
    for (auto a:_net->arcs) {
        if (a->status == 0) continue;
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->qi.init("q("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        a->qj.init("q("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        a->on.init("on("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        
        a->vv.init("vv("+a->_name+","+a->src->_name+","+a->dest->_name+")",a->src->vbound.min*a->dest->vbound.min,a->src->vbound.max*a->dest->vbound.max,0,0);
        if (a->tbound.min < 0 && a->tbound.max > 0)
            a->cs.init("cs("+a->_name+","+a->src->_name+","+a->dest->_name+")",min(cos(a->tbound.min),cos(a->tbound.max)),1,0,0);
        else
            a->cs.init("cs("+a->_name+","+a->src->_name+","+a->dest->_name+")",min(cos(a->tbound.min),cos(a->tbound.max)),max(cos(a->tbound.min),cos(a->tbound.max)),0,0);
        a->wr.init("wr"+a->_name, a->vv.get_lb()*a->cs.get_lb(), a->vv.get_ub()*a->cs.get_ub(),0,0);
        if(a->tbound.min < 0 && a->tbound.max > 0)
            a->wi.init("wi"+a->_name, a->vv.get_ub()*sin(a->tbound.min), a->vv.get_ub()*sin(a->tbound.max),0,0);
        if (a->tbound.min >= 0)
            a->wi.init("wi"+a->_name, a->vv.get_lb()*sin(a->tbound.min), a->vv.get_ub()*sin(a->tbound.max),0,0);
        if (a->tbound.max <= 0)
            a->wi.init("wi"+a->_name, a->vv.get_ub()*sin(a->tbound.min), a->vv.get_lb()*sin(a->tbound.max),0,0);
        a->w_line_ij.init("w_line("+a->_name+","+a->src->_name+","+a->dest->_name+")", pow(a->src->vbound.min,2), pow(a->src->vbound.max,2),0,0);
        a->w_line_ji.init("w_line("+a->_name+","+a->dest->_name+","+a->src->_name+")", pow(a->dest->vbound.min,2), pow(a->dest->vbound.max,2),0,0);
        
        a->sn.init("sn("+a->_name+","+a->src->_name+","+a->dest->_name+")",sin(a->tbound.min), sin(a->tbound.max),0,0);
        a->ci.init("ci("+a->_name+","+a->src->_name+","+a->dest->_name+")", 0, INFINITY);
        a->delta.init("delta("+a->_name+","+a->src->_name+","+a->dest->_name+")", a->tbound.min, a->tbound.max, _net->m_theta_lb, _net->m_theta_ub);
        _model->addVar(a->pi);
        _model->addVar(a->pj);
        _model->addVar(a->qi);
        _model->addVar(a->qj);
        a->on = 1;
        _model->addVar(a->on);
        a->wr = 1;
        _model->addVar(a->wr);
        a->wi = 0;
        _model->addVar(a->wi);
        _model->addVar(a->w_line_ij);
        _model->addVar(a->w_line_ji);
        _model->addVar(a->vv);
        _model->addVar(a->cs);
        _model->addVar(a->sn);
        _model->addVar(a->ci);
        _model->addVar(a->delta);
        a->init_complex();
    }
}

void PowerModel::post_QC_OTS(bool lin_cos_cuts, bool quad_cos){
    cout << "QC model not available in the open source version of PowerTools, please contact us to get a license.\n";
    exit(-1);
}

void PowerModel::propagate_bounds(){
    if (_model != NULL) {
        cerr << "\npropagate_bounds should be called before building the model.";
        exit(1);
    }
    var<double>* delta;
    var<double>* voltage;
    double tol = 0.01;
    double* tol_delta_l = new double[_net->arcs.size()];
    double* tol_delta_u = new double[_net->arcs.size()];
    double* tol_v_l = new double[_net->nodes.size()];
    double* tol_v_u = new double[_net->nodes.size()];
    double maxtol = 1;
    int i = 0;
    fill_n(tol_delta_l, _net->arcs.size(), 1);
    fill_n(tol_delta_u, _net->arcs.size(), 1);
    fill_n(tol_v_l, _net->nodes.size(), 1);
    fill_n(tol_v_u, _net->nodes.size(), 1);

    _solver = new Solver(NULL,_stype);
    
    while(maxtol > tol) {
        cout << "\nmaxtol = " << maxtol << "\n";
        _model = new Model();
        //        _solver = new Solver(_model,_stype);
        _solver->set_model(_model);
        maxtol = 0;
        //        add_QC_OTS_vars();
        //        if (_type == QC_OTS_N) post_QC_OTS(true,true);
        //        if (_type == QC_OTS_O) post_QC_OTS(false,true);
        //        if (_type == QC_OTS_L) post_QC_OTS(true,false);
        switch (_type) {
            case QC:
                cout << "QC model not available in the open source version of PowerTools, please contact us to get a license.\n";
                exit(-1);
                break;
            case QC_OTS_L:
                cout << "QC model not available in the open source version of PowerTools, please contact us to get a license.\n";
                exit(-1);
                break;
            case QC_OTS_N:
                cout << "QC model not available in the open source version of PowerTools, please contact us to get a license.\n";
                exit(-1);
                break;
            case QC_OTS_O:
                cout << "QC model not available in the open source version of PowerTools, please contact us to get a license.\n";
                exit(-1);
                break;
            default:
                cerr << "\nBound propagation is not supported for this model yet\n";
                return;
                break;
        }
        
        i = 0;
        for (auto& a: _net->arcs) {
            //if (!a->src->_has_gen && !a->dest->_has_gen) {i++; continue;}
            if (tol_delta_l[i] > tol) {
                if(_type==QC_OTS_L || _type==QC_OTS_N || _type==QC_OTS_O || _type==SOCP_OTS)
                    a->on.set_lb(true);
                min_var(a->delta);
                solve(0,true);
                delta = _model->getVar_<double>(a->delta.get_idx());
                a->tbound.min = delta->get_value();
                tol_delta_l[i] = a->tbound.min-delta->get_lb();
                if (tol_delta_l[i] > maxtol) maxtol = tol_delta_l[i];
                if(_type==QC_OTS_L || _type==QC_OTS_N || _type==QC_OTS_O || _type==SOCP_OTS)
                    a->on.set_lb(false);
            }
            if(tol_delta_u[i]>tol) {
                if(_type==QC_OTS_L || _type==QC_OTS_N || _type==QC_OTS_O || _type==SOCP_OTS)
                    a->on.set_lb(true);
                max_var(a->delta);
                solve(0,true);
                delta = _model->getVar_<double>(a->delta.get_idx());
                a->tbound.max = delta->get_value();
                tol_delta_u[i] = delta->get_ub()-a->tbound.max;
                if (tol_delta_u[i] > maxtol) maxtol = tol_delta_u[i];
                if(_type==QC_OTS_L || _type==QC_OTS_N || _type==QC_OTS_O || _type==SOCP_OTS)
                    a->on.set_lb(false);
            }
            i++;
        }
        
        i = 0;
        for(auto& n: _net->nodes){
            //            if (!n->_has_gen) {i++; continue;}
            if(tol_v_l[i] > tol) {
                min_var(n->v);
                solve(0,true);
                voltage = _model->getVar_<double>(n->v.get_idx());
                n->vbound.min = voltage->get_value();
                tol_v_l[i] = n->vbound.min-voltage->get_lb();
                if (tol_v_l[i] > maxtol) maxtol = tol_v_l[i];
            }
            if(tol_v_u[i] > tol) {
                max_var(n->v);
                solve(0,true);
                voltage = _model->getVar_<double>(n->v.get_idx());
                n->vbound.max = voltage->get_value();
                tol_v_u[i] = n->vbound.max-voltage->get_ub();
                if (tol_v_u[i] > maxtol) maxtol = tol_v_u[i];
            }
            i++;
        }
        
        cout << endl << "New bounds, maxtol = " << maxtol << ":";
        for (auto& a: _net->arcs) {
            cout << "\nmin = " << a->tbound.min << " max = " << a->tbound.max;
        }
        for (auto& n: _net->nodes) {
            cout << "\nvmin = " << n->vbound.min << " vmax = " << n->vbound.max;
        }
        delete _model;
    }
    cout << endl << "New bounds: ";
    i = 0;
    for (auto& a: _net->arcs) {
        cout << "\nmin = " << a->tbound.min << " max = " << a->tbound.max;
        i++;
    }
    delete[] tol_delta_l;
    delete[] tol_delta_u;
    delete[] tol_v_l;
    delete[] tol_v_u;
 
}

void PowerModel::post_QC(){
    cout << "QC model not available in the open source version of PowerTools, please contact us to get a license.\n";
    exit(-1);
}


void PowerModel::add_AC_SOCP_vars(){
    string key, src, dest;
    set<string> parallel;
//    Arc* ap = nullptr;
    for (auto a:_net->arcs) {
        if (a->status==0) {
            continue;
        }
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        _model->addVar(a->pi);
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->pj);
        a->qi.init("q("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        _model->addVar(a->qi);
        a->qj.init("q("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->qj);
//        ap = *(_net->lineID[a->src->_name+","+a->dest->_name]->begin());
//            ap->wr.init("wr"+a->_name, a->src->vbound.min*a->dest->vbound.min*cos(a->tbound.min), a->src->vbound.max*a->dest->vbound.max);
//            ap->wr = 1;
//            _model->addVar(ap->wr);
//            ap->wi.init("wi"+a->_name, -a->src->vbound.max*a->dest->vbound.max*sin(a->tbound.max), a->src->vbound.max*a->dest->vbound.max*sin(a->tbound.max));
//            _model->addVar(ap->wi);
        if (!a->parallel) {
            a->wr.init("wr"+a->_name, a->src->vbound.min*a->dest->vbound.min*cos(a->tbound.min), a->src->vbound.max*a->dest->vbound.max);
            a->wr = 1;
            _model->addVar(a->wr);
            a->wi.init("wi"+a->_name, -a->src->vbound.max*a->dest->vbound.max*sin(a->tbound.max), a->src->vbound.max*a->dest->vbound.max*sin(a->tbound.max));
            _model->addVar(a->wi);
        }
//        else {
//            cout << "avoiding parallel line\n";
//        }
        a->init_complex();
        
    }
    for (auto n:_net->nodes) {
        n->w.init("w"+n->_name, pow(n->vbound.min,2), pow(n->vbound.max,2));
//        n->w = pow(n->vs,2);
        n->w = 1.001;
        _model->addVar(n->w);
        n->init_lifted_complex();
    }
    add_AC_gen_vars();
}

void PowerModel::add_DC_vars(){
    for (auto n:_net->nodes) {
        n->theta.init("t"+n->_name);
        _model->addVar(n->theta);
        
    }
    for (auto g:_net->gens) {
        if (!g->_active)
            continue;
        g->pg.init("pg"+g->_bus->_name, g->pbound.min, g->pbound.max);
        _model->addVar(g->pg);
    }
    for (auto a:_net->arcs) {
        if (a->status==0)
            continue;
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->pi);
        _model->addVar(a->pj);
    }
}

void PowerModel::add_SOCP_OTS_vars() {
    for (auto a:_net->arcs) {
        a->pi.init("p("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        _model->addVar(a->pi);
        a->pj.init("p("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->pj);
        a->qi.init("q("+a->_name+","+a->src->_name+","+a->dest->_name+")");
        _model->addVar(a->qi);
        a->qj.init("q("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        _model->addVar(a->qj);
        a->wr.init("wr"+a->_name);
        _model->addVar(a->wr);
        a->wi.init("wi"+a->_name);
        _model->addVar(a->wi);
        a->on.init("on("+a->_name+","+a->dest->_name+","+a->src->_name+")");
        a->on = true;
        _model->addVar(a->on);
        a->w_line_ij.init("w_line("+a->_name+","+a->src->_name+","+a->dest->_name+")", 0.0, INFINITY);
        _model->addVar(a->w_line_ij);
        a->w_line_ji.init("w_line("+a->_name+","+a->dest->_name+","+a->src->_name+")", 0.0, INFINITY);
        _model->addVar(a->w_line_ji);
        a->init_complex();
    }
    for (auto n:_net->nodes) {
        n->w.init("w"+n->_name, pow(n->vbound.min,2), pow(n->vbound.max,2));
        n->w = pow(n->vs,2);
        _model->addVar(n->w);
        n->init_lifted_complex();
    }
    add_AC_gen_vars();
}

//#subject to Theta_Delta_UB {(l,i,j) in arcs_from}: wi[l] <= tan(ad_max[l])*wr[l];
//#subject to Theta_Delta_LB {(l,i,j) in arcs_from}: wi[l] >= tan(ad_min[l])*wr[l];
void PowerModel::add_Wr_Wi(Arc *a){
    if (a->status==1 && !a->parallel) {
        Constraint Wr_Wi_l("Theta_Delta_UB");
        Wr_Wi_l += a->wi;
        Wr_Wi_l -= tan(a->tbound.max)*a->wr;
        Wr_Wi_l <= 0;
        _model->addConstraint(Wr_Wi_l);
        Constraint Wr_Wi_u("Theta_Delta_UB");
        Wr_Wi_u += a->wi;
        Wr_Wi_u -= tan(a->tbound.min)*a->wr;
        Wr_Wi_u >= 0;
        _model->addConstraint(Wr_Wi_u);        
    }
}

void PowerModel::add_AC_thermal(Arc* a, bool switch_lines){
    /** subject to Thermal_Limit {(l,i,j) in arcs}: p[l,i,j]^2 + q[l,i,j]^2 <= s[l]^2;*/
    if (a->status==1 || switch_lines) {
        Constraint Thermal_Limit_from("Thermal_Limit_from");
        Thermal_Limit_from += a->_Si_.square_magnitude();
        if (!switch_lines){
            Thermal_Limit_from <= pow(a->limit,2.);
        }
        else{
            //Thermal_Limit_from -= (0+a->on)*pow(a->limit,2.) + (1-a->on)*a->smax;
            Thermal_Limit_from -= (0+a->on)*(pow(a->limit,2.)-a->smax) + a->smax;
            Thermal_Limit_from <= 0;
        }
        _model->addConstraint(Thermal_Limit_from);
        Constraint Thermal_Limit_to("Thermal_Limit_to");
        Thermal_Limit_to += a->_Sj_.square_magnitude();
        if (!switch_lines){
            Thermal_Limit_to <= pow(a->limit,2.);
        }
        else{
            //Thermal_Limit_to -= (0+a->on)*pow(a->limit,2.) + (1-a->on)*a->smax;
            Thermal_Limit_to -= (0+a->on)*(pow(a->limit,2.)-a->smax) + a->smax;
            Thermal_Limit_to <= 0;
        }
        _model->addConstraint(Thermal_Limit_to);
    }
}


void PowerModel::add_AC_Angle_Bounds(Arc* a, bool switch_lines){
    /** Adding phase angle difference bound constraint
     subject to Theta_Delta_LB{(l,i,j) in arcs_from}: t[i]-t[j] >= -theta_bound;
     subject to Theta_Delta_UB{(l,i,j) in arcs_from}: t[i]-t[j] <= theta_bound;
     */
    if (a->status==1 || switch_lines) {
        Node* src = a->src;
        Node* dest = a->dest;
        Constraint Theta_Delta_UB("Theta_Delta_UB");
        Theta_Delta_UB += (src->theta - dest->theta);
        if (!switch_lines){
            Theta_Delta_UB <= a->tbound.max;
        }
        else{
            Theta_Delta_UB -= a->tbound.max*a->on + (1-a->on)*_net->m_theta_ub;
            //Theta_Delta_UB -= (0+a->on)*(a->tbound.max-_net->m_theta_ub) + _net->m_theta_ub;
            Theta_Delta_UB <= 0;
        }
        _model->addConstraint(Theta_Delta_UB);
        Constraint Theta_Delta_LB("Theta_Delta_LB");
        Theta_Delta_LB += src->theta - dest->theta;
        if (!switch_lines){
            Theta_Delta_LB >= a->tbound.min;
        }
        else{
            //Theta_Delta_LB -= a->tbound.min*a->on + (1-a->on)*_net->m_theta_lb;
            Theta_Delta_LB -= (0+a->on)*(a->tbound.min-_net->m_theta_lb) + _net->m_theta_lb;
            Theta_Delta_LB >= 0;
        }
        _model->addConstraint(Theta_Delta_LB);
    }
}


void PowerModel::add_AC_Power_Flow(Arc *a, bool polar){
    
    
    //    Flow_P_From += a->pi;
    //    Flow_P_From -= constant(a->g/pow(a->tr,2.))*(src->_V_.square_magnitude());
    //    Flow_P_From += constant(a->g/a->tr)*((src->v)*(dest->v)*cos(src->theta - dest->theta - a->as));
    //    Flow_P_From += a->b/a->tr*((src->v)*(dest->v)*sin(src->theta - dest->theta - a->as));
    //    Flow_P_From = 0;
    
    if (a->status==1) {
        /** subject to Flow_P_From {(l,i,j) in arcs_from}:
         p[l,i,j] = g[l]*(v[i]/tr[l])^2
         + -g[l]*v[i]/tr[l]*v[j]*cos(t[i]-t[j]-as[l])
         + -b[l]*v[i]/tr[l]*v[j]*sin(t[i]-t[j]-as[l]);
         */
        Node* src = a->src;
        Node* dest = a->dest;
        Constraint Flow_P_From("Flow_P_From"+a->pi._name);
        Flow_P_From += a->pi;
        Flow_P_From -= a->g*(src->_V_.square_magnitude())/pow(a->tr,2.);
        if (polar) {
            Flow_P_From += a->g/a->tr*((src->v)*(dest->v)*cos(src->theta - dest->theta - a->as)); /** TODO write the constraints in Complex form */
            Flow_P_From += a->b/a->tr*((src->v)*(dest->v)*sin(src->theta - dest->theta - a->as));
        }
        else{
            Flow_P_From -= (-a->g*a->cc + a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(src->vr*dest->vr + src->vi*dest->vi);
            Flow_P_From -= (-a->b*a->cc - a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(src->vi*dest->vr - src->vr*dest->vi);
        }
        Flow_P_From = 0;
        _model->addConstraint(Flow_P_From);
        /** subject to Flow_P_To {(l,i,j) in arcs_to}:
         p[l,i,j] = g[l]*v[i]^2
         + -g[l]*v[i]*v[j]/tr[l]*cos(t[i]-t[j]+as[l])
         + -b[l]*v[i]*v[j]/tr[l]*sin(t[i]-t[j]+as[l]);
         */
        Constraint Flow_P_To("Flow_P_To"+a->pj._name);
        Flow_P_To += a->pj;
        Flow_P_To -= a->g*(dest->_V_.square_magnitude());
        if (polar) {
            Flow_P_To += a->g/a->tr*(dest->v*src->v*cos(dest->theta - src->theta + a->as));
            Flow_P_To += a->b/a->tr*(dest->v*src->v*sin(dest->theta - src->theta + a->as));
        }
        else {
            Flow_P_To -= (-a->g*a->cc - a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(dest->vr*src->vr + dest->vi*src->vi);
            Flow_P_To -= (-a->b*a->cc + a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(dest->vi*src->vr - dest->vr*src->vi);
        }
        Flow_P_To = 0;
        _model->addConstraint(Flow_P_To);
        /** subject to Flow_Q_From {(l,i,j) in arcs_from}:
         q[l,i,j] = -(charge[l]/2+b[l])*(v[i]/tr[l])^2
         +  b[l]*v[i]/tr[l]*v[j]*cos(t[i]-t[j]-as[l])
         + -g[l]*v[i]/tr[l]*v[j]*sin(t[i]-t[j]-as[l]);
         */
        Constraint Flow_Q_From("Flow_Q_From"+a->qi._name);
        Flow_Q_From += a->qi;
        Flow_Q_From += (a->ch/2+a->b)*(src->_V_.square_magnitude())/pow(a->tr,2.);
        if (polar) {
            Flow_Q_From -= a->b/a->tr*(src->v*dest->v*cos(src->theta - dest->theta - a->as));
            Flow_Q_From += a->g/a->tr*(src->v*dest->v*sin(src->theta - dest->theta - a->as));
        }
        else {
            Flow_Q_From += (-a->b*a->cc - a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(dest->vr*src->vr + dest->vi*src->vi);
            Flow_Q_From -= (-a->g*a->cc + a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(src->vi*dest->vr - src->vr*dest->vi);
        }
        Flow_Q_From = 0;
        _model->addConstraint(Flow_Q_From);
        /** subject to Flow_Q_To {(l,i,j) in arcs_to}:
         q[l,i,j] = -(charge[l]/2+b[l])*v[i]^2
         +  b[l]*v[i]*v[j]/tr[l]*cos(t[i]-t[j]+as[l])
         + -g[l]*v[i]*v[j]/tr[l]*sin(t[i]-t[j]+as[l]);
         */
        Constraint Flow_Q_To("Flow_Q_To"+a->qj._name);
        Flow_Q_To += a->qj;
        Flow_Q_To += (a->ch/2+a->b)*(dest->_V_.square_magnitude());
        if (polar) {
            Flow_Q_To -= a->b/a->tr*(dest->v*src->v*cos(dest->theta - src->theta + a->as));
            Flow_Q_To += a->g/a->tr*(dest->v*src->v*sin(dest->theta - src->theta + a->as));
        }
        else{
            Flow_Q_To += (-a->b*a->cc + a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(dest->vr*src->vr + dest->vi*src->vi);
            Flow_Q_To -= (-a->g*a->cc - a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*(dest->vi*src->vr - dest->vr*src->vi);
        }
        Flow_Q_To = 0;
        _model->addConstraint(Flow_Q_To);
    }
    
}

void PowerModel::add_AC_KCL(Node* n, bool switch_lines){
    /** subject to KCL_P {i in buses}: sum{(l,i,j) in arcs} p[l,i,j] + shunt_g[i]*v[i]^2 + load_p[i] = sum{(i,gen) in bus_gen} pg[gen];
     subject to KCL_Q {i in buses}: sum{(l,i,j) in arcs} q[l,i,j] - shunt_b[i]*v[i]^2 + load_q[i] = sum{(i,gen) in bus_gen} qg[gen];
     */
    Constraint KCL_P("KCL_P");
    KCL_P += sum(n->get_out(), "pi",switch_lines);
    KCL_P += sum(n->get_in(), "pj",switch_lines);
    KCL_P -= sum(n->_gen, "pg");
    KCL_P += n->gs()*(n->_V_.square_magnitude()) + n->pl();
    KCL_P = 0;
    _model->addConstraint(KCL_P);
    
    Constraint KCL_Q("KCL_Q");
    KCL_Q += sum(n->get_out(), "qi",switch_lines);
    KCL_Q += sum(n->get_in(), "qj",switch_lines);
    KCL_Q -= sum(n->_gen, "qg");
    KCL_Q -= n->bs()*(n->_V_.square_magnitude()) - n->ql();
    KCL_Q = 0;
    _model->addConstraint(KCL_Q);
}


//void PowerModel::post_AC_Polar(){
//    for (auto a:_net->arcs) {
//        if (a->status==0) {
//            continue;
//        }
//        add_AC_Angle_Bounds(a);
//        add_AC_Power_Flow(a, true);
//        add_AC_thermal(a);
//    }
//    for (auto n:_net->nodes) {
//        add_AC_KCL(n);
//    }
//
//}

void PowerModel::add_Cosine(Arc *a){
    double *x = new double[_model->get_nb_vars()];
    double theta_u;
    double Ml = _net->m_theta_lb;
    double Mu = _net->m_theta_ub;
    double l = a->tbound.min;
    double u = a->tbound.max;
    theta_u = max(-l,u);
    Function CSR; // cs(theta,u) if theta > 0
    double alpha = (1-cos(theta_u))/pow(theta_u,2);
    double on_cont;
    
    //    Function Cs_r;
    //    Cs_r += a->delta - Mu*(1-a->on) - sqrt((a->on*a->on-a->cs*a->on)/alpha);
    //    on_cont = 0.5;
    //    CSR += on_cont - alpha*(a->delta*a->delta - 2*Mu*a->delta*(1-on_cont) + pow(Mu,2)*(1-on_cont)*(1-on_cont))/on_cont;
    //    x[a->on.get_idx()] = on_cont;
    //    x[a->delta.get_idx()] = u;
    //    x[a->delta.get_idx()] = Mu - Mu*on_cont + u*on_cont/2;
    //    x[a->cs.get_idx()] = CSR.eval(x);
    //    Constraint CS_UB_OAR("CS_UB_OAR");
    //    CS_UB_OAR += Cs_r.outer_approx(x);
    //    CS_UB_OAR <= 0;
    //    _model->addConstraint(CS_UB_OAR);
    
    //    Function CSL; // cs(theta,u) if theta < 0
    //    Function Cs_l;
    //    Cs_l += Ml*(1-a->on) - a->delta - sqrt((a->on*a->on-a->cs*a->on)/alpha);
    //    on_cont = 0.5;
    //    CSL += on_cont - alpha*(a->delta*a->delta - 2*Ml*a->delta*(1-on_cont) + pow(Ml,2)*(1-on_cont)*(1-on_cont))/on_cont;
    //    x[a->on.get_idx()] = on_cont;
    //    x[a->delta.get_idx()] = Ml - Ml*on_cont + l*on_cont/2;
    //    x[a->cs.get_idx()] = CSL.eval(x);
    //    Constraint CS_UB_OAL("CS_UB_OAL");
    //    CS_UB_OAL += Cs_l.outer_approx(x);
    //    CS_UB_OAL <= 0;
    //    _model->addConstraint(CS_UB_OAL);
    
    
    
    
    if(u > 0) {
        Function Cs_r;
        Cs_r += a->delta - Mu*(1-a->on) - sqrt((a->on*a->on-a->cs*a->on)/alpha);
        
        for(int i = 5; i <= 5; ++i) {
            on_cont = i*0.2;
            if (on_cont == 0) on_cont = 0.01;
            CSR.reset();
            CSR += on_cont - alpha*(a->delta*a->delta - 2*Mu*a->delta*(1-on_cont) + pow(Mu,2)*(1-on_cont)*(1-on_cont))/on_cont;
            
            x[a->on.get_idx()] = on_cont;
            
            ////            if (on_cont >= 0.4) {
            //              if(0.01 >= Mu - Mu*on_cont) {
            //                x[a->delta.get_idx()] = 0.01;
            ////                x[a->delta.get_idx()] = Mu - Mu*on_cont + 0.01;
            //                x[a->cs.get_idx()] = CSR.eval(x);
            //                Constraint CS_UB_OAR1("CS_UB_OAR1");
            //                CS_UB_OAR1 += Cs_r.outer_approx(x);
            //                CS_UB_OAR1 <= 0;
            //                _model->addConstraint(CS_UB_OAR1);
            //            }
            
            //            if(u/2 >= Mu - Mu*on_cont) {
            //                x[a->delta.get_idx()] = u/2;
            ////                x[a->delta.get_idx()] = Mu - Mu*on_cont + u*on_cont/2;
            //                x[a->cs.get_idx()] = CSR.eval(x);
            //                Constraint CS_UB_OAR2("CS_UB_OAR2");
            //                CS_UB_OAR2 += Cs_r.outer_approx(x);
            //                CS_UB_OAR2 <= 0;
            //                _model->addConstraint(CS_UB_OAR2);
            //            }
            
            //            if(u >= Mu - Mu*on_cont) {
            //            if (on_cont >= 0.4) {
            //                x[a->delta.get_idx()] = u; //need to check for zero
            x[a->delta.get_idx()] = Mu + on_cont*(u - Mu);
            x[a->cs.get_idx()] = CSR.eval(x);
            Constraint CS_UB_OAR3("CS_UB_OAR3");
            CS_UB_OAR3 += Cs_r.outer_approx(x);
            CS_UB_OAR3 <= 0;
            _model->addConstraint(CS_UB_OAR3);
            //            }
            
        }
    }
    
    if(l < 0) {
        Function CSL; // cs(theta,u) if theta < 0
        Function Cs_l;
        Cs_l += Ml*(1-a->on) - a->delta - sqrt((a->on*a->on-a->cs*a->on)/alpha);
        
        for(int i = 5; i <= 5; ++i) {
            on_cont = i*0.2;
            if (on_cont == 0) on_cont = 0.01;
            CSL.reset();
            CSL += on_cont - alpha*(a->delta*a->delta - 2*Ml*a->delta*(1-on_cont) + pow(Ml,2)*(1-on_cont)*(1-on_cont))/on_cont;
            
            x[a->on.get_idx()] = on_cont;
            
            //            if(-0.01 <= Ml - Ml*on_cont) {
            ////            if (on_cont >= 0.4) {
            //                x[a->on.get_idx()] = on_cont;
            //                x[a->delta.get_idx()] = -0.01;
            ////                x[a->delta.get_idx()] = Ml - Ml*on_cont - 0.01;
            //                x[a->cs.get_idx()] = CSL.eval(x);
            //                Constraint CS_UB_OAL1("CS_UB_OAL1");
            //                CS_UB_OAL1 += Cs_l.outer_approx(x);
            //                CS_UB_OAL1 <= 0;
            //                _model->addConstraint(CS_UB_OAL1);
            //            }
            
            //            if(l/2 <= Ml - Ml*on_cont) {
            //                x[a->delta.get_idx()] = l/2;
            ////                x[a->delta.get_idx()] = Ml - Ml*on_cont + l*on_cont/2;
            //                x[a->cs.get_idx()] = CSL.eval(x);
            //                Constraint CS_UB_OAL2("CS_UB_OAL2");
            //                CS_UB_OAL2 += Cs_l.outer_approx(x);
            //                CS_UB_OAL2 <= 0;
            //                _model->addConstraint(CS_UB_OAL2);
            //            }
            
            //            if(l <= Ml - Ml*on_cont) {
            //            if (on_cont >= 0.4) {
            //                x[a->delta.get_idx()] = l; //need to check for zero
            x[a->delta.get_idx()] = Ml + on_cont*(l - Ml);
            x[a->cs.get_idx()] = CSL.eval(x);
            Constraint CS_UB_OAL3("CS_UB_OAL3");
            CS_UB_OAL3 += Cs_l.outer_approx(x);
            CS_UB_OAL3 <= 0;
            _model->addConstraint(CS_UB_OAL3);
            //            }
            
        }
    }
}



void PowerModel::post_AC_Polar(){
    for (auto n:_net->nodes) {
        add_AC_Voltage_Bounds(n);
        add_AC_KCL(n, false);
    }
    meta_var* p_ij = new meta_var("p_ij", _model);
    meta_var* q_ij = new meta_var("q_ij", _model);
    meta_var* p_ji = new meta_var("p_ji", _model);
    meta_var* q_ji = new meta_var("q_ji", _model);
    meta_var* v_i = new meta_var("v_i",_model);
    meta_var* v_j = new meta_var("v_j",_model);
    meta_var* th_i = new meta_var("th_i",_model);
    meta_var* th_j = new meta_var("th_j",_model);
    meta_constant* g = new meta_constant("g",_model);
    meta_constant* b = new meta_constant("b",_model);
    meta_constant* tr = new meta_constant("tr",_model);
    meta_constant* as = new meta_constant("as",_model);
    meta_constant* ch = new meta_constant("ch",_model);
    meta_constant* S = new meta_constant("S",_model);
    /** subject to Flow_P_From {(l,i,j) in arcs_from}:
     p[l,i,j] = g[l]*(v[i]/tr[l])^2
     + -g[l]*v[i]/tr[l]*v[j]*cos(t[i]-t[j]-as[l])
     + -b[l]*v[i]/tr[l]*v[j]*sin(t[i]-t[j]-as[l]);
     */
    meta_Constraint* Meta_Flow_P_From = new meta_Constraint();
    *Meta_Flow_P_From = (*p_ij) - (*g)*(((*v_i)/(*tr))^2) + (*g)/(*tr)*((*v_i)*(*v_j)*cos(*th_i - *th_j - *as)) + (*b)/(*tr)*((*v_i)*(*v_j)*sin(*th_i - *th_j - *as));
    *Meta_Flow_P_From = 0;
//    Meta_Flow_P_From->print();
//    Meta_Flow_P_From->_val.resize(_net->arcs.size());
    _model->addMetaConstraint(*Meta_Flow_P_From);
    
    /** subject to Flow_P_To {(l,i,j) in arcs_to}:
     p[l,i,j] = g[l]*v[i]^2
     + -g[l]*v[i]*v[j]/tr[l]*cos(t[i]-t[j]+as[l])
     + -b[l]*v[i]*v[j]/tr[l]*sin(t[i]-t[j]+as[l]);
     */
    meta_Constraint* Meta_Flow_P_To = new meta_Constraint();
    *Meta_Flow_P_To = (*p_ji) - (*g)*(Function((*v_j)^2)) + (*g)/(*tr)*((*v_i)*(*v_j)*cos(*th_j - *th_i + *as)) + (*b)/(*tr)*((*v_i)*(*v_j)*sin(*th_j - *th_i + *as));
    *Meta_Flow_P_To = 0;
//    Meta_Flow_P_To->print();
//    Meta_Flow_P_To->_val.resize(_net->arcs.size());
    _model->addMetaConstraint(*Meta_Flow_P_To);

    /** subject to Flow_Q_From {(l,i,j) in arcs_from}:
     q[l,i,j] = -(charge[l]/2+b[l])*(v[i]/tr[l])^2
     +  b[l]*v[i]/tr[l]*v[j]*cos(t[i]-t[j]-as[l])
     + -g[l]*v[i]/tr[l]*v[j]*sin(t[i]-t[j]-as[l]);
     */
    meta_Constraint* Meta_Flow_Q_From = new meta_Constraint();
    *Meta_Flow_Q_From = (*q_ij) + ((0.5*(*ch))+(*b))*(((*v_i)/(*tr))^2) - (*b)/(*tr)*((*v_i)*(*v_j)*cos(*th_i - *th_j - *as)) + (*g)/(*tr)*((*v_i)*(*v_j)*sin(*th_i - *th_j - *as));
    *Meta_Flow_Q_From = 0;
//    Meta_Flow_Q_From->print();
//    Meta_Flow_Q_From->_val.resize(_net->arcs.size());
    _model->addMetaConstraint(*Meta_Flow_Q_From);
    
    /** subject to Flow_Q_To {(l,i,j) in arcs_to}:
     q[l,i,j] = -(charge[l]/2+b[l])*v[i]^2
     +  b[l]*v[i]*v[j]/tr[l]*cos(t[i]-t[j]+as[l])
     + -g[l]*v[i]*v[j]/tr[l]*sin(t[i]-t[j]+as[l]);
     */
    meta_Constraint* Meta_Flow_Q_To = new meta_Constraint();
    *Meta_Flow_Q_To = (*q_ji) + ((0.5*(*ch))+(*b))*(Function((*v_j)^2)) - (*b)/(*tr)*((*v_i)*(*v_j)*cos(*th_j - *th_i + *as)) + (*g)/(*tr)*((*v_i)*(*v_j)*sin(*th_j - *th_i + *as));
    *Meta_Flow_Q_To = 0;
//    Meta_Flow_Q_To->print();
//    Meta_Flow_Q_To->_val.resize(_net->arcs.size());
    _model->addMetaConstraint(*Meta_Flow_Q_To);
    
    meta_Constraint* Thermal_Limit_From = new meta_Constraint();
    *Thermal_Limit_From = (*p_ij)*(*p_ij) + (*q_ij)*(*q_ij) - (*S)*(*S);
    *Thermal_Limit_From <= 0;
//    Thermal_Limit_From->_val.resize(_net->arcs.size());
    _model->addMetaConstraint(*Thermal_Limit_From);

    meta_Constraint* Thermal_Limit_To = new meta_Constraint();
    *Thermal_Limit_To = (*p_ij)*(*p_ij) + (*q_ij)*(*q_ij) - (*S)*(*S);
    *Thermal_Limit_To <= 0;
//    Thermal_Limit_To->_val.resize(_net->arcs.size());
    _model->addMetaConstraint(*Thermal_Limit_To);

    _model->init_functions(_net->arcs.size());
    _model->print_functions();
//    exit(-1);
    for (auto a:_net->arcs) {
        if(a->status==1){
            add_AC_Angle_Bounds(a, false);
            *as = a->as;
            *g = a->g;
            *b = a->b;
            *tr = a->tr;
            *ch = a->ch;
            *S = a->limit;
            *p_ij = a->pi;
            *q_ij = a->qi;
            *p_ji = a->pj;
            *q_ji = a->qj;
            *v_i = a->src->v;
            *v_j = a->dest->v;
            *th_i = a->src->theta;
            *th_j = a->dest->theta;
            _model->concretise(*Meta_Flow_P_From, a->id, "Flow_From_"+a->pi._name);
            _model->concretise(*Meta_Flow_Q_From, a->id, "Flow_From_"+a->qi._name);
            _model->concretise(*Meta_Flow_P_To, a->id, "Flow_To_"+a->pi._name);
            _model->concretise(*Meta_Flow_Q_To, a->id, "Flow_To_"+a->qi._name);
            
            add_AC_thermal(a, false);
        }
    }
}

void PowerModel::post_AC_OTS(){
    for (auto n:_net->nodes) {
        add_AC_KCL(n, true);
    }
    for (auto a:_net->arcs) {
        add_AC_Angle_Bounds(a, true);
        add_AC_Power_Flow(a, true);
        add_AC_thermal(a, true);
    }
}


void PowerModel::post_AC_SOCP(){
    Node* src = NULL;
    Node* dest = NULL;
    for (auto n:_net->nodes) {
        add_AC_KCL(n, false);
    }
    Arc* ap = nullptr;
    
    for (auto a:_net->arcs) {
        if (a->status==0) {
            continue;
        }
        src = a->src;
        dest = a->dest;
        /** subject to Flow_P_From {(l,i,j) in arcs_from}:
         p[l,i,j] = g[l]*(v[i]/tr[l])^2
         + -g[l]*v[i]/tr[l]*v[j]*cos(t[i]-t[j]-as[l])
         + -b[l]*v[i]/tr[l]*v[j]*sin(t[i]-t[j]-as[l]);
         */
        Constraint Flow_P_From("Flow_P_From"+a->pi._name);
        Flow_P_From += a->pi;
//        if(!a->src->_name.compare("9006") && !a->dest->_name.compare("9003")){
//            cout << "ok";
//            cout << endl;
//        }
        
        Flow_P_From -= (a->g/(pow(a->cc,2)+pow(a->dd,2)))*src->w;
        ap = _net->get_arc(a->src, a->dest);
        Flow_P_From -= (-a->g*a->cc + a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wr;
        Flow_P_From -= (-a->b*a->cc - a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wi;
        Flow_P_From = 0;
        _model->addConstraint(Flow_P_From);
        
        /** subject to Flow_P_To {(l,i,j) in arcs_to}:
         p[l,i,j] = g[l]*v[i]^2
         + -g[l]*v[i]*v[j]/tr[l]*cos(t[i]-t[j]+as[l])
         + -b[l]*v[i]*v[j]/tr[l]*sin(t[i]-t[j]+as[l]);
         */
        Constraint Flow_P_To("Flow_P_To"+a->pj._name);
        Flow_P_To += a->pj;
        Flow_P_To -= a->g*dest->w;
        Flow_P_To -= (-a->g*a->cc - a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wr;
        Flow_P_To += (-a->b*a->cc + a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wi;
        Flow_P_To = 0;
        _model->addConstraint(Flow_P_To);
        
        /** subject to Flow_Q_From {(l,i,j) in arcs_from}:
         q[l,i,j] = -(charge[l]/2+b[l])*(v[i]/tr[l])^2
         +  b[l]*v[i]/tr[l]*v[j]*cos(t[i]-t[j]-as[l])
         + -g[l]*v[i]/tr[l]*v[j]*sin(t[i]-t[j]-as[l]);
         */
        Constraint Flow_Q_From("Flow_Q_From"+a->qi._name);
        Flow_Q_From += a->qi;
        Flow_Q_From += (a->ch/2+a->b)/(pow(a->cc,2)+pow(a->dd,2))*src->w;
        Flow_Q_From += (-a->b*a->cc - a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wr;
        Flow_Q_From -= (-a->g*a->cc + a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wi;
        Flow_Q_From = 0;
        _model->addConstraint(Flow_Q_From);
        
        /** subject to Flow_Q_To {(l,i,j) in arcs_to}:
         q[l,i,j] = -(charge[l]/2+b[l])*v[i]^2
         +  b[l]*v[i]*v[j]/tr[l]*cos(t[i]-t[j]+as[l])
         + -g[l]*v[i]*v[j]/tr[l]*sin(t[i]-t[j]+as[l]);
         */
        Constraint Flow_Q_To("Flow_Q_To"+a->qj._name);
        Flow_Q_To += a->qj;
        Flow_Q_To += (a->ch/2+a->b)*dest->w;
        Flow_Q_To += (-a->b*a->cc + a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wr;
        Flow_Q_To += (-a->g*a->cc - a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*ap->wi;
        Flow_Q_To = 0;
        _model->addConstraint(Flow_Q_To);
        
        /** subject to PSD {l in lines}: w[from_bus[l]]*w[to_bus[l]] >= wr[l]^2 + wi[l]^2;
         */
        Constraint SOCP("SOCP");
        SOCP += a->src->w*a->dest->w;
        SOCP -= ((ap->wr)^2);
        SOCP -= ((ap->wi)^2);
        SOCP >= 0;
        _model->addConstraint(SOCP);
        
        add_AC_thermal(a, false);
        add_Wr_Wi(a);
    }
}

int PowerModel::get_order(vector<Node*>* bag, int order){
    Node* src = nullptr;
    switch (order) {
        case 1:
            src = bag->at(0);
            if (_net->_clone->has_directed_arc(src, bag->at(1)) &&  _net->_clone->has_directed_arc(src, bag->at(2))){
                    return 0;
            }
            src = bag->at(1);
            if (_net->_clone->has_directed_arc(src, bag->at(0)) &&  _net->_clone->has_directed_arc(src, bag->at(2))){
                return 1;
            }
            return 2;
            break;
        case 2:
            src = bag->at(0);
            if (_net->_clone->has_directed_arc(bag->at(1), src) &&  _net->_clone->has_directed_arc(bag->at(2), src)){
                return 0;
            }
            src = bag->at(1);
            if (_net->_clone->has_directed_arc(bag->at(0), src) &&  _net->_clone->has_directed_arc(bag->at(2), src)){
                return 1;
            }
            return 2;
            break;
        case 3:
            src = bag->at(0);
            if (_net->_clone->has_directed_arc(src, bag->at(1)) &&  _net->_clone->has_directed_arc(bag->at(2), src)){
                return 0;
            }
            if (_net->_clone->has_directed_arc(bag->at(1), src) &&  _net->_clone->has_directed_arc(src, bag->at(2))){
                return 0;
            }
            src = bag->at(1);
            if (_net->_clone->has_directed_arc(src, bag->at(0)) &&  _net->_clone->has_directed_arc(bag->at(2), src)){
                return 1;
            }
            if (_net->_clone->has_directed_arc(bag->at(0), src) &&  _net->_clone->has_directed_arc(src, bag->at(2))){
                return 1;
            }
            return 2;
            break;
            
        default:
            assert(false);
            return -1;
            break;
    }
}

void PowerModel::add_SDP_cuts(int dim){
    assert(dim==3);
    /** subject to PSD {l in lines}: w[from_bus[l]]*w[to_bus[l]] >= wr[l]^2 + wi[l]^2;
     //     */
    meta_var* wr12 = new meta_var("wr12", _model);
    meta_var* wr13 = new meta_var("wr13", _model);
    meta_var* wr31 = new meta_var("wr31", _model);
    meta_var* wr32 = new meta_var("wr32", _model);
    meta_var* wr23 = new meta_var("wr23", _model);
    meta_var* wi12 = new meta_var("wi12", _model);
    meta_var* wi13 = new meta_var("wi13", _model);
    meta_var* wi31 = new meta_var("wi31", _model);
    meta_var* wi23 = new meta_var("wi23", _model);
    meta_var* wi32 = new meta_var("wi32", _model);
    meta_var* w1 = new meta_var("w1", _model);
    meta_var* w2 = new meta_var("w2", _model);
    meta_var* w3 = new meta_var("w3", _model);
    
    meta_Constraint* SDP = new meta_Constraint();
    *SDP = (*wr12)*((*wr32)*(*wr13) - (*wi32)*(*wi13));
    *SDP += (*wi12)*((*wi32)*(*wr13) + (*wr32)*(*wi13));
    *SDP *= 2;
    *SDP -= (((*wr12)^2)+((*wi12)^2))*(*w3);
    *SDP -= (((*wr13)^2)+((*wi13)^2))*(*w2);
    *SDP -= (((*wr32)^2)+((*wi32)^2))*(*w1);
    *SDP += (*w1)*(*w2)*(*w3);
    *SDP >= 0;
    _model->addMetaConstraint(*SDP);
    
    meta_Constraint* SDPr = new meta_Constraint();// Rotational SDP
    *SDPr = (*wr12)*((*wr23)*(*wr31) - (*wi23)*(*wi31));
    *SDPr -= (*wi12)*((*wi23)*(*wr31) + (*wr23)*(*wi31));
    *SDPr *= 2;
    *SDPr -= (((*wr12)^2)+((*wi12)^2))*(*w3);
    *SDPr -= (((*wr31)^2)+((*wi31)^2))*(*w2);
    *SDPr -= (((*wr23)^2)+((*wi23)^2))*(*w1);
    *SDPr += (*w1)*(*w2)*(*w3);
    *SDPr >= 0;
    _model->addMetaConstraint(*SDPr);
    
    Node* n1 = nullptr;
    Node* n2 = nullptr;
    Node* n3 = nullptr;
    Arc* a12 = nullptr;
    Arc* a13 = nullptr;
    Arc* a32 = nullptr;
    _model->init_functions(_net->_bags->size());
    _model->print_functions();
    
    int id = 0;
    int n_i1, n_i2, n_i3 = 0;
    bool rot_bag = false;
    for (auto b: *_net->_bags){
        //        if (id>=1) {
        //            continue;
        //        }
        //        string ok;
        if (b->size()==dim) {
            rot_bag = _net->rotation_bag(b);
            if (rot_bag) {
                //                cout << "rot_bag: ";
                n1 = b->at(0);
                if (_net->_clone->has_directed_arc(n1, b->at(1))){
                    n2 = b->at(1);
                    n3 = b->at(2);
                }
                else {
                    n2 = b->at(2);
                    n3 = b->at(1);
                }
                a13 = _net->get_arc(n3,n1);
                a32 = _net->get_arc(n2,n3);
            }
            else {
                n_i1 = get_order(b,1);
                n1 = b->at(n_i1);
                n_i2 = get_order(b,2);
                n2 = b->at(n_i2);
                n_i3 = get_order(b,3);
                n3 = b->at(n_i3);
                a13 = _net->get_arc(n1,n3);
                a32 = _net->get_arc(n3,n2);
            }
            a12 = _net->get_arc(n1,n2);
            
            if(!a12 || a12->status==0) {
                //                continue;
                if (!a12) {
                    a12 = _net->_clone->get_arc(n1,n2)->clone();
                    a12->status=1;
                    a12->src = _net->get_node(a12->src->_name);
                    a12->dest = _net->get_node(a12->dest->_name);
                    a12->connect();
                    _net->add_arc(a12);
                }
                //                else {
                //                    assert(false);
                //                }
                if (a12->tbound.min < 0 && a12->tbound.max > 0)
                    a12->cs.init("cs("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",min(cos(a12->tbound.min), cos(a12->tbound.max)), 1.);
                else
                    a12->cs.init("cs("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",min(cos(a12->tbound.min), cos(a12->tbound.max)), max(cos(a12->tbound.min),cos(a12->tbound.max)));
                a12->vv.init("vv("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->src->vbound.min*a12->dest->vbound.min,a12->src->vbound.max*a12->dest->vbound.max);
                if (_type==QC_SDP) {
                    a12->vcs.init("vcs("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_lb()*a12->cs.get_lb(), a12->vv.get_ub()*a12->cs.get_ub());
                    if(a12->tbound.min < 0 && a12->tbound.max > 0)
                        a12->vsn.init("vsn("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_ub()*sin(a12->tbound.min), a12->vv.get_ub()*sin(a12->tbound.max));
                    if (a12->tbound.min >= 0)
                        a12->vsn.init("vsn("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_lb()*sin(a12->tbound.min), a12->vv.get_ub()*sin(a12->tbound.max));
                    if (a12->tbound.max <= 0)
                        a12->vsn.init("vsn("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_ub()*sin(a12->tbound.min), a12->vv.get_lb()*sin(a12->tbound.max));
                    _model->addVar(a12->vcs);
                    a12->vcs = 1;
                    _model->addVar(a12->vsn);
                }
                else {
                    a12->wr.init("wr("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_lb()*a12->cs.get_lb(), a12->vv.get_ub()*a12->cs.get_ub());
                    if(a12->tbound.min < 0 && a12->tbound.max > 0)
                        a12->wi.init("wi("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_ub()*sin(a12->tbound.min), a12->vv.get_ub()*sin(a12->tbound.max));
                    if (a12->tbound.min >= 0)
                        a12->wi.init("wi("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_lb()*sin(a12->tbound.min), a12->vv.get_ub()*sin(a12->tbound.max));
                    if (a12->tbound.max <= 0)
                        a12->wi.init("wi("+a12->_name+","+a12->src->_name+","+a12->dest->_name+")",a12->vv.get_ub()*sin(a12->tbound.min), a12->vv.get_lb()*sin(a12->tbound.max));
                    a12->wr = 1;
                    _model->addVar(a12->wr);
                    _model->addVar(a12->wi);
                }
                Constraint SOCP1("SOCP1");
                SOCP1 += _net->get_node(n1->_name)->w*_net->get_node(n2->_name)->w;
                if (_type==QC_SDP) {
                    SOCP1 -= ((a12->vcs)^2);
                    SOCP1 -= ((a12->vsn)^2);
                }
                else{
                    SOCP1 -= ((a12->wr)^2);
                    SOCP1 -= ((a12->wi)^2);
                }
                SOCP1 >= 0;
                _model->addConstraint(SOCP1);
                //                SOCP1.print();
                //                _net->_clone->remove_arc(a12);
            }
            if(!a13 || a13->status==0) {
                if(!a13) {
                    //                    continue;
                    if (rot_bag) {
                        a13 = _net->_clone->get_arc(n3,n1)->clone();
                    }
                    else {
                        a13 = _net->_clone->get_arc(n1,n3)->clone();
                    }
                    a13->status=1;
                    a13->src = _net->get_node(a13->src->_name);
                    a13->dest = _net->get_node(a13->dest->_name);
                    a13->connect();
                    _net->add_arc(a13);
                }
                //                else {
                //                    assert(false);
                //                }
                if (a13->tbound.min < 0 && a13->tbound.max > 0)
                    a13->cs.init("cs("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",min(cos(a13->tbound.min), cos(a13->tbound.max)), 1.);
                else
                    a13->cs.init("cs("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",min(cos(a13->tbound.min), cos(a13->tbound.max)), max(cos(a13->tbound.min),cos(a13->tbound.max)));
                a13->vv.init("vv("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->src->vbound.min*a13->dest->vbound.min,a13->src->vbound.max*a13->dest->vbound.max);
                
                if (_type==QC_SDP) {
                    a13->vcs.init("vcs("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_lb()*a13->cs.get_lb(), a13->vv.get_ub()*a13->cs.get_ub());
                    if(a13->tbound.min < 0 && a13->tbound.max > 0)
                        a13->vsn.init("vsn("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_ub()*sin(a13->tbound.min), a13->vv.get_ub()*sin(a13->tbound.max));
                    if (a13->tbound.min >= 0)
                        a13->vsn.init("vsn("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_lb()*sin(a13->tbound.min), a13->vv.get_ub()*sin(a13->tbound.max));
                    if (a13->tbound.max <= 0)
                        a13->vsn.init("vsn("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_ub()*sin(a13->tbound.min), a13->vv.get_lb()*sin(a13->tbound.max));
                    _model->addVar(a13->vcs);
                    a13->vcs = 1;
                    _model->addVar(a13->vsn);
                }
                else {
                    a13->wr.init("wr("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_lb()*a13->cs.get_lb(), a13->vv.get_ub()*a13->cs.get_ub());
                    if(a13->tbound.min < 0 && a13->tbound.max > 0)
                        a13->wi.init("wi("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_ub()*sin(a13->tbound.min), a13->vv.get_ub()*sin(a13->tbound.max));
                    if (a13->tbound.min >= 0)
                        a13->wi.init("wi("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_lb()*sin(a13->tbound.min), a13->vv.get_ub()*sin(a13->tbound.max));
                    if (a13->tbound.max <= 0)
                        a13->wi.init("wi("+a13->_name+","+a13->src->_name+","+a13->dest->_name+")",a13->vv.get_ub()*sin(a13->tbound.min), a13->vv.get_lb()*sin(a13->tbound.max));
                    a13->wr = 1;
                    _model->addVar(a13->wr);
                    _model->addVar(a13->wi);
                }
                Constraint SOCP2("SOCP2");
                SOCP2 += _net->get_node(n1->_name)->w*_net->get_node(n3->_name)->w;
                if (_type==QC_SDP) {
                    SOCP2 -= ((a13->vcs)^2);
                    SOCP2 -= ((a13->vsn)^2);
                }
                else{
                    SOCP2 -= ((a13->wr)^2);
                    SOCP2 -= ((a13->wi)^2);
                }
                SOCP2 >= 0;
                _model->addConstraint(SOCP2);
                //                _net->_clone->remove_arc(a13);
            }
            if(!a32 || a32->status==0) {
                if (!a32) {
                    //                    continue;
                    if (rot_bag) {
                        a32 = _net->_clone->get_arc(n2,n3)->clone();
                    }
                    else {
                        a32 = _net->_clone->get_arc(n3,n2)->clone();
                    }
                    a32->status=1;
                    a32->src = _net->get_node(a32->src->_name);
                    a32->dest = _net->get_node(a32->dest->_name);
                    a32->connect();
                    _net->add_arc(a32);
                }
                //                else {
                //                    assert(false);
                //                }
                if (a32->tbound.min < 0 && a32->tbound.max > 0)
                    a32->cs.init("cs("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",min(cos(a32->tbound.min), cos(a32->tbound.max)), 1.);
                else
                    a32->cs.init("cs("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",min(cos(a32->tbound.min), cos(a32->tbound.max)), max(cos(a32->tbound.min),cos(a32->tbound.max)));
                a32->vv.init("vv("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->src->vbound.min*a32->dest->vbound.min,a32->src->vbound.max*a32->dest->vbound.max);
                
                if (_type==QC_SDP) {
                    a32->vcs.init("vcs("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_lb()*a32->cs.get_lb(), a32->vv.get_ub()*a32->cs.get_ub());
                    if(a32->tbound.min < 0 && a32->tbound.max > 0)
                        a32->vsn.init("vsn("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_ub()*sin(a32->tbound.min), a32->vv.get_ub()*sin(a32->tbound.max));
                    if (a32->tbound.min >= 0)
                        a32->vsn.init("vsn("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_lb()*sin(a32->tbound.min), a32->vv.get_ub()*sin(a32->tbound.max));
                    if (a32->tbound.max <= 0)
                        a32->vsn.init("vsn("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_ub()*sin(a32->tbound.min), a32->vv.get_lb()*sin(a32->tbound.max));
                    _model->addVar(a32->vcs);
                    a32->vcs = 1;
                    _model->addVar(a32->vsn);
                }
                else {
                    a32->wr.init("wr("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_lb()*a32->cs.get_lb(), a32->vv.get_ub()*a32->cs.get_ub());
                    if(a32->tbound.min < 0 && a32->tbound.max > 0)
                        a32->wi.init("wi("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_ub()*sin(a32->tbound.min), a32->vv.get_ub()*sin(a32->tbound.max));
                    if (a32->tbound.min >= 0)
                        a32->wi.init("wi("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_lb()*sin(a32->tbound.min), a32->vv.get_ub()*sin(a32->tbound.max));
                    if (a32->tbound.max <= 0)
                        a32->wi.init("wi("+a32->_name+","+a32->src->_name+","+a32->dest->_name+")",a32->vv.get_ub()*sin(a32->tbound.min), a32->vv.get_lb()*sin(a32->tbound.max));
                    a32->wr = 1;
                    _model->addVar(a32->wr);
                    _model->addVar(a32->wi);
                }
                Constraint SOCP3("SOCP3");
                SOCP3 += _net->get_node(n3->_name)->w*_net->get_node(n2->_name)->w;
                if (_type==QC_SDP) {
                    SOCP3 -= ((a32->vcs)^2);
                    SOCP3 -= ((a32->vsn)^2);
                }
                else{
                    SOCP3 -= ((a32->wr)^2);
                    SOCP3 -= ((a32->wi)^2);
                }
                SOCP3 >= 0;
                _model->addConstraint(SOCP3);
                //                _net->_clone->remove_arc(a32);
            }
            
            if (_type==QC_SDP) {
                *wr12 = a12->vcs;
                *wi12 = a12->vsn;
                *wr13 = a13->vcs;
                *wi13 = a13->vsn;
                *wr31 = a13->vcs;
                *wi31 = a13->vsn;
                *wr32 = a32->vcs;
                *wi32 = a32->vsn;
                *wr23 = a32->vcs;
                *wi23 = a32->vsn;
            }
            else{
                *wr12 = a12->wr;
                *wi12 = a12->wi;
                *wr13 = a13->wr;
                *wi13 = a13->wi;
                *wr31 = a13->wr;
                *wi31 = a13->wi;
                *wr32 = a32->wr;
                *wi32 = a32->wi;
                *wr23 = a32->wr;
                *wi23 = a32->wi;
            }
            *w1 = _net->get_node(n1->_name)->w;
            *w2 = _net->get_node(n2->_name)->w;
            *w3 = _net->get_node(n3->_name)->w;
            if (rot_bag) {
                _model->concretise(*SDPr, id, "SDP"+to_string(id));
                id++;
            }
            else {
                _model->concretise(*SDP, id, "SDP"+to_string(id));
                id++;
            }
        }
    }
    cout << "total number of 3d SDP cuts added = " << id << endl;
    
}


void PowerModel::add_AC_Voltage_Bounds(Node* n){
    /** subject to V_UB{i in buses}: vr[i]^2 + vi[i]^2 <= v_max[i]^2;
     subject to V_LB{i in buses}: vr[i]^2 + vi[i]^2 >= v_min[i]^2;
     */
    Constraint V_UB("V_UB");
    V_UB += (n->_V_.square_magnitude());
    V_UB <= pow(n->vbound.max,2);
    _model->addConstraint(V_UB);
    
    Constraint V_LB("V_LB");
    V_LB += (n->_V_.square_magnitude());
    V_LB >= pow(n->vbound.min,2);
    _model->addConstraint(V_LB);
}


void PowerModel::post_AC_Rect(){
    
    for (auto n:_net->nodes) {
        add_AC_Voltage_Bounds(n);
        add_AC_KCL(n, false);
    }
    for (auto a:_net->arcs) {
        add_AC_Power_Flow(a, false);
        add_AC_thermal(a, false);
    }
    
    
}


void PowerModel::post_DC(){
    Node* src = NULL;
    Node* dest = NULL;
    for (auto a:_net->arcs) {
        if (a->status==0) {
            continue;
        }
        src = a->src;
        dest = a->dest;
        /** Adding phase angle difference bound constraint
         subject to Theta_Delta_LB{(l,i,j) in arcs_from}: t[i]-t[j] >= -theta_bound;
         subject to Theta_Delta_UB{(l,i,j) in arcs_from}: t[i]-t[j] <= theta_bound;
         */
        Constraint Theta_Delta_UB("Theta_Delta_UB");
        Theta_Delta_UB += src->theta - dest->theta;
        Theta_Delta_UB <= a->tbound.max;
        _model->addConstraint(Theta_Delta_UB);
        Constraint Theta_Delta_LB("Theta_Delta_LB");
        Theta_Delta_LB += src->theta - dest->theta;
        Theta_Delta_LB >= a->tbound.min;
        _model->addConstraint(Theta_Delta_LB);
        
        /** subject to Flow_P_From {(l,i,j) in arcs_from}:
         p[l,i,j] = g[l]*(v[i]/tr[l])^2
         + -g[l]*v[i]/tr[l]*v[j]*cos(t[i]-t[j]-as[l])
         + -b[l]*v[i]/tr[l]*v[j]*sin(t[i]-t[j]-as[l]);
         */
        Constraint Flow_P_From("Flow_P_From"+a->pi._name);
        Flow_P_From += a->pi;
        Flow_P_From += a->b/a->tr*(src->theta - dest->theta - a->as);
        Flow_P_From = 0;
        _model->addConstraint(Flow_P_From);
        /** subject to Flow_P_To {(l,i,j) in arcs_to}:
         p[l,i,j] = p[l,j,i];
         */
        Constraint Flow_P_To("Flow_P_To"+a->pj._name);
        Flow_P_To += a->pj;
        Flow_P_To += a->pi;
        Flow_P_To = 0;
        _model->addConstraint(Flow_P_To);
        
        /** subject to Thermal_Limit {(l,i,j) in arcs}: p[l,i,j] <=  s[l];
         */
        Constraint Thermal_Limit_from("Thermal_Limit_from");
        Thermal_Limit_from += a->pi;
        Thermal_Limit_from <= a->limit;
        _model->addConstraint(Thermal_Limit_from);
        Constraint Thermal_Limit_to("Thermal_Limit_to");
        Thermal_Limit_to += a->pj;
        Thermal_Limit_to <= a->limit;
        _model->addConstraint(Thermal_Limit_to);
    }
    
    for (auto n:_net->nodes) {
        /** subject to KCL_P {i in buses}: sum{(l,i,j) in arcs} p[l,i,j] + shunt_g[i]*1.0 + load_p[i] = sum{(i,gen) in bus_gen} pg[gen];
         */
        Constraint KCL_P("KCL_P");
        for (auto a:n->branches) {
            if (a->status==0) {
                continue;
            }
            if(a->src==n){
                KCL_P += a->pi;
            }
            else {
                KCL_P += a->pj;
            }
        }
        for (auto g:n->_gen) {
            if (!g->_active)
                continue;
            KCL_P -= g->pg;
        }
        KCL_P += n->gs() + n->pl();
        KCL_P = 0;
        _model->addConstraint(KCL_P);
    }
}



void PowerModel::post_SOCP_OTS(){
    Node* src = NULL;
    Node* dest = NULL;
    for (auto n:_net->nodes) {
        add_AC_KCL(n, false);
    }

    for (auto a:_net->arcs) {
        if (a->status==0) {
            continue;
        }
        src = a->src;
        dest = a->dest;

        Constraint Flow_P_From("Flow_P_From"+a->pi._name);
        Flow_P_From += a->pi;
        Flow_P_From -= (a->g/(pow(a->cc,2)+pow(a->dd,2)))*a->w_line_ij;
        Flow_P_From -= (-a->g*a->cc + a->b*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*a->wr;
        Flow_P_From -= (-a->b*a->cc - a->g*a->dd)/(pow(a->cc,2)+pow(a->dd,2))*a->wi;
        Flow_P_From = 0;
        _model->addConstraint(Flow_P_From);
    }
}

Function PowerModel::sum(vector<Arc*> vec, string var, bool switch_lines){
    Function res;
    for (auto& a:vec) {
        if (!switch_lines) {
            if (a->status==1) {
                if (var.compare("pi")==0) {
                    res += a->pi;
                }
                if (var.compare("pj")==0) {
                    res += a->pj;
                }
                if (var.compare("qi")==0) {
                    res += a->qi;
                }
                if (var.compare("qj")==0) {
                    res += a->qj;
                }
            }
        }
        else {
            if (var.compare("pi")==0) {
                res += a->pi*a->on;
            }
            if (var.compare("pj")==0) {
                res += a->pj*a->on;
            }
            if (var.compare("qi")==0) {
                res += a->qi*a->on;
            }
            if (var.compare("qj")==0) {
                res += a->qj*a->on;
            }
        }
    }
    return res;
}


Function PowerModel::sum(vector<Gen*> vec, string var){
    Function res;
    for (auto& g:vec) {
        if (g->_active) {
            if (var.compare("pg")==0) {
                res += g->pg;
            }
            if (var.compare("qg")==0) {
                res += g->qg;
            }
        }
    }
    return res;
}


