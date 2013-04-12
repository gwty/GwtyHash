/**
Author:Gowtham Ashok
E-mail: gwty93@gmail.com
Program:Plagiarism Detector
*/



#include<fstream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<cctype>
#include<cmath>
#include<cctype>
#include<complex>
#include<algorithm>
#include<map>
#include<set>
#include<exception>
#include<memory>
#include<numeric>
#include<string>
#include<vector>
#include<stack>
#include<queue>
#include<list>
#include<iterator>
#include <stdint.h>
#include<sstream>
#include <time.h>
#include <sys/param.h>
#include "lookup3.c"

#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

#define HAMM_MAX 2
using namespace std;


//typedef unsigned long long  uint64_t;
//typedef unsigned long int uint32_t;


/* Filters unimportant characters.
 * Now important characters are alphanumeric and those with the most
 * significant bit set (utf-8 friendly).
 */
static bool
meaningful_char( char c)
{
    return isalnum( c) || (c&0x80);
}

/* Returns next token.
 * string	current data pointer
 * string_end	pointer one character behind the end of data
 * token_start	output hash value of the token
 * token_length
 */
static bool
get_next_token( const char **string, const char *string_end, const char **token_start, size_t *token_length)
{
    const char *start = *string;

    /* Skip white space */
    while (start != string_end && !meaningful_char( *start))
        start++;

    /* At the end of the string? */
    if (start == string_end)
    {
        *string = start;
        //dprintf( " token: end\n");
        return false;
    }

    /* Read the string */
    const char *s = start+1;
    while (s != string_end && meaningful_char( *s))
    {
        s++;
    }

    *string = s;
    *token_start = start;
    *token_length = s-start;
    return true;
}


/* Hashes the given token.
 */
static uint64_t
get_hashed_token( const char *token_start, size_t token_length)
{
    uint32_t h1 = 0xac867c1d; /* Dummy init values */
    uint32_t h2 = 0x5434e4c4;
    hashlittle2( token_start, token_length, &h1, &h2);
    return ((uint64_t)h1 << 32) +h2;
}


/* Updates histogram with weight==1.
 * Tricky implementation, optimized for speed.
 */

static inline void
update_hist( int *hist, uint64_t token)
{
    uint32_t t = token;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t = token>>32;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist++ += t & 1;
    t >>= 1;
    *hist   += t & 1;
}


/* Calculates the hamming distance.
 */
int
hamming_dist( uint64_t a1, uint64_t a2)
{
    uint32_t v1 = a1^a2;
    uint32_t v2 = (a1^a2)>>32;

    v1 = v1 - ((v1>>1) & 0x55555555);
    v2 = v2 - ((v2>>1) & 0x55555555);
    v1 = (v1 & 0x33333333) + ((v1>>2) & 0x33333333);
    v2 = (v2 & 0x33333333) + ((v2>>2) & 0x33333333);
    int c1 = ((v1 + (v1>>4) & 0xF0F0F0F) * 0x1010101) >> 24;
    int c2 = ((v2 + (v2>>4) & 0xF0F0F0F) * 0x1010101) >> 24;

    return c1+c2;
}


/* Calculates the similarity hash.
 */
uint64_t
charikar_hash64( const char *data, long data_length)
{
    const char *data_end = data +data_length;

    /* Clear histogram */
    int hist[ 64];
    memset( hist, 0, sizeof( hist));

    /* Token position and length */
    const char *token_start;
    size_t token_length;

    /* Over all tokens... */
    int tokens = 0;
    while ((get_next_token( &data, data_end, &token_start, &token_length)))
    {
        /* Calculate token hash */
        uint64_t token = get_hashed_token( token_start, token_length);
        //dprintf( " token: %016llx %s\n", (long long)token, string( token_start, token_length));

        /* Update histogram, weigth==1 */
        update_hist( hist, token);
        tokens++;
    }
    tokens /= 2;

    /* Calculate a bit vector from the histogram */
    uint64_t simhash=0;
    for (int c=0; c<64; c++)
        simhash |= (uint64_t)((hist[ c]-tokens)>=0) << c;

    return simhash;
}






/* Word stemming: Groups words with the same conceptual meaning together.
 * Usually this is implemented by removing the word suffix.
 * Depends heavily on a language, therefore this function is a stub.
 * To change the word, simply modify the passed parameters.
 */
static void
word_stemming( const char **token_start, size_t *token_length)
{
    /* Nothing */
}


/* Stopword test.
 * The stopword is a frequent word with no useful information.
 * Stopwords differs across languages: This function is a stub.
 * Token is passed in a form of 64-bit hash.
 */
static bool
is_stopword( uint64_t token)
{
    return false;
}


/** Token weight.
 */
static int
token_weight( uint64_t token)
{
    return 1;
}


/* Calculates the similarity hash with super-tokens.
 */
uint64_t
charikar_hash64_wide( const char *data, long data_length, int stoken_size)
{
    const char *data_end = data +data_length;

    /* Clear histogram */
    int hist[ 64];
    memset( hist, 0, sizeof( hist));

    /* Token position and length */
    const char *token_start;
    size_t token_length;

    /* Token buffer to create super-tokens */
    uint32_t token_buf[ 2*stoken_size];
    memset( token_buf, 0, sizeof( token_buf));

    /* Over all tokens... */
    while ((get_next_token( &data, data_end, &token_start, &token_length)))
    {
        /* Word stemming */
        word_stemming( &token_start, &token_length);

        /* Calculate token hash */
        uint64_t token = get_hashed_token( token_start, token_length);
        //dprintf( " token: %016llx %s\n", (long long)token, string( token_start, token_length));

        /* Check for stopwords */
        if (is_stopword( token))
        {
            //	dprintf( "stopword\n");
            continue;
        }

        /* Get token weight */
        int weight = token_weight( token);

        /* Rotate the buffer of tokens */
        if (stoken_size != 1)
            for (int c=0; c<stoken_size-1; c++)
            {
                token_buf[ c*2  ] = token_buf[ c*2+2];
                token_buf[ c*2+1] = token_buf[ c*2+3];
            }

        /* Write the new token at the end of the buffer */
        token_buf[ (stoken_size-1)*2  ] = token>>32;
        token_buf[ (stoken_size-1)*2+1] = token&0xffffffff;

        /* Calculate a hash of the super-token */
        uint32_t h1=0x2c759c01; /* Dummy init values */
        uint32_t h2=0xfef136d7;
        hashword2( token_buf, stoken_size*2, &h1, &h2);
        /* Concatenate results to create a super-token */
        uint64_t stoken = ((uint64_t)h1 << 32) +h2;

        //dprintf( "stoken: %016llx\n", stoken);

        /* Update histogram */
        for (int c=0; c<64; c++)
            hist[ c] += (stoken & ((uint64_t)1 << c)) == 0 ? -weight : weight;
    }

    /* Calculate a bit vector from the histogram */
    uint64_t simhash=0;
    for (int c=0; c<64; c++)
        simhash |= (uint64_t)(hist[ c]>=0) << c;

    return simhash;
}


/* Reads the file specified.
 */
void
get_file( char *file_name, char **data,long long *file_length)
{
    /* Open input file */
    FILE *input = fopen( file_name, "r");
    if (!input)
        cout<<"Cannot open file";

    /* Get file length */

    fseek( input, 0, SEEK_END);
    *file_length=ftell(input);
    fseek( input, 0, SEEK_SET);


    /* Get the buffer */
    *data = (char *)malloc(*file_length);
    if (!*data)
        cout<<"Out of memory"<<endl;

    /* Read the file */
    size_t readed = fread( *data, 1, *file_length, input);
    if (readed ==0)
        /* EOF cannot happen */
        cout<<"Error reading file "<<file_name<<endl;

    /* Close the file */
    if (fclose( input) == EOF)
        cout<<"Cant close file "<<file_name<<endl;
}

char db1[]="DBmain.txt";
char db2[]="DBsub.txt";

/*
Writes the Hashes to file
Does not check for duplicates(TO DO)
*/
void writetofile(uint64_t  hash1,int ch, char *filename, long line)
{
    long long hash=(long long) hash1;
    FILE *outmain,*outsub;
    if(ch==1)
    {
        outmain=fopen(db1,"a+");
        fprintf(outmain,"%016llx\t%s\n",hash,filename);
        fclose(outmain);
    }
    else if(ch==2)
    {
        outsub=fopen(db2,"a+");
        fprintf(outsub,"%016llx\t%ld\t%s\n",hash,line,filename);
        fclose(outsub);
    }

}

/*
Declaration of the global vectors to be used
*/

vector<uint64_t>hashtofile2;
vector<long>linecount;
vector<uint64_t>::iterator it;
vector<long>::iterator it2;
long totallinesprocessed=0;


/* Returns a sentence */

void ret_blockoftext(char *filename,int stoken_size)
{
    //English only construct. Assuming a sentence ends with "."
    /* Open input file */
    ifstream input;
    input.open(filename,ifstream::in);
    if (!input)
        cout<<"Cannot open file";

    /* Get file length */
    long long file_length;
    input.seekg(0,input.end);
    file_length=input.tellg();
    input.seekg(0,input.beg);

    char c;
    uint64_t hash1;
    uint_fast32_t pos1=0,pos2=0,i=0;
    char what[10000];
    string countline;
    long countmeall=0;
    while(!input.eof())
    {
        long countme;
        input.getline(what,10000,'.');
        countline=string(what);
        countme= std::count(countline.begin(), countline.end(), '\n');
        countmeall+=countme;
        if(input.gcount()>10)
        {

            hash1 = stoken_size == 1
                    ? charikar_hash64( what, input.gcount())
                    : charikar_hash64_wide(what, input.gcount(),stoken_size);
            // For Debugging: printf( "\n%016llx \n", (long long)hash1);
            hashtofile2.push_back(hash1);
            linecount.push_back(countmeall);
            totallinesprocessed++;
        }

    }


}
/*
Declaration of the global vectors to be used
*/

vector<uint64_t>mainstuff;
vector<string>filenameofmain;
vector<string>filenameofsub;
vector<uint64_t>::iterator mains;
vector<uint64_t>readhashtofile2;
vector<long>readlinecount;
vector<uint64_t>::iterator readit;
vector<long>::iterator readit2;
vector<string>::iterator filenmread;
vector<string>::iterator filesubread;

void readmetomem(int ch)
{
    mainstuff.resize(0);
    filenameofmain.resize(0);
    char readin[100];
    uint64_t temp;
    long templong;
    string filename;
    ifstream in;
    in.open(db1,ios::in);
    if(!in)
    {
        cout<<"Cant open DB"<<endl;
        return;
    }
    while(!in.eof())
    {
        in.getline(readin,100,'\t');
        if(!in.good())
            break;
        temp= strtoull(readin, NULL, 16);
        mainstuff.push_back(temp);
        in.getline(readin,100,'\n');
        if(!in.good())
            break;
        filenameofmain.push_back(readin);
    }
    in.close();
    readhashtofile2.resize(0);
    readlinecount.resize(0);
    filenameofsub.resize(0);
    if(ch==1)
    {
        ifstream in2;
        in2.open(db2,ios::in);
        if(!in2)
        {
            cout<<"Cant open DB"<<endl;
            return;
        }
        while(!in2.eof())
        {
            if(!in2.good())
                break;
            in2.getline(readin,100,'\t');
            temp= strtoull(readin, NULL, 16);
            readhashtofile2.push_back(temp);
            in2.getline(readin,100,'\t');
            if(!in2.good())
                break;
            templong= strtoull(readin, NULL, 10);
            readlinecount.push_back(templong);
            in2.getline(readin,100,'\n');
            if(!in2.good())
                break;
            filenameofsub.push_back(readin);
        }
        in2.close();
    }
}

/*
Used for calculating the Hamming Distance
Input: 2 Hashes
Output: Integer containing the Hamming Distance
*/

int calchamm(uint64_t hash1,uint64_t hash2)
{
    uint64_t hashx = hash1^hash2;
    int hamm = ::hamming_dist( hash1, hash2);
    return hamm;
}


/*
Checks whether 2 sentences are similar by comparing their hashes
If same, they are printed as such
If the hashes vary only slightly, limited by Hamming Distance,
they are printed as Similar Documents
*/

void compareme(int argc,char *argv[])
{

    int optind=1;
    for (int c=optind; c<argc; c++)
    {
        ::totallinesprocessed=0;
        uint64_t hash1, hash2;
        long similarlines=0;
        int optind=1;
        int quiet=0;
        int diff=1;
        int stoken_size=1;
        int ch=0;
        int hamm;
        char *data;
        string d;
        long long data_length;
        get_file( argv[c], &data, &data_length);
        d=argv[1];
        hash2 = hash1;
        hash1 = stoken_size == 1
                ? charikar_hash64( data, data_length)
                : charikar_hash64_wide( data, data_length, stoken_size);
        int flag=0;
        free( data);
        readmetomem(0);
        int exact=0;
        cout<<endl<<endl;
        cout<<"For "<<argv[c]<<endl;

        for(readit=mainstuff.begin(),filenmread=filenameofmain.begin();
         readit!=mainstuff.end(),filenmread!=filenameofmain.end();
         readit++,filenmread++)
        {
            hamm= calchamm(hash1,*readit);

            if(hash1==*readit || hamm==0)
            {
                cout<<"Exact/Very close match of  "<<*filenmread<<endl;
                exact=1;
                flag=1;
                continue;
            }
            if(hamm<=HAMM_MAX)
            {
                cout<<"Similar Document of "<<*filenmread<<endl;
                flag=1;
            }

        }


        if(d.find("txt")!=string::npos)
        {
            ret_blockoftext(argv[c],stoken_size);
            readmetomem(1);
            ch=1;
            if(exact==1)
            {
                ch=0;
                similarlines=totallinesprocessed;
            }
        }
        int similarline=0;
        if(ch==1 && exact==0)
        {
            for(readit=readhashtofile2.begin(),
                readit2=readlinecount.begin(),
                filesubread=filenameofsub.begin();

            readit!=readhashtofile2.end(),readit2!=readlinecount.end()
            ,filesubread!=filenameofsub.end();

            readit++,readit2++,filesubread++)
            {
                similarline=0;
                for(it=hashtofile2.begin(),it2=linecount.begin(); it!=hashtofile2.end(),it2!=linecount.end(); it++,it2++)
                {
                    hamm=calchamm(*readit,*it);
                    if(hamm<=HAMM_MAX && hamm>=0)
                    {
                        cout<<"Sentence in line "<<*it2<<" maybe copied from "<<*readit2<<" line of "<<*filesubread<<endl;
                        flag=1;
                        similarline++;
                        if(similarline==1)
                            similarlines++;
                    }
                }
            }
        }

        cout<<endl;
        double percent;
        if(::totallinesprocessed!=0)
        {
            percent=((double)(similarlines)/(double)(::totallinesprocessed))*100;
            cout<<percent<<"% of line(s) may be plagiarised"<<endl;
        }
        if(flag==0)
        {
            cout<<"Not found in database"<<endl;
        }
    }
}


/*
Read the database and compare with the hashes to find similar hashes
one by one
*/

void dbread(int argc, char *argv[])
{
    uint64_t hash1, hash2;
    int optind=1;
    int quiet=0;
    int diff=1;
    int stoken_size=1;


    for (int c=optind; c<argc; c++)
    {
        char *data;
        string d;
        long long data_length;
        get_file( argv[ c], &data, &data_length);
        hash2 = hash1;
        hash1 = stoken_size == 1
                ? charikar_hash64( data, data_length)
                : charikar_hash64_wide( data, data_length, stoken_size);
        d=argv[c];

        if(d.find("txt")!=string::npos)
            ret_blockoftext(argv[c],stoken_size);


        free( data);

        if (!quiet)
        {
            writetofile(hash1,1,argv[c],0);
            printf( "%016llx\t%s\n", (long long)hash1, argv[ c]);

            if(d.find("txt")!=string::npos)
            {
                for(it=hashtofile2.begin(),it2=linecount.begin(); it!=hashtofile2.end(),it2!=linecount.end(); it++,it2++)
                    writetofile((*it),2,argv[c],*it2);

                hashtofile2.resize(0);
                linecount.resize(0);
            }
        }

    }
}


int main(int argc,char *argv[])
{
    int a;
    cout<<"********************************************"<<endl;
    cout<<"                  MENU                               "<<endl;
    cout<<"********************************************"<<endl;
    cout<<"* 1.  Entering into database                            "<<endl;
    cout<<"* 2.  Check if the file is plagiarised or not     " <<endl;
    cout<<"********************************************"<<endl;
    cout<<"Enter your choice:"<<endl;
    cin>>a;
    if(a==1)
        dbread(argc,argv);
    else
        compareme(argc,argv);

    cout<<"Operation over"<<endl;
    return 0;
}

