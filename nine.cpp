#include <iostream>
using namespace std;

int main(){
	int r,c;
    cout<<"\nEnter Rows: ";
    cin>>r;
	for(int i=r;i>=1;i--){
		for(int j=1;j<=i;j++){
			cout<<j<<" ";
		}
		cout<<"\n";
	}   
}
