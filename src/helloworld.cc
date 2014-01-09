#include <iostream>
#include <vector>

int main()
{
   using namespace std;

   cout << "Hello, world." << endl;

   vector<int *> vi = {new int(1), new int(2), new int(3)};
   const vector<int*> & vir = vi;
   *vir[2] = 4;
   cout << *vi[2] << endl;
}
