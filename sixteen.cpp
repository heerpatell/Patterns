//       1 
//     2 3
//   4 5 6
// 7 8 9 10
#include <iostream>
using namespace std;
int main()
{
    int n,count=1;
    cin >> n; // no of rows
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {            
            if(i>=n-j+1){
                cout<<count<<" ";
                count++;
            }else{
                cout<<"  ";
            }
        }
        cout<<endl;
    }
}