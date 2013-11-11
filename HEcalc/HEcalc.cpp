#include <FHE.h>
#include <EncryptedArray.h>
#include <NTL/lzz_pXFactoring.h>

#include <iostream>
#include <stack>
#include <string>
#include <stdlib.h>
#include <time.h>

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
void hecalc(char* input);

void greeting() {
	cout << "Welcome to the homomorphic encryption calculator" << endl;
	cout << "Enter expressions in Reverse Polish Notation" << endl;
	cout << "Enter anything else other than numbers or + or * to quit" << endl;
}

void hecalc(char* input) {
	// Local variables
	string token;

	// setup FHE
    setupHELib();
    EncryptedArray ea(*context, G); 

	// Main loop
	int i = 0;
	while(!is_end(input[i])) {
		
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

//if char c is a number or * or +, it is good to go
bool is_end(char c)
{
    return ( (int)c == 0x2A || 
            (int)c == 0x2B || 
            ((int)c >= 0x30 && (int)c <= 0x39) ) ?  false : true;
}

int main(int argc, char** argv) {
	  greeting();
	  
	  clock_t init, final;
	  char* input = new char[1000];
	  cin >> input;

    init=clock();
    hecalc(input);
    final=clock()-init;
    cout << "time consumed: " << (double)final / ((double)CLOCKS_PER_SEC) << "s" << endl;	  
}


