// FizzBuzz                                             
#include<iostream>                                                      
using namespace std;                                    
                                                        
int main() {                                            
    int i;                                              
    for (i = 1; i < 101; i++) {                          
        cout << (!(i % 3) ? "Fizz" : "");               
        cout << (!(i % 5) ? "Buzz" : "");               
        (i % 3 && i % 5) ? cout << i : cout << ""; 
                                                      
        cout << "\n";                                   
    }                                                   
    return 0;                                           
}                                                       

