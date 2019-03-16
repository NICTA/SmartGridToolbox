// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "YamlSupport.h"

namespace YAML
{
    using namespace arma;
    using namespace boost::posix_time;
    using namespace Sgt;
    using namespace std;
    
    Node convert<nullptr_t>::encode(const nullptr_t& from)
    {
        return Node();
    }

    bool convert<nullptr_t>::decode(const Node& nd, nullptr_t& to)
    {
        bool result = false;
        if (nd.Type() == YAML::NodeType::Null)
        {
            to = nullptr;
            result = true;
        }
        return result;
    }

    Node convert<json>::encode(const json& from)
    {
        YAML::Node nd = YAML::Load(from.dump());
        return nd;
    }

    bool convert<json>::decode(const Node& nd, json& to)
    {
        // TODO: actually try to reconstruct json from YAML.
        to = nd.as<string>();
        return true;
    }

    Node convert<Complex>::encode(const Complex& from)
    {
        Node nd(to_string(from));
        return nd;
    }

    bool convert<Complex>::decode(const Node& nd, Complex& to)
    {
        to = from_string<Complex>(nd.as<string>());
        return true;
    }

    Node convert<Phase>::encode(const Phase& from)
    {
        Node nd(to_string(from));
        return nd;
    }

    bool convert<Phase>::decode(const Node& nd, Phase& to)
    {
        to = from_string<Phase>(nd.as<string>());
        return true;
    }

    Node convert<Phases>::encode(const Phases& from)
    {
        Node nd;
        for (const auto& phase: from)
        {
            nd.push_back(to_string(phase));
        }
        return nd;
    }

    bool convert<Phases>::decode(const Node& nd, Phases& to)
    {
        if(!nd.IsSequence())
        {
            return false;
        }
        else
        {
            vector<Phase> pVec; 
            for (size_t i = 0; i < nd.size(); ++i)
            {
                pVec.push_back(nd[i].as<Phase>());
            }
            to = Phases(pVec);
        }
        return true;
    }

    Node convert<BusType>::encode(const BusType& from)
    {
        Node nd(to_string(from));
        return nd;
    }

    bool convert<BusType>::decode(const Node& nd, BusType& to)
    {
        to = from_string<BusType>(nd.as<string>());
        return true;
    }

    Node convert<Time>::encode(const Time& from)
    {
        Node nd(boost::posix_time::to_simple_string(from));
        return nd;
    }

    bool convert<Time>::decode(const Node& nd, Time& to)
    {
        to = boost::posix_time::duration_from_string(nd.as<string>());
        return true;
    }

    Node convert<ptime>::encode(const ptime& from)
    {
        Node nd(boost::posix_time::to_simple_string(from));
        return nd;
    }

    bool convert<ptime>::decode(const Node& nd, ptime& to)
    {
        to = boost::posix_time::time_from_string(nd.as<string>());
        return true;
    }

    template<typename T> Node convert<Col<T>>::encode(const Col<T>& from)
    {
        Node nd;
        for (const T& val : from) nd.push_back(val);
        return nd;
    }
    template Node convert<Col<double>>::encode(const Col<double>& from);
    template Node convert<Col<Complex>>::encode(const Col<Complex>& from);
    template Node convert<Col<int>>::encode(const Col<int>& from);
    template Node convert<Col<uword>>::encode(const Col<uword>& from);

    template<typename T> bool convert<Col<T>>::decode(const Node& nd, Col<T>& to)
    {
        if(!nd.IsSequence())
        {
            return false;
        }
        else
        {
            to = Col<T>(nd.size());
            for (uword i = 0; i < nd.size(); ++i)
            {
                to(i) = nd[i].as<T>();
            }
        }
        return true;
    }
    template bool convert<Col<double>>::decode(const Node& nd, Col<double>& to);
    template bool convert<Col<Complex>>::decode(const Node& nd, Col<Complex>& to);
    template bool convert<Col<int>>::decode(const Node& nd, Col<int>& to);
    template bool convert<Col<uword>>::decode(const Node& nd, Col<uword>& to);

    template<typename T> Node convert<Mat<T>>::encode(const Mat<T>& from)
    {
        Node nd;
        for (uword i = 0; i < from.n_rows; ++i)
        {
            Node nd1;
            for (uword k = 0; k < from.n_cols; ++k)
            {
                nd1.push_back(from(i, k));
            }
            nd.push_back(nd1);
        }
        return nd;
    }
    template Node convert<Mat<double>>::encode(const Mat<double>& from);
    template Node convert<Mat<Complex>>::encode(const Mat<Complex>& from);
    template Node convert<Mat<int>>::encode(const Mat<int>& from);
    template Node convert<Mat<uword>>::encode(const Mat<uword>& from);

    template<typename T> bool convert<Mat<T>>::decode(const Node& nd, Mat<T>& to)
    {
        if(!nd.IsSequence())
        {
            return false;
        }
        else
        {
            auto nrows = nd.size();
            if (nrows == 0)
            {
                cerr << "Matrix has no rows in yaml." << endl;
                return false;
            }
            auto ncols = nd[0].size();
            if (ncols == 0)
            {
                cerr << "Matrix has no columns in yaml." << endl;
                return false;
            }
            for (size_t i = 1; i < nrows; ++i)
            {
                if (nd[i].size() != ncols)
                {
                    cerr << "Ill-formed matrix in yaml." << endl;
                    return false;
                }
            }
            to = Mat<T>(nrows, ncols);
            for (uword i = 0; i < nrows; ++i)
            {
                for (uword k = 0; k < ncols; ++k)
                {
                    to(i, k) = nd[i][k].as<T>();
                }
            }
        }
        return true;
    }
    template bool convert<Mat<double>>::decode(const Node& nd, Mat<double>& to);
    template bool convert<Mat<Complex>>::decode(const Node& nd, Mat<Complex>& to);
    template bool convert<Mat<int>>::decode(const Node& nd, Mat<int>& to);
    template bool convert<Mat<uword>>::decode(const Node& nd, Mat<uword>& to);
}
