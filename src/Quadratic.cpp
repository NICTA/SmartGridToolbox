//
//  Quadratic.cpp
//  PowerTools++
//
//  Created by Hassan on 9/02/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#include "PowerTools++/Quadratic.h"
#include "PowerTools++/Function.h"
using namespace::std;

Quadratic::Quadratic():_cst(0){};
Quadratic::Quadratic(var_& v){
    _cst = 0;
    *this += v;
};

Quadratic::Quadratic(const Quadratic& q){
    _cst = 0;
    *this += q;
};


Quadratic::Quadratic(double cst):_cst(cst){};


Quadratic::~Quadratic(){
    for(auto itdf:_qmatrix) {
        delete itdf.second;
    }
    for(auto itdf:_hess) {
        delete itdf.second;
    }
};

/* Boolean Requests */
bool Quadratic::is_constant() const{
    return (_coefs.empty() && _qmatrix.empty());
}

bool Quadratic::is_linear() const{
    return (_qmatrix.empty());
}

bool Quadratic::is_quadratic() const{
    return (!_qmatrix.empty());
}

bool Quadratic::has_var(int vid) const{
    return (_quad_vars.count(vid)==1 || _lin_vars.count(vid)==1);
}

bool Quadratic::has_q_var(int vid) const{
    return _hess.count(vid)==1;
    //    return _quad_vars.count(vid)==1;
    //    int vjd = 0;
    //    map<int, double>* qmat = NULL;
    //    for (auto& it:_qmatrix){
    //        vjd = it.first;
    //        qmat = it.second;
    //        if (vjd==vid || qmat->count(vid)==1) {
    //            return true;
    //        }
    //    }
    //    return false;
}

/* Accessors */

var_* Quadratic::get_var(int vid) const{
    if (!has_var(vid)) {
        return NULL;
    }
    if (!has_q_var(vid)) {
        return _lin_vars.find(vid)->second;
    }
    return _quad_vars.find(vid)->second;
}

int Quadratic::get_nb_vars() const {
    int n =_lin_vars.size();
    int vid = 0;
    for (auto& iter: _quad_vars) {
        vid = iter.first;
        if (_lin_vars.count(vid)==0) {
            n++;
        }
    }
    return  n;
}

double Quadratic::get_coeff(int vid) const{
    if (_coefs.count(vid)==0) {
        return 0;
    }
    return _coefs.find(vid)->second;
}

double Quadratic::get_q_coeff(int vid1, int vid2) const{
    if (!has_q_var(vid1) || !has_q_var(vid2)) {
        return 0;
    }
    auto it = _qmatrix.find(vid1);
    if(vid1 > vid2)
        it = _qmatrix.find(vid2);
    if (it!=_qmatrix.end()) {
        map<int, double>* coefs = it->second;
        auto it2 = coefs->find(vid2);
        if(vid1 > vid2)
            it2 = coefs->find(vid1);
        if (it2!=coefs->end()) {
            return it2->second;
        }
    }
    return 0;
}

double Quadratic::eval(const double *x) const{
    double res = 0;
    int vid = 0;
    int vjd = 0;
    double coeff = 0;
    
    for(auto& it:_coefs) {
        vid = it.first;
        coeff = it.second;
        res += coeff*x[vid];
    }
    for (auto& it:_qmatrix) {
        vid = it.first;
        map<int, double>* qcoefs = it.second;
        for (auto& itq:*qcoefs) {
            vjd = itq.first;
            assert(vid<=vjd);
            coeff = itq.second;
            res+= coeff*x[vid]*x[vjd];
        }
    }
    res+= _cst;
    return res;
}

double Quadratic::eval_new() const{
    double res = 0;
    int vid = 0;
    int vjd = 0;
    double coeff = 0;
    
    for(auto& it:_coefs) {
        vid = it.first;
        coeff = it.second;
        res += coeff*get_var(vid)->_dval;
    }
    for (auto& it:_qmatrix) {
        vid = it.first;
        map<int, double>* qcoefs = it.second;
        for (auto& itq:*qcoefs) {
            vjd = itq.first;
            assert(vid<=vjd);
            coeff = itq.second;
            res+= coeff*get_var(vid)->_dval*get_var(vjd)->_dval;
        }
    }
    res+= _cst;
    return res;
}


double Quadratic::get_const() const{
    return _cst;
}
int Quadratic::get_q_nnz() const{
    int idx = 0;
    for (auto& it:_qmatrix){
        idx += it.second->size();
    }
    return idx;
}

/* Modifiers */

void Quadratic::add(double cst, var_* v){
    int vid = v->get_idx();
    if (_coefs.count(vid)==0) {
        _coefs.insert(pair<int, double> (vid,cst));
        _lin_vars.insert(pair<int, var_*> (vid,v));
    }
    else
        _coefs[vid]+=cst;
    if (_coefs[vid]==0) {
        _coefs.erase(vid);
        _lin_vars.erase(vid);
    }
};

void Quadratic::merge_vars(const Quadratic& q){
    int vid = 0;
    var_* v = NULL;
    set<int>* hess = NULL;
    for (auto& it:q._lin_vars){
        vid = it.first;
        v = it.second;
        _lin_vars.insert(pair<int, var_*> (vid,v));
    }
    for (auto& it:q._quad_vars){
        vid = it.first;
        v = it.second;
        _quad_vars.insert(pair<int, var_*> (vid,v));
    }
    for(auto& it:q._hess)
    {
        vid = it.first;
        hess = it.second;
        for(int vjd:*hess)
        {
            add_hess_link(vid, vjd);
        }
    }
    
}

void Quadratic::update_q_vars(int vid){
    if (!has_q_var(vid)) {
        _quad_vars.erase(vid);
    }
    
}

void Quadratic::add_hess_link(int vid, int vjd){
    set<int>* hess_set = NULL;
    if(_hess.count(vid)==0) {
        hess_set = new set<int>();
        _hess[vid] = hess_set;
    }
    else
        hess_set = _hess[vid];
    hess_set->insert(vjd);
    if(_hess.count(vjd)==0) {
        hess_set = new set<int>();
        _hess[vjd] = hess_set;
    }
    else
        hess_set = _hess[vjd];
    hess_set->insert(vid);
}

void Quadratic::remove_hess_link(int vid, int vjd){
    set<int>* hess_set = NULL;
    if(_hess.count(vid)==1) {
        hess_set = _hess[vid];
        hess_set->erase(vjd);
    }
    if(_hess.count(vjd)==1) {
        hess_set = _hess[vjd];
        hess_set->erase(vid);
    }
}


/* Operators */
bool Quadratic::operator==(const Quadratic& q) const{
    if (_cst != q._cst || _lin_vars.size()!=q._lin_vars.size() || _quad_vars.size() != q._quad_vars.size() || _hess.size()!= q._hess.size()) {
        return false;
    }
    for (auto& v: _coefs){
        if (!q.has_var(v.first) || q.get_coeff(v.first)!= v.second) {
            return false;
        }
    }
    for (auto& v: q._coefs){
        if (!has_var(v.first) || get_coeff(v.first)!= v.second) {
            return false;
        }
    }
    map<int, double>* qcoefs = nullptr;
    int vid = 0, vjd = 0;
    double coeff = 0;
    for (auto& it:_qmatrix) {
        vid = it.first;
        qcoefs = it.second;
        for (auto& itq:*qcoefs) {
            vjd = itq.first;
            coeff = itq.second;
            if (q.get_q_coeff(vid, vjd)!=coeff) {
                return false;
            }
        }
    }
    for (auto& it:q._qmatrix) {
        vid = it.first;
        qcoefs = it.second;
        for (auto& itq:*qcoefs) {
            vjd = itq.first;
            coeff = itq.second;
            if (get_q_coeff(vid, vjd)!=coeff) {
                return false;
            }
        }
    }
    
    return true;
}


bool Quadratic::operator!=(const Quadratic& q) const{
    return !(*this==q);
}

Quadratic& Quadratic::operator+=(double cst){
    _cst+=cst;
    return *this;
};

Quadratic& Quadratic::operator-=(double cst){
    _cst-=cst;
    return *this;
};

Quadratic& Quadratic::operator*=(double cst){
    // Taking care of zero
    if (is_constant() && _cst==0){
        return *this;
    }
    if(cst==0) {
        reset_coeffs();
        return *this;
    }
    // Taking care of one
    if (cst==1) {
        return *this;
    }
    int vid = 0, vjd = 0;
    // Linear part
    for(auto& it:_coefs)
    {
        vid = it.first;
        this->_coefs[vid]*=cst;
    }
    // Quadratic part
    map<int, double>* qmat = NULL;
    for(auto& it:_qmatrix) {
        vid = it.first;
        qmat = it.second;
        for(auto& itq:*qmat) {
            vjd = itq.first;
            (*qmat)[vjd] *= cst;
        }
    }
    this->_cst *= cst;
    return *this;
};


Quadratic& Quadratic::operator+=(var_& v){
    int vid = v.get_idx();
    if (_coefs.count(vid)==0) {
        _coefs.insert(pair<int, double> (vid,1));
        _lin_vars.insert(pair<int, var_*> (vid,&v));
    }
    else
        _coefs[vid]+=1;
    if (_coefs[vid]==0) {
        _coefs.erase(vid);
        _lin_vars.erase(vid);
    }
    return *this;
};


Quadratic& Quadratic::operator-=(var_& v){
    int vid = v.get_idx();
    if (_coefs.count(vid)==0) {
        _coefs.insert(pair<int, double> (vid,-1));
        _lin_vars.insert(pair<int, var_*> (vid,&v));
    }
    else
        _coefs[vid]-=1;
    if (_coefs[vid]==0) {
        _coefs.erase(vid);
        _lin_vars.erase(vid);
    }
    return *this;
};

Quadratic& Quadratic::operator*=(var_& v){
    assert(!is_quadratic()); // This function should only be called for the product of two linear functions
    Quadratic temp(v);
    // Taking care of zero
    *this *= temp;
    return *this;
}

Quadratic& Quadratic::operator+=(const Quadratic& q){
    double coeff = 0;
    int vid = 0, vjd = 0;
    merge_vars(q);
    // Linear part
    for(auto& it:q._coefs)
    {
        vid = it.first;
        coeff= it.second;
        if(this->_coefs.count(vid)==0) {
            this->_coefs[vid]=coeff;
        }
        else {
            this->_coefs[vid]+=coeff;
        }
        
        if (this->_coefs[vid]==0){
            this->_coefs.erase(vid);
            _lin_vars.erase(vid);
        }
    }
    // Quadratic part
    map<int, double>* qmat = NULL;
    map<int, double>* fqmat = NULL;
    for(auto& it:q._qmatrix) {
        vid = it.first;
        fqmat = it.second;
        if(this->_qmatrix.count(vid)==1)
            qmat = this->_qmatrix[vid];
        else {
            qmat = new map<int, double>();
            this->_qmatrix.insert(pair<int, map<int, double>*> (vid,qmat));
        }
        for(auto& itq:*fqmat) {
            vjd = itq.first;
            coeff = (*qmat)[vjd] + itq.second;
            if(coeff!=0) {
                (*qmat)[vjd] = coeff;
                add_hess_link(vid, vjd);
            }
            else{
                qmat->erase(vjd);
                remove_hess_link(vid,vjd);
                if(qmat->empty()){
                    delete qmat;
                    _qmatrix.erase(vid);
                    update_q_vars(vid);
                    update_q_vars(vjd);
                }
            }
        }
    }
    this->_cst += q._cst;
    return *this;
};

Quadratic& Quadratic::operator-=(const Quadratic& q){
    double coeff = 0;
    int vid = 0, vjd = 0;
    // Linear part
    merge_vars(q);
    for(auto it:q._coefs)
    {
        vid = it.first;
        coeff= it.second;
        if(this->_coefs.count(vid)==0) {
            this->_coefs[vid]=-coeff;
        }
        else {
            this->_coefs[vid]-=coeff;
        }
        
        if (this->_coefs[vid]==0){
            this->_coefs.erase(vid);
        }
    }
    // Quadratic part
    map<int, double>* qmat = NULL;
    map<int, double>* fqmat = NULL;
    for(auto& it:q._qmatrix) {
        vid = it.first;
        fqmat = it.second;
        if(this->_qmatrix.count(vid)==1)
            qmat = this->_qmatrix[vid];
        else {
            qmat = new map<int, double>();
            this->_qmatrix.insert(pair<int, map<int, double>*> (vid,qmat));
        }
        for(auto& itq:*fqmat) {
            vjd = itq.first;
            coeff = (*qmat)[vjd] - itq.second;
            if(coeff!=0) {
                (*qmat)[vjd] = coeff;
                add_hess_link(vid, vjd);
            }
            else{
                qmat->erase(vjd);
                remove_hess_link(vid,vjd);
                if(qmat->empty()){
                    delete qmat;
                    _qmatrix.erase(vid);
                    update_q_vars(vid);
                    update_q_vars(vjd);
                }
            }
        }
    }
    this->_cst -= q._cst;
    return *this;
    
};

Quadratic& Quadratic::operator*=(const Quadratic& q){
    assert((!is_quadratic() && !q.is_quadratic())); // This function should only be called for the product of two linear/constant functions or the product of a quadratic and a constant
    // Taking care of zero
    if (is_constant() && _cst==0){
        return *this;
    }
    if(q.is_constant() && q._cst==0) {
        reset_coeffs();
        return *this;
    }
    // Taking care of one
    if ((q.is_constant() && q._cst==1)) {
        return *this;
    }
    int vid = 0, vjd = 0;
    var_* vi = NULL;
    var_* vj = NULL;
    int vmin = 0, vmax = 0;
    map<int, double>* qmat = NULL;
    double coeff = 0;
    map<int, double> coefs(_coefs);
    double cst = _cst;
    merge_vars(q);
    map<int, var_*> lin_vars(_lin_vars);
    map<int, var_*> quad_vars(_quad_vars);
    reset_coeffs();
    // Multiply the linear parts
    for(auto it0:coefs)
    {
        vid = it0.first;
        vi = lin_vars.find(vid)->second;
        coeff = it0.second;
        for(auto it:q._coefs)
        {
            vjd = it.first;
            vj = q.get_var(vjd);
            coeff=it0.second*it.second;
            assert(coeff!=0);
            if(vid < vjd) {
                vmin = vid;
                vmax = vjd;
            }
            else {
                vmin = vjd;
                vmax = vid;
            }
            if(_qmatrix.count(vmin)==0){
                qmat = new map<int, double>();
                this->_qmatrix.insert(pair<int, map<int, double>*> (vmin,qmat));
                qmat->insert(pair<int, double> (vmax,coeff));
                _quad_vars.insert(pair<int, var_*>(vid, vi));
                _quad_vars.insert(pair<int, var_*>(vjd, vj));
                add_hess_link(vmin,vmax);
            }
            else {
                qmat = this->_qmatrix[vmin];
                if (qmat->count(vmax)==1) {
                    (*qmat)[vmax] += coeff;
                }
                
                else {
                    qmat->insert(pair<int, double> (vmax,coeff));
                    _quad_vars.insert(pair<int, var_*>(vid, vi));
                    _quad_vars.insert(pair<int, var_*>(vjd, vj));
                    add_hess_link(vmin,vmax);
                }
            }
        }
        if (q._cst!=0) {
            if (this->_coefs.count(vid)==0){
                this->_coefs.insert(pair<int, double> (vid,coeff*q._cst));
                this->_lin_vars.insert(pair<int, var_*> (vid,vi));
            }
            
            else
                this->_coefs[vid]=coeff*q._cst;
        }
    }
    if (cst!=0) {
        for(auto it:q._coefs)
        {
            vjd = it.first;
            vj = q.get_var(vjd);
            coeff = it.second;
            if (this->_coefs.count(vjd)==0){
                this->_coefs.insert(pair<int, double> (vjd,coeff*cst));
                this->_lin_vars.insert(pair<int, var_*> (vjd,vj));
            }
            else
                this->_coefs[vjd]+=coeff*cst;
        }
    }
    this->_cst = cst*q._cst;
    return *this;
};


Quadratic operator+(double cst, Quadratic q){
    return q+=cst;
};

Quadratic operator-(double cst, Quadratic q){
    return (-1*q + cst);
};

Quadratic operator*(double cst, Quadratic q){
    return q*=cst;
};

Function operator/(double cst, Quadratic q){
    return cst/Function(q);
};


Quadratic operator+(Quadratic q, double cst){
    return q+=cst;
};

Quadratic operator-(Quadratic q, double cst){
    return q-=cst;
};

Quadratic operator*(Quadratic q, double cst){
    return q*=cst;
};

Quadratic operator/(Quadratic q, double cst){
    return q*=1/cst;
};


Quadratic operator+(Quadratic q, var_& v){
    return q+=v;
};

Quadratic operator-(Quadratic q, var_& v){
    return q-=v;
};

Function operator*(Quadratic q, var_& v){
    if (!q.is_quadratic()) {
        return Function(q*=v);
    }
    else
        return Function(v) * Function(q);
};

Function operator/(Quadratic q, var_& v){
    int vid = v.get_idx();
    if (q.get_nb_vars()==1 && q._cst==0) {
        if (q._lin_vars.count(vid)==1) {
            if(q._quad_vars.size()==0) {
                return Function(q._coefs[vid]);
            }//else
            return (q._coefs[vid] + q.get_q_coeff(vid, vid)*v);
        }
//        if (q._quad_vars.count(vid)==1) {
            return Function(q.get_q_coeff(vid, vid)*v);
//        }
    }
    return Function(q)/=v;
};



Quadratic operator+(var_& v, Quadratic q){
    return q+=v;
};

Quadratic operator-(var_& v, Quadratic q){
    return (-1*q + v);
};

Function operator*(var_& v, Quadratic q){
    if (!q.is_quadratic()) {
        return Function(q*=v);
    }
    else
        return Function(v) * Function(q);

};

Function operator/(var_& v, Quadratic q){
    int vid = v.get_idx();
    if (q.get_nb_vars()==1 && q._cst==0) {
        if (q._lin_vars.count(vid)==1){
           if(q._quad_vars.size()==0) {
               return Function(q._coefs[vid]);
           }
            return 1/(q._coefs[vid] + q.get_q_coeff(vid, vid)*v);
        }
//        if (q._quad_vars.count(vid)==1) {
            return 1/(q.get_q_coeff(vid, vid)*v);
//        }
    }
    return Function(v)/= q;
};

Function operator/(Quadratic q1, const Quadratic& q2){
//    assert(q1.get_nb_vars()==1 && q2.get_nb_vars()==1);
    if (q1.get_nb_vars()>1 || q2.get_nb_vars()>1) {
            return Function(q1)/= q2;
    }
    var_* v = nullptr;
    if (q1.is_constant()) {
        return q1.get_const()/q2;
    }
    if (q2.is_constant()) {
        return q1/q2.get_const();
    }
    if (q1.is_linear()) {
        v = q1._lin_vars.begin()->second;
        return *v/q2;
    }
    if (q2.is_linear()) {
        v = q2._lin_vars.begin()->second;
        return q1/(*v);
    }
    
    return Function(q1)/= q2;
}

Quadratic operator+(Quadratic q1, const Quadratic& q2){
    return q1+=q2;
};

Quadratic operator-(Quadratic q1, const Quadratic& q2){
    return q1-=q2;
};

Function operator*(Quadratic q1, const Quadratic& q2){
    if (!q1.is_quadratic() && !q2.is_quadratic()) {
        return Function(q1*=q2);
    }
    else
        return Function(q1) * Function(q2);
};


Function sqrt(const Quadratic& q){
    return sqrt(Function(q));
};

Function cos(const Quadratic& q){
    return cos(Function(q));
};


Function sin(const Quadratic& q){
    return sin(Function(q));
};


Quadratic Quadratic::concretise(){
    Quadratic q;
    int vid = 0, vjd = 0;
    var_* vi = NULL;
    var_* vj = NULL;
    meta_var* m_vi = NULL;
    meta_var* m_vj = NULL;
    meta_constant* m_ci = NULL;
    meta_constant* m_cj = NULL;
    double coeff = 0;
    // Linear part
    for(auto& it:_coefs)
    {
        vid = it.first;
        coeff= it.second;
        vi = get_var(vid);
        if (vi->get_type()==constant) {
            m_ci = (meta_constant*)(vi);
            coeff = get_coeff(m_ci->get_idx());
            q += coeff*m_ci->val;
        }
        else {
            m_vi = (meta_var*)(vi);
            coeff = get_coeff(m_vi->get_idx());
            q += coeff*(*m_vi->var);
        }
        
    }
    // Quadratic part
    map<int, double>* qmat = NULL;
    for(auto& it:_qmatrix) {
        vid = it.first;
        vi = get_var(vid);
        qmat = it.second;
        if (vi->get_type()==constant) {
            m_ci = (meta_constant*)(vi);
            for(auto& itq:*qmat) {
                vjd = itq.first;
                vj = get_var(vjd);
                coeff = itq.second;
                if (vj->get_type()==constant) {
                    m_cj = (meta_constant*)(vj);
                    q += coeff*m_ci->val*m_cj->val;
                }
                else {
                    m_vj = (meta_var*)(vj);
                    q += coeff*m_ci->val*(*m_vj->var);
                }
            }
            
        }
        else {
            m_vi = (meta_var*)(vi);
            for(auto& itq:*qmat) {
                vjd = itq.first;
                vj = get_var(vjd);
                coeff = itq.second;
                if (vj->get_type()==constant) {
                    m_cj = (meta_constant*)(vj);
                    q += coeff*m_cj->val*(*m_vi->var);
                }
                else {
                    m_vj = (meta_var*)(vj);
                    q+= (*m_vi->var)*(*m_vj->var);
                    q*= coeff;
                }
            }
            
            
        }
    }
    q += _cst;
    return q;
}

/* Derivatives */
Quadratic Quadratic::get_dfdx(var_* v){
    int vid = v->get_idx();
    int vjd = 0;
    var_* vj = NULL;
    set<int>* hess_set = NULL;
    double coeff = 0;
    Quadratic res(get_coeff(vid));
    if(_hess.count(vid)==0)
        return res;
    hess_set = _hess[vid];
    for (auto& itq:*hess_set) {
        vjd = itq;
        coeff = get_q_coeff(vid, vjd);
        assert(coeff!=0);
        if (vid==vjd) {
            res.add(2*coeff, v);
        }
        else {
            vj = get_var(vjd);
            res.add(coeff, vj);
        }
    }
    return res;
};

double Quadratic::eval_dfdx(int vid, const double* x) const{
    int vjd = 0;
    set<int>* hess_set = NULL;
    double coeff = 0;
    double res = get_coeff(vid);
    if(_hess.count(vid)==0)
        return res;
    hess_set = _hess.find(vid)->second;
    for (auto& itq:*hess_set) {
        vjd = itq;
        coeff = get_q_coeff(vid, vjd);
        assert(coeff!=0);
        if (vid==vjd) {
            res += 2*coeff*x[vid];
        }
        else {
            res += coeff*x[vjd];
        }
    }
    return res;
}


double Quadratic::eval_meta(const double *x, map<int, double>& meta_cons, map<int, var_*>& meta_vars) const{
    double res = 0;
    int vid = 0;
    int vjd = 0;
    double coeff = 0;
    double lhs = 0;
    for(auto& it:_coefs) {
        vid = it.first;
        coeff = it.second;
//        assert(meta_cons.count(vid)+meta_vars.count(vid)==1);
        if (meta_cons.count(vid)==1) {
            res += coeff*meta_cons[vid];
        }
        else if (meta_vars.count(vid)==1) {
            res += coeff*x[meta_vars[vid]->get_idx()];
        }
    }
    for (auto& it:_qmatrix) {
        vid = it.first;
//        assert(meta_cons.count(vid)+meta_vars.count(vid)==1);
        if (meta_cons.count(vid)==1) {
            lhs = meta_cons[vid];
        }
        else if (meta_vars.count(vid)==1) {
            lhs = x[meta_vars[vid]->get_idx()];
        }
        map<int, double>* qcoefs = it.second;
        for (auto& itq:*qcoefs) {
            vjd = itq.first;
            coeff = itq.second;
            if (meta_cons.count(vjd)==1) {
                res += coeff*lhs*meta_cons[vjd];
            }
            else if (meta_vars.count(vjd)==1) {
                res += coeff*lhs*x[meta_vars[vjd]->get_idx()];
            }
        }
    }
    res+= _cst;
    return res;
}



double Quadratic::eval_dfdxdy(int vid, int vjd) const{
    return get_q_coeff(vid, vjd);
};

void Quadratic::reset_coeffs(){
    for (auto& it:_qmatrix){
        delete it.second;
    }
    for (auto& it:_hess){
        delete it.second;
    }
    _lin_vars.clear();
    _quad_vars.clear();
    _qmatrix.clear();
    _hess.clear();
    _coefs.clear();
    _cst = 0;
}


/** Output */
void Quadratic::print() const {
    int vid = 0, vjd = 0;
    double coeff = 0;
    var_* v = NULL;
    var_* vj = NULL;
    map<int, double>* qmat = NULL;
    
    for(auto it = _coefs.cbegin(); it != _coefs.cend(); ++it)
    {
        
        vid = it->first;
        coeff = it->second;
        
        if (coeff==0) {
            continue;
        }
        if (coeff < 0) {
            cout << " - ";
            if(coeff != -1)
                cout << -coeff;
        }
        else {
            if (it == _coefs.cbegin()) {
                if(coeff != 1)
                    cout << coeff;
            }
            else{
                if(coeff != 1)
                    cout << " + " << coeff;
                else
                    cout << " + ";
            }
        }
        v = get_var(vid);
        v->print();
    }
    if (!_coefs.empty() && !_qmatrix.empty() && _qmatrix.begin()->second->begin()->second > 0) {
        cout << " + ";
    }
    for(auto it = _qmatrix.cbegin(); it != _qmatrix.cend(); ++it) {
        vid = it->first;
        qmat = it->second;
        for(auto itq = qmat->cbegin(); itq != qmat->cend(); ++itq) {
            vjd = itq->first;
            coeff = itq->second;
            if (coeff<0) {
                cout << " - ";
                if(coeff!=-1)
                    cout << -coeff;
            }
            else {
                if (it == _qmatrix.cbegin() && itq == qmat->cbegin()) {
                    if(coeff!=1)
                        cout << coeff;
                }
                else{
                    if(coeff!=1)
                        cout << " + " << coeff;
                    else
                        cout << " + ";
                }
            }
            if(vid!=vjd) {
                v = get_var(vid);
                v->print();
                vj = get_var(vjd);
                vj->print();
            }
            else {
                v = get_var(vid);
                v->print();
                cout << "^2";
            }
        }
    }
    if (_cst>0) {
        if (!_coefs.empty()||!_qmatrix.empty()) {
            cout << " + ";
        }
        cout << _cst;
    }
    if (_cst<0) {
        cout << " - " << -_cst;
    }
    
};



