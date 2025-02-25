//
//  main.cpp
//  KRY-RSA
//
//  Created by Jordán Jarolím SDE on 20/04/2018.
//  Copyright © 2018 FIT VUTBR. All rights reserved.
//

#include <iostream>
#include <getopt.h>
#include <tuple>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <gmp.h>

#include "solovay.hpp"
#include "rsa.hpp"

#define GENERATE 0
#define CYPHER 1
#define DECYPHER 2
#define BREAK 3

using namespace std;

/**
 * Structure holding initial settings and params
 */
typedef struct {
    int length;
    int type;
    mpz_t publicExponent;
    mpz_t N;
    mpz_t message;
    mpz_t privateD;
    mpz_t cyphered;
} tParams;

/** Helper method for random seed
 * https://stackoverflow.com/questions/322938/recommended-way-to-initialize-srand?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
 */
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

void help(){
    cout << "./kry -b length || ./kry -e E N M || ./kry -d D N C\n";
}

/**
 * Parse args
 */
tParams getOptions(int argc, char *argv[]) {
    tParams params;
    mpz_init(params.publicExponent); mpz_init(params.N); mpz_init(params.message);
    mpz_init(params.privateD); mpz_init(params.cyphered);
    
    if (!strcmp(argv[1], "-g")){
        params.length = atoi(argv[2]);
        params.type = GENERATE;
    }
    else if (!strcmp(argv[1], "-e")){
        mpz_set_str(params.publicExponent, argv[2], 0);
        mpz_set_str(params.N, argv[3], 0);
        mpz_set_str(params.message, argv[4], 0);
        params.type = CYPHER;
    }
    else if (!strcmp(argv[1], "-d")){
        mpz_set_str(params.privateD, argv[2], 0);
        mpz_set_str(params.N, argv[3], 0);
        mpz_set_str(params.cyphered, argv[4], 0);
        params.type = DECYPHER;
    }
    else if (!strcmp(argv[1], "-b")){
        mpz_set_str(params.N, argv[2], 0);
        params.type = BREAK;
    }
    else{
        help();
        exit(0);
    }
    return params;
}


/**
 * handle generating RSA params
 */
void generate(tParams params){
    /* Init rand */
    unsigned long seed = mix(clock(), time(NULL), getpid());
    gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);
    gmp_randseed_ui(rstate, seed);
    
    /* Init SolovayStrassen class to get primes */
    Solovay* solovay = new Solovay(params.length);
    Rsa* rsa = new Rsa();
    tRandoms randoms;
    mpz_t N; mpz_init(N);
    mpz_t phi; mpz_init(phi);
    mpz_t publicExponent; mpz_init(publicExponent);
    mpz_t privateD; mpz_init(privateD);
    
    
    /* Generate primes, get N and check its length */
    char* str = mpz_get_str(NULL, 2, N);
    while((unsigned)strlen(str) != params.length){
        randoms = solovay->getPrimes(rstate);
        rsa->getN(N, randoms.p, randoms.q);
        str = mpz_get_str(NULL, 2, N);
    }
    
    /* Generate othe parameters */
    rsa->getPhi(phi, randoms.p, randoms.q);
    rsa->getPublicExponent(publicExponent, phi, rstate);
    rsa->extendedEuclid(privateD, publicExponent, phi);
    
    gmp_printf("0x%Zx 0x%Zx 0x%Zx 0x%Zx 0x%Zx\n",randoms.p, randoms.q, N, publicExponent, privateD);
    mpz_clear(N);
    mpz_clear(privateD);
    mpz_clear(phi);
    mpz_clear(publicExponent);
}

/**
 * Cypher message
 */
void cypher(tParams params){
    mpz_t cyphered; mpz_init(cyphered);
    Rsa* rsa = new Rsa();
    rsa->cypher(cyphered, params.message, params.publicExponent, params.N);
    gmp_printf("0x%Zx\n",cyphered);
    mpz_clear(cyphered);
    mpz_clear(params.message);
    mpz_clear(params.N);
    mpz_clear(params.publicExponent);
}

/**
 * decypher message
 */
void decypher(tParams params){
    mpz_t message; mpz_init(message);
    Rsa* rsa = new Rsa();
    rsa->decypher(message, params.cyphered, params.privateD, params.N);
    gmp_printf("0x%Zx\n",message);
    mpz_clear(message);
    mpz_clear(params.cyphered);
    mpz_clear(params.N);
    mpz_clear(params.privateD);
}

/**
 * Break RSA
 */
void breakIt (tParams params){
    /* Init rand */
    unsigned long seed = mix(clock(), time(NULL), getpid());
    gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);
    gmp_randseed_ui(rstate, seed);
    mpz_t p; mpz_init(p);
    
    Rsa* rsa = new Rsa();
    rsa->breakIt(p, params.N, rstate);
    gmp_printf("0x%Zx\n",p);
    mpz_clear(p);
    mpz_clear(params.N);
}

/**
 * main body
 */
int main(int argc, char ** argv) {
    tParams params = getOptions(argc, argv);
    switch (params.type) {
        case GENERATE:
            generate(params);
            break;
        case CYPHER:
            cypher(params);
            break;
        case DECYPHER:
            decypher(params);
            break;
        case BREAK:
            breakIt(params);
        default:
            exit(0);
            break;
    }
    return 0;
}
