#include <iostream>
#include <string>

#include <fc/io/json.hpp>

//#include <fc/crypto/elliptic.hpp>
//#include <steem/utilities/key_conversion.hpp>
//#include <steem/protocol/hardfork.hpp>
//#include <steem/chain/steem_object_types.hpp>

#include <steem/protocol/types.hpp>
#include <steem/protocol/transaction.hpp>
#include <steem/utilities/key_conversion.hpp>

#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/variant.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/reflect/reflect.hpp>


using namespace std;
using namespace steem::protocol;
using namespace fc::raw;

class debug_encoder : public fc::sha256::encoder {
	public:
		void write( const char* d, uint32_t dlen ) {
			for(int i = 0; i < (int)dlen; i++){
				auto v = (int)(*(d+i)) & 0xff;
				cout << v << " ";
			}
			cout << " " << dlen << endl;
			fc::sha256::encoder::write(d, dlen);
		}
};

int main(int argc, char **argv) {
    //auto priv_key = fc::ecc::private_key::regenerate(fc::sha256::hash(std::string(argv[1])));
	auto wif = "5JzpcbsNCu6Hpad1TYmudH4rj1A22SW9Zhb1ofBGHRZSp5poqAX";
	auto priv_key = steem::utilities::wif_to_key(wif);
	auto pub_key = priv_key->get_public_key(); 
	//auto pub_key_type = public_key_type(pub_key);
	auto pub_key_str = pub_key.to_base58();
    //std::cout << steem::utilities::key_to_wif(priv_key) << std::endl;
    std::cout << pub_key_str  << std::endl;
	auto tx = fc::json::from_file("/data0/steem/programs/util/new_account.json.2").as<signed_transaction>();
	//asset fee = asset(9, STEEM_SYMBOL);
	//debug_encoder enc;
	//account_create_operation op = tx.operations[0].get<account_create_operation>();
	// steem::protocol::authority::key_authority_map amap;
	 public_key_type pkt("STM6kbKsZj5kY5QrG8huATPtwfVmZmKzFDfUXz1eEbKYF58LorAxF");
	 auto binary = pkt.key_data;
	 for(auto b : binary.data)
	 {
		 cout << (b & 0xff)  << " ";
	 }
	 cout << endl;
	 auto check = fc::ripemd160::hash( binary.data, binary.size() )._hash[0];
	 cout << check << endl;
	// auto owner = op.owner;
	auto chain_id_str = "2ac122bd70a2f74d6f761c331f4c4da1028b783cc185d23bf5449ac5af49e198";
	auto chain_id = fc::sha256(chain_id_str);
	auto d = tx.sig_digest(chain_id);
	cout << "digest in main:" << d.str() << endl;
	auto sig = tx.signatures[0];
	auto recPubKey = fc::ecc::public_key(sig,d);
	auto pubKeyType = public_key_type(recPubKey);
	cout << "recover public key:" <<  fc::json::to_string(pubKeyType) << endl;
	cout << "----------------" << endl;
	auto sigs  = tx.get_signature_keys(chain_id);
	cout << "================" << endl;
	for(auto& sig : sigs) {
		cout << "get signatures key:"  << fc::json::to_string(sig) << endl;
	}
	authority owner(1, pkt, 1);
	auto get_active  = [&]( const string& name ) { return owner; };
	auto get_owner = [&]( const string& name ) { return owner; };
	auto get_posting = [&]( const string& name ) { return owner; };
	tx.verify_authority(chain_id, get_active, get_owner, get_posting);
	//pack(enc, chain_id);
	//pack(enc, tx);
	//auto pstr = enc.result().str();
	//cout << tx.ref_block_num << endl;	
	//cout << pstr << endl;
}
