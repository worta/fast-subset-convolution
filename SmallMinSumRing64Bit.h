//
// Created by dominik on 9/10/19.
//

#ifndef FAST_SUBSET_CONVOLUTION_SMALLMINSUMRING64BIT_H
#define FAST_SUBSET_CONVOLUTION_SMALLMINSUMRING64BIT_H


class SmallMinSumRing64Bit {
    int big_value; //todo change to at least 64 bit
public:
    static int max_underlying_value; //should be very low, probably 2-3
    static int* powers;
    static void init(unsigned int n,int max_value){
        unsigned int base_ui=(1<<n)+1; //2^n +1
        max_underlying_value=max_value;
        powers= new int[max_underlying_value*2+1];
        powers[0]=1;
        for(unsigned int i=1;i<=2*max_underlying_value;++i){
            powers[i]=powers[i-1]*base_ui;
        }
    }


    explicit SmallMinSumRing64Bit(unsigned int x) {
        big_value=powers[x];
    }

    SmallMinSumRing64Bit() {
    }

    SmallMinSumRing64Bit &operator=(const int &x) //use only for x=0 to set to zero element
    {
        big_value=0;
        return *this;
    }


    SmallMinSumRing64Bit operator*(const SmallMinSumRing64Bit &rhs) {
        SmallMinSumRing64Bit result;
        result.big_value=big_value*rhs.big_value;
        return result;
    }

    SmallMinSumRing64Bit operator+(const SmallMinSumRing64Bit &rhs) {
        SmallMinSumRing64Bit result;
        result.big_value=big_value+rhs.big_value;
        return result;
    }


    SmallMinSumRing64Bit operator-( const SmallMinSumRing64Bit &rhs){
        SmallMinSumRing64Bit result;
        result.big_value=big_value-rhs.big_value;
        return result;
    }

    int min(){
        int min_value=0;
        int remainder=big_value;
        for(int i=max_underlying_value*2;i>=0;--i){
            if(remainder>=powers[i]) { //mpz_cmp(x,y) returns >0 when x>y,0 when equal, <0 otherwiese
                min_value=i;
                remainder=remainder % powers[i];
            }
        }
        return min_value;
    }
};


#endif //FAST_SUBSET_CONVOLUTION_SMALLMINSUMRING64BIT_H
