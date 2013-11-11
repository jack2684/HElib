#include <FHE.h>
#include <EncryptedArray.h>
#include <NTL/lzz_pXFactoring.h>

#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <typeinfo>

using namespace std;
//stack<int> theStack;
stack<Ctxt> theStack;
FHEcontext* context;
FHESecKey* secretKey;
FHEPubKey* publicKey;
ZZX G;

void setupHELib();
bool isOp(string token);
void evaluate(char op);
bool is_end(char c);
void hecalc(string input);

void greeting() {
	cout << "Welcome to the homomorphic encryption calculator" << endl;
	cout << "Enter expressions in Reverse Polish Notation" << endl;
}

void hecalc(string input) {
	// Local variables
	string token;
	
	// setup FHE
    setupHELib();
    EncryptedArray ea(*context, G); 

	// Main loop
	int i = 0;
	
	while(!is_end(input[i])) {
	//while(0){
		
		token = input[i++];
		if(token[0] == 'q') {
			break;
		}
		else if(isOp(token)) {
            if (theStack.size() < 2) {
                cout << "not enough numbers on the stack" << endl;
            }
            else {
			    // perform the operation
			    evaluate(token[0]);
            }
		}
		else {
			// should be a number, push it
			// encrypt it first 
            Ctxt& c = *(new Ctxt(*publicKey));
            PlaintextArray p(ea); 
            p.encode(atoi(token.data()));
            ea.encrypt(c, *publicKey, p); 
            
			//theStack.push(atoi( token.data() ));
            theStack.push(c);
		}

	}
	// TODO: decrypt before printing
    PlaintextArray p(ea);
    ea.decrypt(theStack.top(), *secretKey, p);
	cout << "The answer is: " ;
    p.print(cout);
    cout << endl;
}

bool isOp(string token) {
	return (token[0] == '+' || token[0] == '-' || token[0] == '*');
}

void evaluate(char op) {
    Ctxt *op1, *op2;

	switch(op) {
		case '+':
			op1 = new Ctxt(theStack.top()); theStack.pop();
			op2 = new Ctxt(theStack.top()); theStack.pop();
            (*op1) += (*op2);
			theStack.push(*op1);
			break;
		case '-':
			op1 = new Ctxt(theStack.top()); theStack.pop();
			op2 = new Ctxt(theStack.top()); theStack.pop();
            (*op1) -= (*op2);
			theStack.push(*op1);
			break;
		case '*':
			op1 = new Ctxt(theStack.top()); theStack.pop();
			op2 = new Ctxt(theStack.top()); theStack.pop();
            (*op1) *= (*op2);
			theStack.push(*op1);
			break;
	}
}

void setupHELib() {
    long p=101;
    long r=1;
    long L=8;
    long c=2;
    long k=80;
    long s=0;
    long d=0;
    long w=64;
    long m = FindM(k, L, c, p, d, s, 0);
    
    context = new FHEcontext(m,p,r);
    buildModChain(*context, L, c);
    G = context->alMod.getFactorsOverZZ()[0];

    secretKey = new FHESecKey(*context);
    publicKey = secretKey;

   secretKey->GenSecKey(w); 
   addSome1DMatrices(*secretKey); // compute key-switching matrices that we need
}

int read_dataset(string* input)
{
    cout << "reading file\t";

    int n; 
    ifstream infile;
    infile.open("dataset.txt");
    
    infile >> n;
    cout << "n: " << n << "\n";
     
    int i = 0;
    while(i != n)
    {
        infile >> input[i++];
        cout << "input: " << input[i - 1] << "\n";
    }
    
    return n;
}

void print_timers(clock_t* timers, int n)
{
    for(int i = 0; i < n; i++)
        cout << (double)timers[i] / ((double)CLOCKS_PER_SEC) << endl;	  
}

//if char c is a number or * or +, it is good to go
bool is_end(char c)
{
    return ( (int)c == 0x2A || 
            (int)c == 0x2B || 
            ((int)c >= 0x30 && (int)c <= 0x39) ) ?  false : true;
}

int main(int argc, char** argv) {
	  greeting();
	  
	  clock_t init;
	  string* input;
	  input = new string[1000];
	  int n_formula = read_dataset(input);
	  clock_t* timers = new clock_t[n_formula];
	  
	  for(int i = 0; i < n_formula; i++)
	  {
	      cout << i << " time, input: " << input[i] << endl;
        init = clock();
        hecalc(input[i]);
        timers[i] = clock()-init;
    }
    
    print_timers(timers, n_formula);
}



