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

#ifndef COMPONENT_COLLECTION_DOT_H
#define COMPONENT_COLLECTION_DOT_H

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace Sgt
{
    template<typename T> class ComponentCollection;
    template<typename T> class MutableComponentCollection;

    /// @brief Pointer type to a member of `ComponentCollection`.
    /// @ingroup Components
    template<typename B, typename D = B, bool isConst = false> class ComponentPtr
    {
        // static_assert(std::is_base_of<B, D>(), "ComponentPtr<B, D, isConst>: B must be a base class of D.");
        // NOTE: static_assert (as above) is nice, but prevents using ComponentPtr with forward declarations.  

        template<typename B1, typename D1, bool isConst1> friend class ComponentPtr;

        private:
        using CIter = typename std::map<std::string, std::shared_ptr<B>>::const_iterator;
        using NCIter = typename std::map<std::string, std::shared_ptr<B>>::iterator;

        using Iter = std::conditional_t<isConst, CIter, NCIter>;

        std::unique_ptr<Iter> it_{nullptr};

        public:
        /// @brief Default constructor.
        ComponentPtr() = default;

        /// @brief Constructor with nullptr.
        ComponentPtr(const std::nullptr_t& null) : ComponentPtr() {};

        /// @brief Constructor for existing component, non-const to const conversion.
        template<typename FromIter, std::enable_if_t<std::is_convertible<FromIter, Iter>::value, int> = 0>
        ComponentPtr(const FromIter& it) : it_(std::make_unique<Iter>(it)) {}

        /// @brief Copy constructor.
        ComponentPtr(const ComponentPtr& from) :
            it_(from.it_ == nullptr ? nullptr : std::make_unique<Iter>(*from.it_)) {}

        /// @brief Conversion constructor.
        ///
        /// Implicit up-conversion.
        template<typename FromD, bool fromIsConst, 
            std::enable_if_t<(isConst || !fromIsConst) && std::is_base_of<D, FromD>::value, int> = 0>
        ComponentPtr(const ComponentPtr<B, FromD, fromIsConst>& from) :
            it_(from == nullptr ? nullptr : std::make_unique<Iter>(*from.it_)) {}

        /// @brief Explicit conversion, including down conversion. 
        template<typename ToD, bool toIsConst = isConst,
            std::enable_if_t<(toIsConst || !isConst) && 
                (std::is_base_of<ToD, D>::value || std::is_base_of<D, ToD>::value), int> = 0>
        ComponentPtr<B, ToD, toIsConst> as() const
        {
            if (it_ == nullptr) return nullptr; else return *it_;
            // Ternary causes possible complications.
        }

        /// @brief Copy assignment.
        void operator=(const ComponentPtr& from)
        {
            if (from == nullptr) it_ = nullptr; else it_ = std::make_unique<Iter>(*from.it_);
        }

        /// @brief Conversion assignment.
        template<typename FromD, bool fromIsConst, 
            std::enable_if_t<(isConst || !fromIsConst) && std::is_base_of<D, FromD>(), int> = 0>
        void operator=(const ComponentPtr<B, FromD, fromIsConst>& from)
        {
            if (from == nullptr) it_ = nullptr; else it_ = std::make_unique<Iter>(*from.it_);
        }

        /// @brief Shared pointer access.
        template<typename T = D, std::enable_if_t<std::is_same<T, B>::value, bool> = 0>
        std::shared_ptr<T> shared() const
        {
            if (it_ == nullptr) return  nullptr; else return (**it_).second;
            // Ternary causes possible complications.
        }

        /// @brief Explicit shared pointer access.
        template<typename T = D, std::enable_if_t<!std::is_same<T, B>::value, bool> = 0>
        std::shared_ptr<T> shared() const
        {
            return std::dynamic_pointer_cast<T>(shared<B>());
        }

        /// @brief Raw pointer access.
        template<typename T = D> T* raw() const
        {
            return shared<T>().get();
        }

        /// @brief Dereference.
        D& operator*() const {return *(shared());}

        /// @brief Dereference. 
        D* operator->() const {return raw();}

        /// @brief Converts to raw pointer. 
        operator D*() const {return raw();}

        /// @brief Converts to shared_ptr. 
        operator std::shared_ptr<D>() const {return shared();}

        /// @brief Obtain the key in my container.
        const std::string& key() const {return (**it_).first;}

        /// @brief Do I refer to an actual component? 
        operator bool() const {return shared().operator bool();}

        /// @brief Comparison to nullptr.
        bool operator==(const std::nullptr_t& rhs) const {return shared() == nullptr;}

        /// @brief Comparison to nullptr.
        bool operator!=(const std::nullptr_t& rhs) const {return !operator==(rhs);}

        /// @brief Compare two `ComponentPtrs`.
        ///
        /// Undefined behaviour if they come from different ComponentCollection containers.
        template<typename RhsD, bool rhsIsConst> bool operator==(const ComponentPtr<B, RhsD, rhsIsConst>& rhs) const 
        {
            // First comparison mainly to get nullptrs.
            return (it_ == rhs.it_) || shared() == rhs.shared();
        }

        /// @brief Compare two `ComponentPtr`s.
        ///
        /// Undefined behaviour if they come from different ComponentCollection containers.
        template<typename RhsD, bool rhsIsConst>
        bool operator!=(const ComponentPtr<B, RhsD, rhsIsConst>& rhs) const 
        {
            return !operator=(rhs);
        }
    };

    /// @brief Pointer type to a constant member of `ComponentCollection`.
    /// @ingroup Components
    template<typename B, typename D = B> using ConstComponentPtr = ComponentPtr<B, D, true>;

    /// @brief Utility container type combining aspects of map and vector.
    /// @ingroup Components
    ///
    /// May be iterated over or indexed by a string key or an integer index.
    /// `ComponentPtr` and `ConstComponentPtr` are a kind of smart pointer to elements in the container.
    /// They are effectively equivalent to storing an iterator to a map, and as such, are stable under
    /// changes to the mapped type. Thus we can obtain a `ComponentPtr`, and later replace the value 
    /// stored at the given key with another value, and the `ComponentPtr` will reference the newly replaced
    /// value.
    template<typename T> class ComponentCollection
    {
        public:
        using ConstPtr = ConstComponentPtr<T>;
        using Ptr = ComponentPtr<T>;

        public:
        size_t size() const {return vec_.size();}

        ConstPtr operator[](const std::string& key) const 
        {
            auto it = map_.find(key);
            if (it != map_.end()) return it; else return ConstPtr();
            // Ternary causes possible complications.
        }
        Ptr operator[](const std::string& key)
        {
            auto it = map_.find(key);
            if (it != map_.end()) return it; else return Ptr();
            // Ternary causes possible complications.
        }

        ConstPtr operator[](size_t idx) const {return vec_[idx];}
        Ptr operator[](size_t idx) {return vec_[idx];}

        ConstPtr front() const {return vec_.front();}
        Ptr front() {return vec_.front();}

        ConstPtr back() const {return vec_.back();}
        Ptr back() {return vec_.back();}

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

        protected:
        std::map<std::string, std::shared_ptr<T>> map_;
        std::vector<Ptr> vec_;
    };

    /// @ingroup Components
    template<typename T> class MutableComponentCollection : public ComponentCollection<T>
    {
        public:
        using Ptr = typename ComponentCollection<T>::Ptr;
        using ConstPtr = typename ComponentCollection<T>::ConstPtr;

        Ptr insert(const std::string& key, std::shared_ptr<T> comp)
        {
            auto it = ComponentCollection<T>::map_.find(key);
            if (it == ComponentCollection<T>::map_.end())
            {
                // Insert new element.
                it = ComponentCollection<T>::map_.insert(std::make_pair(key, comp)).first;
                ComponentCollection<T>::vec_.push_back(it);
            }
            else
            {
                // Replace existing element.
                it->second = comp;
            }
            return it;
        }

        Ptr reserve(const std::string& key) {insert(key, std::shared_ptr<T>(nullptr));}

        template<typename U, typename ... Args>
        std::pair<Ptr, std::shared_ptr<U>> emplace(const std::string& key, Args&&... args)
        {
            auto comp = std::make_shared<U>(std::forward<Args>(args)...);
            return {insert(key, comp), comp};
        }

        std::shared_ptr<T> remove(const std::string& key) 
        {
            std::shared_ptr<T> result;
            auto mapIt = ComponentCollection<T>::map_.find(key);
            if (mapIt != ComponentCollection<T>::map_.end())
            {
                result = mapIt->second;
                ComponentCollection<T>::map_.erase(mapIt);
                auto vecIt = std::find_if(ComponentCollection<T>::vec_.begin(), ComponentCollection<T>::vec_.end(), 
                        [&key](Ptr& p){return p.key() == key;});
                assert(vecIt != ComponentCollection<T>::vec_.end());
                ComponentCollection<T>::vec_.erase(vecIt);
            }
            return result;
        }
    };
}

#endif // COMPONENT_COLLECTION_DOT_H
