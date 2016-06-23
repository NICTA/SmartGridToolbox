// Copyright 2015 National ICT Australia Limited (NICTA)
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
    using namespace Sgt;
    
    Node convert<std::nullptr_t>::encode(const std::nullptr_t& from)
    {
        return Node();
    }

    bool convert<std::nullptr_t>::decode(const Node& nd, std::nullptr_t& to)
    {
        bool result = false;
        if (nd.Type() == YAML::NodeType::Null)
        {
            to = nullptr;
            result = true;
        }
        return result;
    }

    Node convert<Complex>::encode(const Complex& from)
    {
        Node nd(to_string(from));
        return nd;
    }

    bool convert<Complex>::decode(const Node& nd, Complex& to)
    {
        to = from_string<Complex>(nd.as<std::string>());
        return true;
    }

    Node convert<Phase>::encode(const Phase& from)
    {
        Node nd(to_string(from));
        return nd;
    }

    bool convert<Phase>::decode(const Node& nd, Phase& to)
    {
        to = from_string<Phase>(nd.as<std::string>());
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
            std::vector<Phase> pVec; 
            for (std::size_t i = 0; i < nd.size(); ++i)
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
        to = from_string<BusType>(nd.as<std::string>());
        return true;
    }

    Node convert<Time>::encode(const Time& from)
    {
        Node nd(posix_time::to_simple_string(from));
        return nd;
    }

    bool convert<Time>::decode(const Node& nd, Time& to)
    {
        to = posix_time::duration_from_string(nd.as<std::string>());
        return true;
    }

    Node convert<posix_time::ptime>::encode(const posix_time::ptime& from)
    {
        Node nd(posix_time::to_simple_string(from));
        return nd;
    }

    bool convert<posix_time::ptime>::decode(const Node& nd, posix_time::ptime& to)
    {
        to = posix_time::time_from_string(nd.as<std::string>());
        return true;
    }

    template<typename T> Node convert<arma::Col<T>>::encode(const arma::Col<T>& from)
    {
        Node nd;
        for (const T& val : from) nd.push_back(val);
        return nd;
    }
    template Node convert<arma::Col<double>>::encode(const arma::Col<double>& from);
    template Node convert<arma::Col<Complex>>::encode(const arma::Col<Complex>& from);
    template Node convert<arma::Col<int>>::encode(const arma::Col<int>& from);

    template<typename T> bool convert<arma::Col<T>>::decode(const Node& nd, arma::Col<T>& to)
    {
        if(!nd.IsSequence())
        {
            return false;
        }
        else
        {
            to = arma::Col<T>(nd.size());
            for (arma::uword i = 0; i < nd.size(); ++i)
            {
                to(i) = nd[i].as<T>();
            }
        }
        return true;
    }
    template bool convert<arma::Col<double>>::decode(const Node& nd, arma::Col<double>& to);
    template bool convert<arma::Col<Complex>>::decode(const Node& nd, arma::Col<Complex>& to);
    template bool convert<arma::Col<int>>::decode(const Node& nd, arma::Col<int>& to);

    template<typename T> Node convert<arma::Mat<T>>::encode(const arma::Mat<T>& from)
    {
        Node nd;
        for (arma::uword i = 0; i < from.n_rows; ++i)
        {
            Node nd1;
            for (arma::uword k = 0; k < from.n_cols; ++k)
            {
                nd1.push_back(from(i, k));
            }
            nd.push_back(nd1);
        }
        return nd;
    }
    template Node convert<arma::Mat<double>>::encode(const arma::Mat<double>& from);
    template Node convert<arma::Mat<Complex>>::encode(const arma::Mat<Complex>& from);
    template Node convert<arma::Mat<int>>::encode(const arma::Mat<int>& from);

    template<typename T> bool convert<arma::Mat<T>>::decode(const Node& nd, arma::Mat<T>& to)
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
                std::cerr << "Matrix has no rows in yaml." << std::endl;
                return false;
            }
            auto ncols = nd[0].size();
            if (ncols == 0)
            {
                std::cerr << "Matrix has no columns in yaml." << std::endl;
                return false;
            }
            for (std::size_t i = 1; i < nrows; ++i)
            {
                if (nd[i].size() != ncols)
                {
                    std::cerr << "Ill-formed matrix in yaml." << std::endl;
                    return false;
                }
            }
            to = arma::Mat<T>(nrows, ncols);
            for (arma::uword i = 0; i < nrows; ++i)
            {
                for (arma::uword k = 0; k < nrows; ++k)
                {
                    to(i, k) = nd[i][k].as<T>();
                }
            }
        }
        return true;
    }
    template bool convert<arma::Mat<double>>::decode(const Node& nd, arma::Mat<double>& to);
    template bool convert<arma::Mat<Complex>>::decode(const Node& nd, arma::Mat<Complex>& to);
    template bool convert<arma::Mat<int>>::decode(const Node& nd, arma::Mat<int>& to);
}
