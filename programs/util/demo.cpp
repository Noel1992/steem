#include <iostream>

#include <fc/crypto/elliptic.hpp>
#include <steem/utilities/key_conversion.hpp>
#include <steem/protocol/hardfork.hpp>
#include <steem/chain/steem_object_types.hpp>

int main(int argc, char **argv) {
    auto priv_key = fc::ecc::private_key::regenerate(fc::sha256::hash(std::string(argv[1])));
	auto pub_key = priv_key.get_public_key(); 
	auto pub_key_type = steem::protocol::public_key_type(pub_key);
	auto pub_key_str = std::string(pub_key_type);
    std::cout << steem::utilities::key_to_wif(priv_key) << std::endl;
    std::cout << pub_key_str  << std::endl;
}
