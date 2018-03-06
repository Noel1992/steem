#include <iostream>

#include <fc/crypto/elliptic.hpp>
#include <steem/utilities/key_conversion.hpp>

int main(int argc, char **argv) {
    auto priv_key = fc::ecc::private_key::regenerate(fc::sha256::hash(std::string(argv[1])));
    std::cout << steem::utilities::key_to_wif(priv_key) << std::endl;
}