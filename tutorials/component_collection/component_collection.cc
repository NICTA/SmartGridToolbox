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

#include <SgtCore.h> // Include SgtCore headers.

// Please note the following namespaces:
using namespace Sgt;
using namespace std;
using namespace arma;

struct Foo
{
    Foo(const string& msg1) : msg(msg1) {}
    virtual ~Foo() = default;
    void whoAmI() const {sgtLogMessage() << "Foo: " << msg << endl;} // Note: not a virtual function.
    void nonConstMethod() {sgtLogMessage() << "Boom!" << endl;}
    string msg;
};

struct Bar : public Foo
{
    Bar(const string& msg1) : Foo(msg1) {}
    void whoAmI() const {sgtLogMessage() << "Bar: " << msg << endl;} // Note: not a virtual function.
};

int main(int argc, char** argv)
{
    // See the Foo and Bar structs defined above.
    sgtLogMessage() << "----------------" << endl;
    sgtLogMessage() << "Components and ComponentCollection:" << endl;
    sgtLogMessage() << "----------------" << endl;
    sgtLogMessage() << 
        "In SmartGridToolbox, both networks and discrete event simulations\n"
        "use the Component and ComponentCollection classes." << endl;
    MutableComponentCollection<Foo> cc;
    // To insert elements, we need a MutableComponentCollection.
    cc.insert("f", make_shared<Foo>("foo"));
    // Members of a ComponentCollection are a special ComponentPtr<T> type, passed in using a shared_ptr
    // as shown above.
    cc.insert("b", make_shared<Bar>("bar"));
    // Members of a ComponentCollection are a special ComponentPtr<T> type, passed in using a shared_ptr
    // as shown above.

    // We can retrieve components like this:
    auto p1 = cc["f"];
    auto p2 = cc["b"];
    
    // Both of these are ComponentPtr<Foo>:
    sgtLogMessage() << "Calling whoAmI on p1:" << endl;
    p1->whoAmI();
    sgtLogMessage() << "Calling whoAmI on p2:" << endl;
    p2->whoAmI();
    
    // We can also retrieve elements by index and iterate over them in order of insertion:
    sgtLogMessage() << "Calling whoAmI on cc[0]:" << endl;
    cc[0]->whoAmI();
    sgtLogMessage() << "Calling whoAmI on cc[1]:" << endl;
    cc[1]->whoAmI();
    sgtLogMessage() << "Iterating:" << endl;
    for (const auto x : cc)
    {
        x->whoAmI();
    }

    // We can replace elements, and ComponentPtrs will remain valid and will point to the new element.
    cc.insert("f", make_shared<Foo>("foo replaced"));
    sgtLogMessage() << "Calling whoAmI on p1 after replace:" << endl;
    p1->whoAmI(); // Note: p1 now points to the new component.

    // We can downcast p2 to point to derived type Bar:
    auto p2a = p2.as<Bar>();
    sgtLogMessage() << "Calling whoAmI on p2 after downcast:" << endl;
    p2a->whoAmI();

    // What if we try to downcast p1?
    auto p1a = p1.as<Bar>();
    sgtLogMessage() << "p1 is null after invalid downcast:" << endl;
    sgtLogMessage() << "p1a == null? " << (p1a == nullptr ? "T" : "F") << endl;

    // What if we try to retrieve a component that isn't there?
    auto z = cc["z"];
    sgtLogMessage() << "Attempting to retrieve a component that is not there gives null:" << endl;
    sgtLogMessage() << "z == null? " << (p1a == nullptr ? "T" : "F") << endl;

    // There is a const version of ComponentPtr:
    const ComponentCollection<Foo>& constCc = cc;
    // Get a const reference to cc so we can try some stuff.

    // The following code would be a compile error (uncomment and see!):
    // ComponentPtr<Foo> ccp = constCc["a"];

    ConstComponentPtr<Foo> ccp = constCc["f"];
    sgtLogMessage() << "Const method on ConstComponentPtr:" << endl;
    ccp->whoAmI();
    // The following code would be a compile error (uncomment and see!):
    // ccp->nonConstMethod();
}
