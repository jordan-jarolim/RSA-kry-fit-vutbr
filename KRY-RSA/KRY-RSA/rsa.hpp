//
//  rsa.hpp
//  KRY-RSA
//
//  Created by Jordán Jarolím on 30.04.18.
//  Copyright © 2018 FIT VUTBR. All rights reserved.
//

#ifndef rsa_hpp
#define rsa_hpp

#include <stdio.h>
#include <gmp.h>


class Rsa{
public:
    void getN(mpz_t N, mpz_t p, mpz_t q);
    void getPhi(mpz_t phi, mpz_t p, mpz_t q);
    void getPublicExponent(mpz_t publicExponent, mpz_t phi, gmp_randstate_t rstate);
    void extendedEuclid(mpz_t result, mpz_t a, mpz_t b);
    void cypher(mpz_t cyphered, mpz_t message, mpz_t publicExponent, mpz_t N);
    void decypher(mpz_t message, mpz_t cyphered, mpz_t privateD, mpz_t N);
    void breakIt(mpz_t p, mpz_t N, gmp_randstate_t rstate);

private:
    void euclid(mpz_t u, mpz_t w);
    void modPow(mpz_t result, mpz_t base, mpz_t exponent, mpz_t mod);


};
#endif /* rsa_hpp */
