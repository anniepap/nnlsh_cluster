#ifndef HASHFUNCTIONS_H
#define HASHFUNCTIONS_H

class Hash {
protected:
	int* r;				 							// to use for ri*hi
	int numOfrs;
	static const int M = (0x100000000 - 5);
	static const int W = 4;
	static const int K_VEC = 3;
public:
	Hash(int numOfrs);	 							// numOfrs = i = k_vec
	virtual ~Hash();
	virtual int HashFunction(double* concatVector) = 0;
};

class LSHFunction : public Hash {
	int** vectors;
	double* ts;
public:
	LSHFunction(int numOfrs);
	~LSHFunction();
	int HashFunction(double* concatVector);
	double marsaglia();
};

#endif
