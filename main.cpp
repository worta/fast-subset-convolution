#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>

#include "functions.h"
#include "mobius.h"
#include "steiner.h"

using namespace std;
using namespace std::chrono;

void output_set(uint32_t setRep,int n)
{
    cout<<"< ";
    for(uint32_t i=0; i<=n; i++)
    {
        if(setRep&(1<<(i-1)))
        {
            cout<<i<<" ";
        }
    }
    cout<<">";
}



class int_max
{
public:
    int value;
    int_max ()
    {
        value=0;
    }
    int_max (int x)
    {
        value=x;
    }
    int_max& operator= (const int& x)
    {
        value=x;
        return *this;
    }
    int_max& operator= (const int_max& x)
    {
        value=x.value;
        return *this;
    }
    int_max operator*(const int_max& rhs)
    {
        return int_max(value + rhs.value);
    }
    int_max operator+(const int_max& rhs)
    {
        return int_max(max(value,rhs.value));
    }
    int_max operator-(const int_max& rhs)
    {
        return int_max(min(value,rhs.value));
    }

} ;



/*inline int bitCount(int i) { //counts bit set to 1 for 32 bit number
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    i = (i + (i >> 4)) & 0x0f0f0f0f;
    i = i + (i >> 8);
    i = i + (i >> 16);
    return i & 0x3f;
}*/


void test_subset_gen()
{
    cout <<"Test subset generation"<<endl;
    uint32_t test_set=7; //<1,2>
    cout <<"Testing subset:";
    output_set(test_set,3);
    cout<<endl;
    vector<uint32_t> test_vec {0,1,2,3,4,5,6,7};
    vector<uint32_t > results;
    //results.reserve(2);
    //getSubsets(test_set,results);
    results=getSubsetsIt(test_set);
    if(results.size()==test_vec.size())
    {
        cout<<"Size as expected"<<endl;
    }
    else{
        cout<<"Error: Wrong subset size"<<endl;
    }
    for(uint32_t i=0; i<results.size(); ++i)
    {
        output_set(results[i],3);
        test_vec.erase(std::remove(test_vec.begin(), test_vec.end(), results[i]), test_vec.end()) ;
    }
    if(test_vec.empty())
    {
        cout<<"Subsets as expected"<<endl;
    }
    else{
        cout<<"Error: Wrong elements in subset generation"<<endl;
    }
}

void test_ranked_mobius_and_convolute()
{
    int n=2; //test set={1,2}
    ConstFunction<int> f=ConstFunction<int>(1);

    //Test mobius transformation
    //transformed f should be:
    vector<int> f_0 {1,1,1,1};
    vector<int> f_1 {0,1,1,2};
    vector<int> f_2 {0,0,0,1};
    vector<vector<int> > results {f_0,f_1,f_2};

    vector<vector<int> > rankedFVec(n+1);
    for(int rang=0; rang<=n; rang++)
    {
        rankedFVec[rang]=rankedMobius<int>(f,n,rang);
        if( rankedFVec[rang]==results[rang])
        {
            cout<<"Ranked mobius transform:Values for rank:" <<rang <<" OK"<<endl;
        }
    }

    //test ranked mobius convolution
    //(transformed f)*(transformed f) should be:
    f_0= {1,1,1,1};
    f_1= {0,2,2,4};
    f_2= {0,1,1,6};
    results= {f_0,f_1,f_2};
    RankedVectFunction<int> ranked_f=RankedVectFunction<int>(rankedFVec);
    vector< vector<int> > conv=ranked_convolute(ranked_f,ranked_f,n);
    for(int rang=0; rang<=n; rang++)
    {
        /*for(uint32_t i=0;i<conv[rang].size();++i){
            cout<<conv[rang][i];
        }*/
        if( conv[rang]==results[rang])
        {
            cout<<"Ranked mobius convolution:Values for rank:" <<rang <<" OK"<<endl;
        }
        else
        {
            cout<<"Error: ranked mobius convolution:Values for rank:" <<rang <<" are incorrect"<<endl;
        }
    }

    //test naive convolute:
    vector<int> naive=naive_convolute(f,f,n);
    vector<int> convolute_result {1,2,2,4};
    if (naive==convolute_result)
    {
        cout<<"Naive convolute: OK"<<endl;
    }
    else
    {
        cout<<"Error: Naive convolute: Incorrect values"<<endl;
    }

    vector<int> normal_invert=ranked_Mobius_inversion(ranked_f,n); //should be the same values as f
    if(normal_invert==vector<int> {1,1,1,1})
    {
        cout <<"Normal Mobius inversion: OK"<<endl;
    }
    else
    {
        cout<<"Error: normal invert is wrong"<<endl;
    }

    RankedVectFunction<int> ranked_conv=RankedVectFunction<int>(conv);
    vector<int> inverse=ranked_Mobius_inversion<int>(ranked_conv,n); //should be the same as f*f
    //cout<<inverse.size()<<endl;
    if (inverse==convolute_result)
    {
        cout<<"Mobius inversion of ranked convolution: OK"<<endl;
    }
    else
    {
        cout<<"Error: Mobius inversion: Incorrect values"<<endl;
        cout<<"Should be: {1,2,2,4} but is {"<<inverse[0];
        for(uint32_t i=1; i<inverse.size(); ++i)
        {
            cout<<","<<inverse[i];
        }
        cout<<"}"<<endl;
    }
}

int main()
{
    //ConstFunction<int> f=ConstFunction<int>(1);
    int_max a=int_max(1);
    ConstFunction<int_max> f=ConstFunction<int_max>(a);
    test_subset_gen();
    test_ranked_mobius_and_convolute();

    test_dijkstra();
    ofstream myfile;

    /*myfile.open ("results.txt");
    myfile<<"n naive mobius\n";
    for(int i=6; i<21; ++i)
    {
        int n=i;
        //naive
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        vector<int_max> result_naive=naive_convolute(f,f,n);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>( t2 - t1 ).count();
        myfile<<n<<" "<<duration;
        cout<<"naive approach with n:"<<n<<" done in "<<duration<<"ms"<<endl;
        //mobius

        t1 = high_resolution_clock::now();
        advanced_convolute<int_max>(f,n);
        t2 = high_resolution_clock::now();
        duration = duration_cast<milliseconds>( t2 - t1 ).count();
        myfile<<" "<<duration<<"\n";
        cout<<"mobius approach with n:"<<n<<" done in "<<duration<<"ms"<<endl;
    }

    myfile.close();*/
    return 0;
}
