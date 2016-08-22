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

#ifndef COMPONENTS_DOT_H
#define COMPONENTS_DOT_H

#include <map>
#include <string>
#include <vector>

namespace Sgt
{
    template<typename T> class ComponentPtr
    {
        public:
            ComponentPtr()
            {
                static const std::unique_ptr<T> null;
                p_ = null;
            }

            ComponentPtr(std::unique_ptr<T>& p) : p_(p) {}

            const T& operator*() const {return *p_;}
            T& operator*() {return *p_;}

            const T* operator->() const {return p_.get();}
            T* operator->() {return p_.get().get();}

            operator bool() {return  p_;}
            bool operator==(const std::nullptr_t& rhs) {return p_ == nullptr;}
            bool operator!=(const std::nullptr_t& rhs) {return p_ != nullptr;}

            template<typename U> const U* as() const {return dynamic_cast<const U*>(p_.get());}
            template<typename U> U* as() const {return dynamic_cast<U*>(p_.get());}

        private:
            std::reference_wrapper<std::unique_ptr<T>> p_;
    };

    template<typename T> class Components
    {
        public:
            void insert(std::unique_ptr<T>&& comp)
            {
                auto pair = map_.insert(std::make_pair(comp->id(), std::move(comp)));
                if (pair.second)
                {
                    // New element was inserted.
                    vec_.push_back(pair.first->second);
                }
                else
                {
                    // Previous element existed, replace it.
                    // An element will already exist in vec_ which need not be updated.
                    pair.first->second = std::move(comp);
                }
            }

            void remove(const std::string& id) 
            {
                map_.erase(id);
                vec_.erase(std::remove(vec_.begin(), vec_.end(), id), vec_.end());
            }

            auto begin()
            {
                return vec_.begin();
            }
            
            auto end()
            {
                return vec_.end();
            }

            auto cbegin()
            {
                return vec_.cbegin();
            }
            
            auto cend()
            {
                return vec_.cend();
            }

            auto rbegin()
            {
                return vec_.rbegin();
            }
            
            auto rend()
            {
                return vec_.rend();
            }

            ComponentPtr<const T> get(const std::string& id) const
            {
                return map_.at(id);
            }
            
            ComponentPtr<T> get(const std::string& id)
            {
                return map_.at(id);  
            }

        private:
            std::map<std::string, std::unique_ptr<T>> map_;
            std::vector<ComponentPtr<T>> vec_;
    };
}

#endif // COMPONENTS_DOT_H
