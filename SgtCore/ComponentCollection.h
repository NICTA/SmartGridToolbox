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
    template<typename T>
        using ComponentCollectionIter = typename std::map<std::string, std::shared_ptr<T>>::iterator;
    template<typename T>
        using ComponentCollectionConstIter = typename std::map<std::string, std::shared_ptr<T>>::const_iterator;

    template<typename Iter>
        using ComponentPtrType = std::remove_reference_t<decltype(*(std::declval<Iter>()->second))>;

    template<typename B, typename D, typename Iter> class CompPtrA;
    template<typename Iter> using CompPtrB = CompPtrA<ComponentPtrType<Iter>, ComponentPtrType<Iter>, Iter>;

    /// @brief Pointer type to a member of ComponentCollection.
    template<typename Iter> class CompPtrA<ComponentPtrType<Iter>, ComponentPtrType<Iter>, Iter>
    {
        using Type = ComponentPtrType<Iter>;
        friend class CompPtrA<Type, Type, ComponentCollectionIter<Type>>;
        friend class CompPtrA<Type, Type, ComponentCollectionConstIter<Type>>;

        protected:
            typename std::unique_ptr<Iter> it_{nullptr};
            using CIter = ComponentCollectionConstIter<Type>;
            using NCIter = ComponentCollectionIter<Type>;

        public:
            /// @brief Default constructor.
            CompPtrA() = default;

            /// @brief Constructor for existing component.
            CompPtrA(const Iter& it) : it_(std::make_unique<Iter>(it)) {}

            /// @brief Copy constructor.
            CompPtrA(const CompPtrA& from) : it_(std::make_unique<Iter>(*from.it_)) {}

            /// @brief Non-const to const copy conversion constructor. 
            template<typename FromIter, 
                std::enable_if_t<std::is_same<FromIter, NCIter>::value && std::is_same<Iter, CIter>::value, int> = 0>
                CompPtrA(const CompPtrA<Type, Type, FromIter>& from) : it_(std::make_unique<Iter>(*from.it_)) {}

            /// @brief Copy assignment.
            void operator=(const CompPtrA& from) {it_ = std::make_unique<Iter>(*from.it_);}

            /// @brief Non-const to const conversion assignment.
            template<typename FromIter, 
                std::enable_if_t<std::is_same<FromIter, NCIter>::value && std::is_same<Iter, CIter>::value, int> = 0>
                void operator=(const CompPtrA<Type, Type, FromIter>& from)
            {
                it_ = std::make_unique<Iter>(*from.it_);
            }

            /// @brief Explicit shared pointer access.
            std::shared_ptr<Type> shared() const {return it_ == nullptr ? nullptr : (**it_).second;}

            /// @brief Explicit raw pointer access.
            Type* get() const {return shared().get();}

            /// @brief Cast.
            template<typename U> U* as() const {return dynamic_cast<U*>(get());}

            /// @brief Cast to shared.
            template<typename U> std::shared_ptr<U> asShared() const {return std::dynamic_pointer_cast<U>(shared());}

            /// @brief Dereference.
            Type& operator*() const {return *(shared());}

            /// @brief Dereference. 
            Type* operator->() const {return get();}

            /// @brief Converts to raw pointer. 
            operator Type*() const {return get();}

            /// @brief Converts to shared_ptr. 
            operator std::shared_ptr<Type>() const {return shared();}

            /// @brief Obtain the key in my container.
            const std::string& key() const {return (**it_).first;}

            /// @brief Do I refer to an actual component? 
            operator bool() const {return shared();}

            /// @brief Comparison to nullptr.
            bool operator==(const std::nullptr_t& rhs) const {return shared() == nullptr;}

            /// @brief Comparison to nullptr.
            bool operator!=(const std::nullptr_t& rhs) const {return !operator==(rhs);}

            /// @brief Compare two CompPtrA.
            ///
            /// Undefined behaviour if they come from different ComponentCollection containers.
            bool operator==(const CompPtrB<Iter>& rhs) const {return (it_ == rhs.it_) || shared() == rhs.shared();}

            /// @brief Compare two CompPtrA.
            ///
            /// Undefined behaviour if they come from different ComponentCollection containers.
            bool operator!=(const CompPtrB<Iter>& rhs) const {return !operator==(rhs);}
    };

    /// @brief Pointer type to a member of ComponentCollection which implicitly casts to a derived class.
    template<typename B, typename D, typename Iter> class CompPtrA : public CompPtrB<Iter>
    {
        protected:
            using CompPtrB<Iter>::it_;
            using CIter = ComponentCollectionConstIter<B>;
            using NCIter = ComponentCollectionIter<B>;

        public:
            // Note: Copy constructor and assignment operator should be implicitly defined.

            /// @brief Default constructor.
            CompPtrA() = default;

            /// @brief Constructor for existing component.
            CompPtrA(const Iter& it) : CompPtrB<Iter>(it) {}

            /// @brief Copy conversion constructor. 
            template<typename FromIter, typename FromD, 
                std::enable_if_t<std::is_same<FromIter, NCIter>::value && std::is_same<Iter, CIter>::value, int> = 0>
                CompPtrA(const CompPtrA<B, FromD, FromIter>& from) : CompPtrB<Iter>(from) {}

            /// @brief Conversion assignment.
            template<typename FromIter, typename FromD, 
                std::enable_if_t<std::is_same<FromIter, NCIter>::value && std::is_same<Iter, CIter>::value, int> = 0>
                void operator=(const CompPtrA<B, FromD, FromIter>& from)
            {
                CompPtrB<Iter>::operator=(from);
            }

            /// @brief Explicit shared pointer access.
            std::shared_ptr<D> shared() const {return std::dynamic_pointer_cast<D>(CompPtrB<Iter>::shared());}

            /// @brief Explicit raw pointer access.
            D* get() const {return shared().get();}

            /// @brief Dereference.
            D& operator*() const {return *(shared());}

            /// @brief Dereference. 
            D* operator->() const {return get();}

            /// @brief Converts to raw pointer. 
            operator D*() const {return get();}

            /// @brief Converts to shared_ptr. 
            operator std::shared_ptr<D>() const {return shared();}

            /// @brief Do I refer to an actual component? 
            operator bool() const {return shared();}

            /// @brief Comparison to nullptr.
            bool operator==(const std::nullptr_t& rhs) const {return shared() == nullptr;}

            /// @brief Comparison to nullptr.
            bool operator!=(const std::nullptr_t& rhs) const {return !operator==(rhs);}

            /// @brief Compare two CompPtrA.
            ///
            /// Undefined behaviour if they come from different ComponentCollection containers.
            bool operator==(const CompPtrA& rhs) const {return (it_ == rhs.it_) || shared() == rhs.shared();}

            /// @brief Compare two CompPtrA.
            ///
            /// Undefined behaviour if they come from different ComponentCollection containers.
            bool operator!=(const CompPtrA& rhs) const {return !operator==(rhs);}
    };

    template<typename B, typename D = B> using ComponentPtr = CompPtrA<B, D, ComponentCollectionIter<B>>;
    template<typename B, typename D = B> using ConstComponentPtr = CompPtrA<B, D, ComponentCollectionConstIter<B>>;

    /// @brief Utility container type combining aspects of map and vector.
    ///
    /// May be iterated over or indexed by a string key or an integer index.
    /// ComponentPtr and ConstComponentPtr are a kind of smart pointer to elements in the container.
    /// They are effectively equivalent to storing an iterator to a map, and as such, are stable under
    /// changes to the mapped type. Thus we can obtain a ComponentPtr, and later replace the value 
    /// stored at the given key with another value, and the ComponentPtr will reference the newly replaced
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
                return it != map_.end() ? it : ConstPtr();
            }
            Ptr operator[](const std::string& key)
            {
                auto it = map_.find(key);
                return it != map_.end() ? it : Ptr();
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
