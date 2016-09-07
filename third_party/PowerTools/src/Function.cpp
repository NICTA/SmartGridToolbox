//
//  Function.cpp
//  PowerTools++
//
//  Created by Hassan on 19/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//


#include "PowerTools++/Function.h"
#include "PowerTools++/var.h"
#include "math.h"
#include <map>

using namespace std;

Function::Function():_idx(-1), _name("noname"), _ftype(constant_), _lparent(nullptr), _otype(id_), _rparent(nullptr), _coeff(1){
};

Function::Function(var_& v):Function(){
    _ftype = lin_;
    _quad += v;
    _vars.insert(pair<int, var_*>(v.get_idx(),&v));
};

Function::Function(const Quadratic& q):Function(){
    _ftype = quad_;
    if (q.is_linear()) {
        _ftype = lin_;
    }
    if (q.is_constant()) {
        _ftype = constant_;
    }
    _quad += q;
    merge_vars(_quad);
};

//Function::Function(const Function& lparent, OperatorType otype, const Function& rparent):Function(){
//    _lparent = shared_ptr<Function>(new Function(lparent));
//    _otype = otype;
//    _rparent = shared_ptr<Function>(new Function(rparent));
//    merge_vars(lparent);
//    merge_vars(rparent);
//    update_type();
//}

Function::Function(const Function& f):Function(){
    _name = f._name;
    _idx = f._idx;
    _ftype = f._ftype;
    _quad += f._quad;
    _otype = f._otype;
    _coeff = f._coeff;
//    _val.resize(f._val.size());
    if (f._ftype==nlin_) {
//        assert(f._lparent);
        if(f._lparent)
            _lparent = make_shared<Function>(*f._lparent);
        if(f._rparent)
            _rparent = make_shared<Function>(*f._rparent);

    }
    merge_vars(f);
};

const Quadratic* Function::get_quad() const{
    return &_quad;
}

Function::Function(double cst):Function(){
    _quad += cst;
};

Function::Function(int cst):Function(){
    _quad += (double)cst;
};


bool additive(OperatorType op) {
    return (op==plus_ || op==minus_);
}

bool unary(OperatorType op) {
    return (op==sin_ || op==cos_ || op==power_ || op==exp_ || op==log_ || op==sqrt_);
}


Function::~Function(){
//    delete _val;
//    delete _evaluated;
};

size_t Function::get_nb_vars() const{
    return _vars.size();
}

template<typename Number> var<Number>* Function::get_var_(int idx) const{
    auto it =_vars.find(idx);
    if(it!=_vars.end()){
        var_* v = (it->second);
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

template var<bool>* Function::get_var_(int idx) const;
template var<int>* Function::get_var_(int idx) const;
template var<float>* Function::get_var_(int idx) const;
template var<>* Function::get_var_(int idx) const;


double Function::get_const() const{
    return _quad.get_const();
}

double Function::get_coeff(int vid) const{
    return _quad.get_coeff(vid);
}

bool Function::same(const double *x) const{
    int vid = 0;
    var_* vi = NULL;
    for(auto& it:_vars) {
        vid = it.first;
        vi = it.second;
        if (vi->_dval!=x[vid]) {
            return false;
        }
    }
    return true;
}

bool Function::has_var(int vid) const{
    return _vars.count(vid)==1;
}

var_* Function::get_var(int idx) const{
    if (!has_var(idx)) {
        return NULL;
    }
    var_* v = _vars.find(idx)->second;
    return v;
}

bool Function::has_dfdx(int vid) const{
    return _dfdx.count(vid)==1;
}


bool Function::has_q_var(int vid) const{
    return _quad.has_q_var(vid);
}

bool Function::is_nonlinear() const{
    return (_ftype==nlin_);
//    if (_lparent) {
//        assert(_otype!=id_);
//        return true;
//    }
//    return false;
}

bool Function::is_quadratic() const{
    if(!is_nonlinear() && _quad.is_quadratic()){
        assert(_ftype==quad_);
        return true;
    }
    return false;
}


bool Function::is_linear() const{
    if(!is_nonlinear() && _quad.is_linear()){
        assert(_ftype==lin_);
        return true;
    }
    return false;
}


bool Function::is_constant() const{
    if(!is_nonlinear() && _quad.is_constant()){
        assert(_ftype==constant_);
        return true;
    }
    return false;
}

bool Function::is_leaf() const{
    return !is_nonlinear();
}

bool Function::symmetric(const Function& f) const{

    if ((f._otype!=cos_ && f._otype!=sin_ && f._otype!=plus_ && f._otype!=product_) || (_otype!=cos_ && _otype!=sin_ &&_otype!=plus_ && _otype!=product_)) {
        return false;
    }
    if (f._ftype!= _ftype || f._otype!=_otype || f.get_nb_vars()!=get_nb_vars()) {
        return false;
    }
    if (f._otype!=sin_ && f._coeff!=_coeff){
        return false;
    }
    if (f._quad != _quad) {
        return false;
    }
    if ((f._otype==plus_ || f._otype==product_) && (!_lparent || !_rparent || !f._lparent || !f._rparent)) {
        return false;
    }
    if ((f._otype==plus_ || f._otype==product_) && *_lparent==*f._rparent && *_rparent==*f._lparent) {
//                print(false);
//                cout << " = ";
//                f.print(false);
//                cout << endl;
        return true;
    }
    if (_otype==cos_) {
        Function f2 = -1 * *f._lparent;
        if (*_lparent == *f._lparent || *_lparent == f2)
            return true;
        else {
            print(false);
            cout << endl;
            f.print(false);
        }
    }
    if (_otype==sin_) {
        Function f2 = -1 * *f._lparent;
        if ((*_lparent == *f._lparent && _coeff==f._coeff) || (*_lparent == f2 && _coeff==-f._coeff))
            return true;
//        else {
//            print(false);
//            cout << "!=";
//            f.print(false);
//            cout << endl;
//        }
    }
    return false;
}

double Function::get_q_coeff(int vid1, int vid2) const{
    return _quad.get_q_coeff(vid1, vid2);
}

void Function::add_hess_link(int vid, int vjd){
    if(_hess.count(vid)==0) {
        _hess[vid] = unique_ptr<set<int>>(new set<int>());
    }
    _hess[vid]->insert(vjd);
    if(_hess.count(vjd)==0) {
        _hess[vjd] = unique_ptr<set<int>>(new set<int>());
    }
    else;
        _hess[vjd]->insert(vid);
}

void Function::update_type(){
    if(_lparent) {
        _ftype = nlin_;
        return;
    }
    _ftype = quad_;
    if (_quad.is_linear()) {
        _ftype = lin_;
    }
    if (_quad.is_constant()) {
        _ftype = constant_;
    }
}


void Function::reset(){
    _quad.reset_coeffs();
    _otype = id_;
    if (_lparent) {
        _lparent.reset();
    }
    if (_rparent) {
        _rparent.reset();
    }
    _ftype=constant_;
    _coeff = 1;
    _vars.clear();
    _hess.clear();
}

void Function::resetTree(){
    _otype = id_;
    if (_lparent) {
        _lparent.reset();
    }
    if (_rparent) {
        _rparent.reset();
    }
    _coeff = 1;
    _vars.clear();
    _hess.clear();
    update_type();
}

//void Function::collapse_expr(){ /**< transform the original form: coeff*(_lparent _otype _rparent) + x^T.Q.x + a^T.x + b into 1*(_lparent1 _otype _rparent1) + 0 by putting the quadratic term in the expression tree */
//
//    if (is_nonlinear()) {
//        _lparent = shared_ptr<Function>(new Function(*_lparent.get(), _otype, *_rparent.get()));
//        _otype = plus_;
//        _rparent = shared_ptr<Function>(new Function(_quad));
//    }
//    else{
//        _lparent = shared_ptr<Function>(new Function(_quad));
//        _otype = id_;
//    }
//    _coeff = 1;
//    _quad.reset_coeffs();
//}

void Function::set_dfdx(int vid, shared_ptr<Function> f){
    assert(!has_dfdx(vid));
    _dfdx[vid] = f;
}


/** A Function has the form: (_lparent _otype _rparent) + x^T.Q.x + a^T.x + b */
double Function::eval(const double* x) const{
//    if (_evaluated && same(x)) {
//        return _val;
//    }
//    _evaluated = true;
    double res = 0;
    double q_res = _quad.eval(x);
    if (_ftype <= quad_) {
        return q_res;
    }
    else { // f is a composition of functions
        res = _lparent->eval(x);
        if(_rparent) { // Binary operators
            switch (_otype) {
                case plus_:
                    res += _rparent->eval(x);
                    break;
                case minus_:
                    res -= _rparent->eval(x);
                    break;
                case product_:
                    res *= _rparent->eval(x);
                    break;
                case div_:
                    res /= _rparent->eval(x);
                    break;
                case power_:
                    res = pow(res, _rparent->eval(x));
                    break;
                default:
                    std::cerr << "unsupported operation";
                    exit(-1);
                    break;
            }
        }
        else { // Unary operators
            switch (_otype) {
                case cos_:
                    res = cos(res);
                    break;
                case sin_:
                    res = sin(res);
                    break;
                case log_:
                    res = log(res);
                    break;
                case exp_:
                    res = exp(res);
                    break;
                case sqrt_:
                    res = sqrt(res);
                    break;
                default:
                    std::cerr << "unsupported operation";
                    exit(-1);
                    break;
            }
        }
        res = _coeff*res + q_res;
    }
    return res;
}


/** A Function has the form: (_lparent _otype _rparent) + x^T.Q.x + a^T.x + b */
double Function::eval_meta(const double *x, map<int, double>& meta_coeff, map<int, var_*>& meta_vars, int meta_link){
    if (is_constant()) {
        return get_const();
    }
    if (_evaluated[meta_link]) {
//        cout << "ok\n";
        return _val[meta_link];
    }
    
    double res = 0;
    double q_res = _quad.eval_meta(x, meta_coeff, meta_vars);
    if (_ftype <= quad_) {
        _evaluated[meta_link] = true;
        _val[meta_link] = q_res;
        return q_res;
    }
    else { // f is a composition of functions
//        assert(_lparent->_evaluated[meta_link]);
        res = _lparent->eval_meta(x, meta_coeff, meta_vars, meta_link);
        if(_rparent) { // Binary operators
            switch (_otype) {
                case plus_:
                    res = res + _rparent->eval_meta(x, meta_coeff, meta_vars, meta_link);
                    break;
                case minus_:
                    res = res - _rparent->eval_meta(x, meta_coeff, meta_vars, meta_link);
                    break;
                case product_:
                    res = res*_rparent->eval_meta(x, meta_coeff, meta_vars, meta_link);
                    break;
                case div_:
                    res = res/_rparent->eval_meta(x, meta_coeff, meta_vars, meta_link);
                    break;
                case power_:
                    res = pow(res, _rparent->eval_meta(x, meta_coeff, meta_vars, meta_link));
                    break;
                default:
                    std::cerr << "unsupported operation";
                    exit(-1);
                    break;
            }
        }
        else { // Unary operators
            switch (_otype) {
                case cos_:
                    res = cos(res);
                    break;
                case sin_:
                    res = sin(res);
                    break;
                case log_:
                    res = log(res);
                    break;
                case exp_:
                    res = exp(res);
                    break;
                case sqrt_:
                    res = sqrt(res);
                    break;
                default:
                    std::cerr << "unsupported operation";
                    exit(-1);
                    break;
            }
        }
        res = _coeff*res + q_res;
    }
    _evaluated[meta_link] = true;
    _val[meta_link] = res;
    return res;
}

double Function::eval_dfdx(int vid, const double* x) const {
    if (!has_var(vid)) {
        return 0;
    }
    double q_res = _quad.eval_dfdx(vid, x);
    if (_ftype <= quad_) {
        return q_res;
    }
    else { // f is a composition of functions
        int exp = 0;
        if(_rparent) { // Binary operators
            switch (_otype) {
                case plus_:
                    return _coeff*(_lparent->eval_dfdx(vid, x)+_rparent->eval_dfdx(vid, x)) + q_res;
                    break;
                case minus_:
                    return _coeff*(_lparent->eval_dfdx(vid, x)-_rparent->eval_dfdx(vid, x)) + q_res;
                    break;
                case product_:
                    return _coeff*(_lparent->eval_dfdx(vid, x)*_rparent->eval(x) + _lparent->eval(x)*_rparent->eval_dfdx(vid, x)) + q_res;// f'g + fg'
                    break;
                case div_:
                    return _coeff*((_lparent->eval_dfdx(vid, x)*_rparent->eval(x) - _lparent->eval(x)*_rparent->eval_dfdx(vid, x))/pow(_rparent->eval(x), 2)) + q_res;// (f'g - fg')/g^2
                    break;
                case power_:
                    if (_rparent->_ftype!=constant_) {
                        std::cerr << "Function in exponent not supported yet.\n";
                        exit(-1);
                    }
                    exp = _rparent->_quad._cst;
                    return _coeff*(exp*_lparent->eval_dfdx(vid, x)*pow(_lparent->eval(x), exp-1));// nf'f^n-1
                    break;
                default:
                    std::cerr << "unsupported operation";
                    exit(-1);
                    break;
            }
        }
        else { // Unary operators
            // f(g(x))' = f'(g(x))*g'(x).
            switch (_otype) {
                case cos_:
                    return _coeff*(-_lparent->eval_dfdx(vid, x)*sin(_lparent->eval(x))) + q_res;
                    break;
                case sin_:
                    return _coeff*(_lparent->eval_dfdx(vid, x)*cos(_lparent->eval(x))) + q_res;
                    break;
                case sqrt_:
                    return _coeff*(_lparent->eval_dfdx(vid, x)/(2*sqrt(_lparent->eval(x)))) + q_res;
                    break;
                case exp_:
                    return _coeff*(_lparent->eval_dfdx(vid, x)*exp2(_lparent->eval(x))) + q_res;
                    break;
                case log_:
                    return _coeff*(_lparent->eval_dfdx(vid, x)/_lparent->eval(x)) + q_res;
                    break;
                default:
                    std::cerr << "ok unsupported unary operation";
                    exit(-1);
                    break;
            }
        }
    }
}


shared_ptr<Function> Function::getTree() const{ /**< returns coeff*(_lparent _otype _rparent) */
    shared_ptr<Function> res(new Function());
    res->_coeff = _coeff;
    res->_otype = _otype;
    int vid = 0;
    for(auto& it:_hess)
    {
        vid = it.first;
        for(int vjd:*it.second)
        {
            res->add_hess_link(vid, vjd);
        }
    }
    res->_lparent = shared_ptr<Function>(new Function(*_lparent.get()));
    res->merge_vars(*_lparent);
    if (_rparent) {
        res->_rparent = shared_ptr<Function>(new Function(*_rparent.get()));
        res->merge_vars(*_rparent);
    }
    res->update_type();
    return res;
}

void Function::full_hess(){
    int vid = 0, vjd = 0;
    var_* vi = NULL;
    var_* vj = NULL;
    for(auto& it:_vars) {
        vid = it.first;
        vi = it.second;
        if(vi->is_constant())
            continue;
        for(auto& it2:_vars) {
            vjd = it2.first;
            vj = it2.second;
            if(vj->is_constant())
                continue;
            add_hess_link(vid, vjd);
        }
    }
}

void Function::full_hess(const Function& f){
    int vid = 0, vjd = 0;
    var_* vi = NULL;
    var_* vj = NULL;
    for(auto& it:_vars) {
        vid = it.first;
        vi = it.second;
        if(vi->is_constant())
            continue;
        for(auto& it2:f._vars) {
            vjd = it2.first;
            vj = it2.second;
            if(vj->is_constant())
                continue;
            add_hess_link(vid, vjd);
        }
    }
}




void Function::compute_dfdx(var_* v){
    int vid = v->get_idx();
    if (_dfdx.count(vid)==1) {
        return;
    }
    if(!has_var(vid)){
//        assert(false);
        _dfdx[vid]= shared_ptr<Function>(new Function(0));
//        _dfdx[vid]->_val.resize(_val.size());
    }
    shared_ptr<Function> df(new Function(_quad.get_dfdx(v)));
//    df->_val.resize(_val.size());
    if(_ftype<=quad_){
        _dfdx[vid] = df;
    }
    else {
        double exp = 0;
//        _lparent->_val.resize(_val.size());
        _lparent->compute_dfdx(v);
        if(_rparent) { // Binary operators
//            _rparent->_val.resize(_val.size());
            _rparent->compute_dfdx(v);
            switch (_otype) {
                case plus_:
                    *df += _coeff*(*_lparent->_dfdx[vid] + *_rparent->_dfdx[vid]);
                    _dfdx[vid] = (df);
                    break;
                case minus_:
                    *df += _coeff*(*_lparent->_dfdx[vid] - *_rparent->_dfdx[vid]);
                    _dfdx[vid] = (df);
                    break;
                case product_:
                    *df += _coeff*((*_lparent->_dfdx[vid]) * (*_rparent) + (*_rparent->_dfdx[vid]) * (*_lparent));// (f'g + fg')
                    _dfdx[vid] = (df);
                    break;
                case div_:
                    *df += _coeff*((*_lparent->_dfdx[vid]) * (*_rparent) - (*_rparent->_dfdx[vid]) * (*_lparent))/((*_rparent)^2);// (f'g - fg')/g^2
                    _dfdx[vid] = (df);
                    break;
                case power_:
                    if (!_rparent->is_leaf() && !_rparent->is_constant()) {
                        std::cerr << "Function in exponent not supported yet.\n";
                        exit(-1);
                    }
                    exp = _rparent->get_const();
                    *df += _coeff*exp*((*_lparent->_dfdx[vid]) * ((*_lparent)^(exp-1)));// nf'f^n-1
                    _dfdx[vid] = (df);
                    break;
                default:
                    std::cerr << "unsupported operation";
                    exit(-1);
                    break;
            }
        }
        else { // Unary operators
            // f(g(x))' = f'(g(x))*g'(x).
            switch (_otype) {
                case cos_:
                    *df += -_coeff * (*_lparent->_dfdx[vid]) * sin(*_lparent);
                    _dfdx[vid] = (df);
                    break;
                case sin_:
                    *df += _coeff * (*_lparent->_dfdx[vid]) * cos(*_lparent);
                    _dfdx[vid] = (df);
                    break;
                case exp_:
                    *df += _coeff * (*_lparent->_dfdx[vid]) * expo(*_lparent);
                    _dfdx[vid] = (df);
                    break;
                case log_:
                    *df += _coeff * (*_lparent->_dfdx[vid])/(*_lparent);
                    _dfdx[vid] = (df);
                    break;
                case sqrt_:
                    *df += _coeff/2 * (*_lparent->_dfdx[vid])/sqrt(*_lparent);
                    _dfdx[vid] = (df);
                    break;
                default:
                    std::cerr << "ouch unsupported unary operation";
                    exit(-1);
                    break;
            }
        }
        
        //        cerr << "Not supported";
        //        exit(-1);
    }
}

void Function::merge_dfdx(const Function& f1){
    for(auto& it:f1._dfdx){
        shared_ptr<Function> df = it.second;
        _dfdx.insert(pair<int, shared_ptr<Function>>(it.first, df));
    }
}

void Function::merge_vars(const Function& f1){
    var_* vi = NULL;
    int vid = 0;
    for(auto& it:f1._vars)
    {
        vid = it.first;
        vi = it.second;
        _vars.insert(pair<int, var_*> (vid,vi));
    }
    for(auto& it:f1._hess)
    {
        vid = it.first;
        for(int vjd:*it.second)
        {
            add_hess_link(vid, vjd);
        }
    }
//    for(auto& it:f1._dfdx){
//        _dfdx.insert(pair<int, shared_ptr<Function>>(it.first, it.second));
//    }
}

Function Function::outer_approx(const double* x) const{
    Function res; // res = gradf(x*)*(x-x*) + f(x*)
    for(auto it: _vars){
        res -= eval_dfdx(it.first, x)*(x[it.first]-*(it.second));
    }
    res += eval(x);
    return res;
}

Function Function::quad_ch_right(var<>& x, var<>& z, var<bool>& u, double *vals) const{
    Function res, z_of_x;
    double a, b, c;
    assert(is_quadratic());
    a = _quad.get_q_coeff(x.get_idx(), x.get_idx()); // a should be > 0, vals[x] should be set
    b = _quad.get_coeff(x.get_idx());
    c = _quad.get_const();
    if (_quad.get_coeff(z.get_idx()) == -1) res += x - x.get_ub_off()*(1-u) + (b/(2*a))*u - sqrt(((b*b - 4*a*c)/(4*a*a))*u*u + z*u/a);
    if (_quad.get_coeff(z.get_idx()) == 1) res += x - x.get_ub_off()*(1-u) + (b/(2*a))*u - sqrt(((b*b - 4*a*c)/(4*a*a))*u*u - z*u/a);

    // evaluating at on = 1
    z_of_x = a*(b/(2*a) + x)*(b/(2*a) + x) - (b*b - 4*a*c)/(4*a);
    vals[z.get_idx()] = z_of_x.eval(vals);
    if(_quad.get_coeff(z.get_idx()) == 1) vals[z.get_idx()] = -vals[z.get_idx()];
    vals[u.get_idx()] = 1;

    return res.outer_approx(vals);
}

Function Function::quad_ch_left(var<>& x, var<>& z, var<bool>& u, double *vals) const{
    Function res, z_of_x;
    double a, b, c;
    assert(is_quadratic());
    a = _quad.get_q_coeff(x.get_idx(), x.get_idx()); // a should be > 0, vals[x] should be set
    b = _quad.get_coeff(x.get_idx());
    c = _quad.get_const();
    if (_quad.get_coeff(z.get_idx()) == -1) res += -1*x + x.get_lb_off()*(1-u) - (b/(2*a))*u - sqrt(((b*b - 4*a*c)/(4*a*a))*u*u + z*u/a);
    if (_quad.get_coeff(z.get_idx()) == 1) res += -1*x + x.get_lb_off()*(1-u) - (b/(2*a))*u - sqrt(((b*b - 4*a*c)/(4*a*a))*u*u - z*u/a);

    // evaluating at on = 1
    z_of_x = a*(b/(2*a) + x)*(b/(2*a) + x) - (b*b - 4*a*c)/(4*a);
    vals[z.get_idx()] = z_of_x.eval(vals);
    if(_quad.get_coeff(z.get_idx()) == 1) vals[z.get_idx()] = -vals[z.get_idx()];
    vals[u.get_idx()] = 1;

    return res.outer_approx(vals);
}

void Function::merge_vars(const Quadratic& q){
    int vid = 0;
    var_* v = NULL;
    set<int>* hess = NULL;
    for(auto& it:q._lin_vars)
    {
        vid = it.first;
        v = it.second;
        _vars.insert(pair<int, var_*>(vid,v));
    }
    for(auto& it:q._quad_vars)
    {
        vid = it.first;
        v = it.second;
        _vars.insert(pair<int, var_*>(vid,v));
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

void Function::cloneTree(const Function& f){
    resetTree(); // remove all vars, we don't know which are in the _quad part
    merge_vars(_quad); // reinsert the variables of the _quad part
    _coeff = f._coeff;
    _lparent = make_shared<Function>(*f._lparent);
    _otype = f._otype;
    if(f._rparent) {
        _rparent = make_shared<Function>(*f._rparent);
    }
    merge_vars(f);
    update_type();
}

//bool Function::operator==(const Function& f) const{
//    if (f._ftype!= _ftype || f._otype!=_otype || f._coeff!=_coeff || f.get_nb_vars()!=get_nb_vars()) {
//        return false;
//    }
//    if (f._quad != _quad) {
//        return false;
//    }
//    if (!f.is_nonlinear() && !is_nonlinear()) { // If both are quadratic stop
//        return true;
//    }
//    
//    if (f._lparent==_lparent && f._rparent == _rparent) {
//        return true;
//    }
//    
//    if ((f._lparent && !_lparent) && (*f._lparent == *_rparent) && !_rparent) {
//        return true;
//    }
//    if ((f._rparent && !_rparent) || (_rparent && !f._rparent)) {
//        return false;
//    }
//    
//    if ((*f._lparent == *_lparent) {
//        if (f._rparent && *f._rparent != *_rparent) {
//            return false;
//        }
//    }
//        
//        //    print(false);
//        //    cout << " = ";
//        //    f.print(false);
//        //    cout << endl;
//        return false;
//        }

        
bool Function::operator==(const Function& f) const{
    if (f._ftype!= _ftype || f._otype!=_otype || f.get_nb_vars()!=get_nb_vars()) {
        return false;
    }
    if (f._quad != _quad) {
        return false;
    }
//    if (f._dfdx.size()!=_dfdx.size()) {
//        return false;
//    }
    if (f._coeff==_coeff && f._lparent==_lparent && f._rparent == _rparent) {
        return true;
    }
    if ((f._lparent && !_lparent) || (_lparent && !f._lparent)) {
        return false;
    }
    if ((f._rparent && !_rparent) || (_rparent && !f._rparent)) {
        return false;
    }
    if (symmetric(f)) {
//    if (_dfdx.size()!= f._dfdx.size()) {
//        print(false);
//        cout << " = ";
//        f.print(false);
//        cout << endl;
//    }
//        merge_dfdx(f);
//        
//                    assert(false);
//        if (_dfdx.size()!= f._dfdx.size()) {
//            f.print(false);
//            cout << " dfdx size of f = " << f._dfdx.size();
//            cout << endl;
//            print(false);
//            cout << " dfdx size of this = " << _dfdx.size();
//            cout << endl;
//            return true;
//        }
        return true;
    }

    if (*f._lparent != *_lparent) {
        return false;
    }
    if (f._rparent && *f._rparent != *_rparent) {
        return false;
    }
//    print(false);
//    cout << " = ";
//    f.print(false);
//    cout << endl;
    if(f._coeff==_coeff)
        return true;
    return false;
}

bool Function::operator!=(const Function& f) const{
    return !(*this==f);
}


Function& Function::operator+=(double cst) {
    _quad+=cst;
    return (*this);
};

Function& Function::operator-=(double cst) {
    _quad -= cst;
    return (*this);
};


Function& Function::operator*=(double cst) {
    if(is_constant() && get_const()==0){
        return *this;
    }
    if (cst==0) {
        reset();
        return *this;
    }
    _quad *= cst;
    if (is_nonlinear()) {
        _coeff *= cst;
    }
    return (*this);
};



Function& Function::operator+=(const Quadratic& q) {
    _quad += q;
    merge_vars(_quad);
    update_type();
    return (*this);
};

Function& Function::operator-=(const Quadratic& q) {
    _quad -= q;
    merge_vars(_quad);
    update_type();
    return (*this);
};


Function& Function::operator*=(const Quadratic& q) {
    if(is_constant()){
        if(get_const()==0){
            return *this;
        }
        double cst = get_const();
        *this = Function(q);
        *this *= cst;
        return *this;
    }
    if(q.is_constant()){
        if (q._cst==0) {
            reset();
            return *this;
        }
        _quad *= q._cst;
        _coeff *= q._cst;
        update_type();
        return *this;
    }
    if (_ftype<=lin_ && (q.is_linear()|| q.is_constant())) {
        _quad *= q;
        merge_vars(_quad);
        update_type();
        return (*this);
    }
    assert(_lparent);
//    if (is_quadratic() && _quad==q) {
//        *this = *this^2;
//        return (*this);
//    }
    //    collapse_expr();
    _lparent = shared_ptr<Function>(new Function(*this));
    _otype = product_;
    _rparent = shared_ptr<Function>(new Function(q));
    _coeff = 1;
    _quad.reset_coeffs();
    merge_vars(q);
    full_hess(q);
    update_type();
    return *this;
};

Function& Function::operator+=(const Function& f) {
    *this += f._quad;
    if (f.is_nonlinear()) {
        shared_ptr<Function> ftree = f.getTree();
        merge_vars(*ftree);
        if (is_nonlinear()) {
            _lparent = getTree();
            _otype = plus_;
            _rparent = ftree;
            _coeff = 1;
        }
        else{
            cloneTree(f);
        }
    }
    update_type();
    return (*this);
};


Function operator+(Function f1, const Function& f2){
    return f1 += f2;
}

Function& Function::operator-=(const Function& f) {
    *this += -1*f;
    return (*this);
};

Function operator-(Function f1, const Function& f2){
    return f1 -= f2;
}


Function& Function::operator*=(const Function& f) {
    if(f.is_constant()){
        if (f.get_const()==0) {
            reset();
            return *this;
        }
        _quad *= f.get_const();
        _coeff *= f.get_const();
        update_type();
        return *this;
    }
    if(is_constant()){
        if(get_const()==0){
            return *this;
        }
        double cst = get_const();
        *this = f;
        *this *= cst;
        return *this;
    }
    
    if (_ftype<=lin_ && (f._ftype==lin_|| f._ftype==constant_)) {
        _quad *= f._quad;
        merge_vars(_quad);
        update_type();
        return (*this);
    }
//    if (*this==f) {
//        *this = *this^2;
//        return (*this);
//    }
    _lparent = shared_ptr<Function>(new Function(*this));
    _rparent = shared_ptr<Function>(new Function(f));
    _otype = product_;
    _coeff = 1;
    _quad.reset_coeffs();
    full_hess(f);
    merge_vars(f);
    update_type();
    return (*this);
};

Function& Function::operator/=(const Function& f) {
    if(is_constant() && get_const()==0){
        return *this;
    }
    if(f.is_constant()){
        if (f.get_const()==0) {
            cerr << "dividing by zero!\n";
            exit(-1);
        }
        _quad *= 1/f.get_const();
        _coeff *= 1/f.get_const();
        return *this;
    }
    if (!f.is_nonlinear() && !is_nonlinear() && f.get_nb_vars()==1 && get_nb_vars()==1) {
        var_* v = f._vars.begin()->second;
        int vid = v->get_idx();
        if (has_var(vid)) {
            *this = _quad/f._quad;
            update_type();
            return *this;
        }
    }
    _lparent = shared_ptr<Function>(new Function(*this));
    _rparent = shared_ptr<Function>(new Function(f));
    _otype = div_;
    _coeff = 1;
    _quad.reset_coeffs();
    full_hess(f);
    merge_vars(f);
    update_type();
    return (*this);
};

Function operator^(Function f, int p){

    if (p==1) {
        return f;
    }
    if (p==2 && f.is_linear()) {
        return f*f;
    }
    //    f.collapse_expr();
    f._lparent = shared_ptr<Function>(new Function(f));
    f._rparent = shared_ptr<Function>(new Function(p));
    f._otype = power_;
    f._coeff = 1;
    f._quad.reset_coeffs();
    f.full_hess();
    f.update_type();
    return f;
}

Function cos(Function& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = cos_;
    res.full_hess();
    res.update_type();
    return res;
}


Function sin(Function& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = sin_;
    res.full_hess();
    res.update_type();
    return res;
}

Function sqrt(Function& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = sqrt_;
    res.full_hess();
    res.update_type();
    return res;
}

Function expo(Function& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = exp_;
    res.full_hess();
    res.update_type();
    return res;
}


Function log(Function& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = log_;
    res.full_hess();
    res.update_type();
    return res;
}



Function cos(Function&& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = cos_;
    res.full_hess();
    res.update_type();
    return res;
}

Function sin(Function&& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = sin_;
    res.full_hess();
    res.update_type();
    return res;
}

Function sqrt(Function&& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = sqrt_;
    res.full_hess();
    res.update_type();
    return res;
}

Function expo(Function&& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = exp_;
    res.full_hess();
    res.update_type();
    return res;
}

Function log(Function&& f){
    Function res;
    res.merge_vars(f);
    res._lparent = make_shared<Function>(f);
    res._rparent = nullptr;
    res._otype = log_;
    res.full_hess();
    res.update_type();
    return res;
}

Function operator*(Function f1, const Function& f2){
    return f1 *= f2;
}

Function operator/(Function f1, const Function& f2){
    return f1 /= f2;
}


Function operator+(Function f1, double cst){
    return f1 += cst;
}

Function operator-(Function f1, double cst){
    return f1 -= cst;
}

Function operator*(Function f1, double cst){
    return f1 *= cst;
}


Function operator/(Function f1, double cst){
    return f1 /= cst;
}

Function operator+(double cst, Function f1){
    return f1 += cst;
}

Function operator-(double cst, Function f1){
    return (-1*f1 + cst);
}

Function operator*(double cst, Function f1){
    return f1 *= cst;
}


Function operator/(double cst, Function f1){
    return Function(cst) /= f1;
}




Function& Function::operator+=(var_& v) {
    _quad += v;
    merge_vars(_quad);
    update_type();
    return (*this);
};

Function& Function::operator-=(var_& v) {
    _quad -= v;
    merge_vars(_quad);
    update_type();
    return (*this);
};

Function& Function::operator*=(var_& v) {
    if(is_constant() && get_const()==0){
        return *this;
    }
    Quadratic temp(v);
    *this *= temp;
    update_type();
    return (*this);
    
};


Function Function::concretise(){
    if (is_leaf()) {
        return Function(_quad.concretise());
    }
    Function res = _lparent->concretise();
    if(_rparent) { // Binary operators
        switch (_otype) {
            case plus_:
                res += _rparent->concretise();
                break;
            case minus_:
                res -= _rparent->concretise();
                break;
            case product_:
                res *= _rparent->concretise();
                break;
            case div_:
                res /= _rparent->concretise();
                break;
            case power_:
                assert(_rparent->is_constant());
                res = res^(_rparent->get_const());
                break;
            default:
                std::cerr << "unsupported operation";
                exit(-1);
                break;
        }
    }
    else { // Unary operators
        switch (_otype) {
            case cos_:
                res = cos(res);
                break;
            case sin_:
                res = sin(res);
                break;
            case exp_:
                res = expo(res);
                break;
            case log_:
                res = log(res);
                break;
            case sqrt_:
                res = sqrt(res);
                break;

            default:
                std::cerr << "unsupported operation";
                exit(-1);
                break;
        }
    }
    return _coeff*res + _quad.concretise();
}


Function& Function::operator=(const Function& f){
    reset();
    _name = f._name;
    _idx = f._idx;
    _ftype = f._ftype;
    *this += f._quad;
    _coeff = f._coeff;
//    _val.resize(f._val.size());
//    _val = 0;
//    _evaluated[meta_link] = false;
    if (f.is_nonlinear()) {
        cloneTree(f);
    }
    //    merge_vars(f);
    return *this;
}


void Function::print_domain() const {
    cout << "f(";
    if (!_vars.empty()) {
        auto end = _vars.cend();
        end--;
        for(auto it = _vars.cbegin(); it != _vars.cend(); ++it)
        {
            it->second->print();
            if(it != end)
                cout << " , ";
        }
    }
    cout << ") = ";
}

void Function::print_expr(bool brackets) const {

    if(_otype==id_){
        _quad.print();
        return;
    }
    if (_coeff!=0 && _coeff!=1 && _coeff!=-1) {
        cout << _coeff;
    }
    if (_coeff==-1) {
        cout << "-";
    }

    if (_coeff != 0 && _coeff!=1 && _lparent->get_nb_vars()>1) {
        cout << "(";
    }
    if(_otype==cos_) {
        cout << "cos";
    }
    if(_otype==sin_) {
        cout << "sin";
    }
    if (_otype==exp_) {
        cout << "exp";
    }
    if (_otype==log_) {
        cout << "log";
    }
    if (_otype==sqrt_) {
        cout << "sqrt";
    }
    if(brackets && _rparent)
        cout << "(";
    if (_otype!=product_ || !_lparent->is_constant() || _lparent->get_const()!=1) {

        if (_lparent->is_leaf()) {
            if((_otype!=plus_ && _otype!=minus_&& _lparent->get_nb_vars()>1) || _otype==sqrt_ || _otype==sin_ || _otype==cos_|| _otype==exp_ || _otype==log_){
                cout << "(";
                _lparent->_quad.print();
                cout << ")";
            }
            else
                _lparent->_quad.print();
        }
        else {
            if(_otype!=plus_ && _otype!=minus_) {
                _lparent->print_expr(true);
            }
            else
                _lparent->print_expr(false);
        }

    }
    if (_otype==plus_) {
        cout << " + ";
    }
    if (_otype==minus_) {
        cout << " - ";
    }
    if (_otype==product_) {
        if(!_lparent || !_lparent->is_constant())
            cout << " * ";
    }
    if (_otype==div_) {
        cout << "/";
    }

    if (_otype==power_) {
        cout << "^";
        //        if (_rparent->get_const()==1) {
        //            cerr << "To the power of one!\n";
        //            exit(-1);
        //        }
    }

    if (_rparent) {
        if (_otype==plus_ || _rparent->get_nb_vars()<=1) {
            _rparent->print(false);
        }
        else {
            cout << "(";
            _rparent->print(false);
            cout << ")";
        }
        if(brackets)
            cout << ")";
    }
    if (_coeff != 0 && _coeff!=1 && _lparent->get_nb_vars()>1) {
        cout << ")";
    }
    
    
}


void Function::print(bool domain) const {
    
    if (domain) {
        print_domain();
    }
    if (is_nonlinear()) {
        print_expr(false);
    }
    if (_lparent && (!_quad.is_constant() ||  _quad.get_const() > 0)) {
        cout << " + ";
    }
    _quad.print();
    //    cout << endl;
    if (domain) {
        switch (_ftype) {
            case constant_:
                cout << "\nf is constant_.\n";
                break;
            case lin_:
                cout << "\nf is linear.\n";
                break;
            case quad_:
                cout << "\nf is quadratic.\n";
                break;
                
            default:
                cout << "\nf is nonlinear.\n";
                break;
        }
    }
}
