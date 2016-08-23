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

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace Sgt
{
    template<class Iter> class Range
    {
        public:
            Range(const Iter& begin, const Iter& end) : begin_(begin), end_(end) {}
            Iter begin() const {return begin_;}
            Iter end() const {return end_;}
            auto operator[](int i) {return *(begin_ + i);}
        private:
            Iter begin_;
            Iter end_;
    };

    template<class Iter> Range<Iter> range(Iter b, Iter e)
    {
        return Range<Iter>(b, e);
    }

    template<typename T> class Components
    {
        public:
            class ConstPtr
            {
                public:
                    ConstPtr()
                    {
                        static const std::unique_ptr<T> null;
                        p_ = null;
                    }

                    ConstPtr(std::unique_ptr<T>& p) : p_(p) {}

                    const T& operator*() const {return *(p_.get().get());}

                    const T* operator->() const {return p_.get().get();}

                    operator bool() {return  p_;}
                    bool operator==(const std::nullptr_t& rhs) {return p_ == nullptr;}
                    bool operator!=(const std::nullptr_t& rhs) {return p_ != nullptr;}

                    template<typename U> const U* as() const {return dynamic_cast<const U*>(p_.get().get());}

                protected:
                    std::reference_wrapper<std::unique_ptr<T>> p_;
            };

            class Ptr : public ConstPtr
            {
                public:
                    Ptr(std::unique_ptr<T>& p) : ConstPtr(p) {}

                    T& operator*() {return *(ConstPtr::p_.get());}

                    T* operator->() {return ConstPtr::p_.get().get();}

                    template<typename U> const U* as() const {return dynamic_cast<const U*>(ConstPtr::p_.get().get());}
                    template<typename U> U* as() {return dynamic_cast<U*>(ConstPtr::p_.get().get());}
            };

        public:

            ConstPtr operator[](const std::string& id) const {return map_.at(id);}
            Ptr operator[](const std::string& id) {return map_.at(id);}
            
            ConstPtr operator[](size_t idx) const {return vec_[idx];}
            Ptr operator[](size_t idx) {return vec_[idx];}

            size_t size() const {return vec_.size();}

            auto begin() {return vec_.begin();}
            auto end() {return vec_.end();}

            auto begin() const {return vec_.cbegin();}
            auto end() const {return vec_.cend();}
           
            auto cbegin() const {return vec_.cbegin();}
            auto cend() const {return vec_.cend();}
            
            auto rbegin() {return vec_.rbegin();}
            auto rend() {return vec_.rend();}
            
            auto rbegin() const {return vec_.rcbegin();}
            auto rend() const {return vec_.rcend();}

            auto rcbegin() const {return vec_.rcbegin();}
            auto rcend() const {return vec_.rcend();}
            
            Range<decltype(std::declval<Components>->cbegin())> vec() const {return {cbegin(), cend()};}
            Range<decltype(std::declval<Components>->begin())> vec() {return {begin(), end()};}

        private:
            std::map<std::string, std::unique_ptr<T>> map_;
            std::vector<Ptr> vec_;
    };
    
    template<typename T> class MutableComponents : public Components<T>
    {
        public:
            void insert(std::unique_ptr<T> comp)
            {
                auto it = Components<T>::map_.find(comp->id());
                if (it == Components<T>::map_.end())
                {
                    // Insert new element.
                    it = Components<T>::map_.insert(std::make_pair(comp->id(), std::move(comp))).first;
                    Components<T>::vec_.push_back(it->second);
                }
                else
                {
                    // Replace existing element.
                    it->second = std::move(comp);
                }
            }

            std::unique_ptr<T> remove(const std::string& id) 
            {
                std::unique_ptr<T> result;
                auto it = Components<T>::map_.find(id);
                if (it != Components<T>::map_.end())
                {
                    result = std::move(it->second);
                    Components<T>::map_.erase(it);
                    Components<T>::vec_.erase(
                            std::remove(Components<T>::vec_.begin(), Components<T>::vec_.end(), id), 
                            Components<T>::vec_.end());
                }
                return result;
            }
    };
}

#endif // COMPONENTS_DOT_H
