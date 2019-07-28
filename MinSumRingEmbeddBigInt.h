//
// Created by dominik on 7/26/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDDBIGINT_H
#define FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDDBIGINT_H

#include <gmp.h>

class MinSumRingEmbeddBigInt {
    mpz_t big_int;

public:
    static int max_underlying_value;
    static mpz_t* powers;
    static void init(unsigned int n,int max_value){
        mpz_t base;
        mpz_init(base);
        unsigned int base_ui=1<<n+1; //2^n +1
        mpz_set_ui(base,base_ui);
        max_underlying_value=max_value;
        powers= new mpz_t[max_underlying_value*2+1];
        mpz_init(powers[0]);
        mpz_set_ui(powers[0],1);
        for(unsigned int i=1;i<=2*max_underlying_value;++i){
            mpz_init(powers[i]);
            mpz_mul(powers[i],powers[i-1],base);
        }
    }

    static void clear(int max_value){
        for(unsigned int i=1;i<=2*max_underlying_value;++i){
            mpz_clear(powers[i]);
        }
    }


    MinSumRingEmbeddBigInt(){
        mpz_init(big_int);
    }


    MinSumRingEmbeddBigInt &operator=(const int &x) //use only for x=0 to set to zero element
    {
        mpz_set_ui(big_int,0);
        return *this;
    }

    explicit MinSumRingEmbeddBigInt(unsigned int value){
        mpz_init(big_int);
        mpz_set(big_int,powers[value]);
    }

    MinSumRingEmbeddBigInt operator*(const MinSumRingEmbeddBigInt &rhs) {
        MinSumRingEmbeddBigInt result;
        mpz_mul(result.big_int,big_int,rhs.big_int);
        return result;
    }

    MinSumRingEmbeddBigInt operator+(const MinSumRingEmbeddBigInt &rhs) {
        MinSumRingEmbeddBigInt result;
        mpz_add(result.big_int,big_int,rhs.big_int);
        return result;
    }


    MinSumRingEmbeddBigInt operator-( const MinSumRingEmbeddBigInt &rhs){
        MinSumRingEmbeddBigInt result;
        mpz_sub(result.big_int,big_int,rhs.big_int);
        return result;
    }

    virtual ~MinSumRingEmbeddBigInt(){
        //mpz_clear(big_int);
    }


    int min(){
        int min_value=0;
        mpz_t remainder;
        mpz_init(remainder);
        mpz_set(remainder,big_int);
        for(int i=max_underlying_value*2;i>=0;--i){
            if(mpz_cmp(remainder,powers[i])>=0) { //mpz_cmp(x,y) returns >0 when x>y,0 when equal, <0 otherwiese
                min_value=i;
                mpz_mod(remainder,remainder,powers[i]);
            }
        }
        mpz_clear(remainder);
        return min_value;
    }


    int max(){

        return 0;
    }
};


#endif //FAST_SUBSET_CONVOLUTION_MINSUMRINGEMBEDDBIGINT_H
