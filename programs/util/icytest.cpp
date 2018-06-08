#include <appbase/application.hpp>
#include <steem/manifest/plugins.hpp>

#include <steem/protocol/asset_symbol.hpp>
#include <steem/protocol/asset_symbol.hpp>
#include <steem/protocol/config.hpp>
#include <steem/protocol/transaction.hpp>
#include <steem/protocol/types.hpp>
#include <steem/protocol/version.hpp>

#include <steem/utilities/git_revision.hpp>
#include <steem/utilities/key_conversion.hpp>
#include <steem/utilities/logging_config.hpp>

#include <steem/plugins/chain/chain_plugin.hpp>
#include <steem/plugins/p2p/p2p_plugin.hpp>
#include <steem/plugins/webserver/webserver_plugin.hpp>
#include <steem/protocol/sign_state.hpp>
#include <steem/protocol/exceptions.hpp>

#include <steem/chain/account_object.hpp>
#include <steem/chain/dao.hpp>

#include <fc/exception/exception.hpp>
#include <fc/git_revision.hpp>
#include <fc/interprocess/signals.hpp>
#include <fc/macros.hpp>
#include <fc/stacktrace.hpp>
#include <fc/thread/thread.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <csignal>
#include <iostream>
#include <vector>

using namespace boost::program_options;
using namespace steem::protocol;
using namespace steem::chain;
using steem::protocol::version;
using std::string;
using std::vector;
using fc::flat_set;
using namespace std;

void ASSERT_EQUAL(bool cond, const string &hint) {
    if (!(cond)) {
        std::cout << hint << std::endl;
    }
}

string &version_string() {
    static string v_str =
        "steem_blockchain_version: " + fc::string(STEEM_BLOCKCHAIN_VERSION) + "\n" +
        "steem_git_revision:       " + fc::string(steem::utilities::git_revision_sha) + "\n" +
        "fc_git_revision:          " + fc::string(fc::git_revision_sha) + "\n";
    return v_str;
}

void info() {
    std::cerr << "------------------------------------------------------\n\n";
    std::cerr << "            STARTING STEEM NETWORK\n\n";
    std::cerr << "------------------------------------------------------\n";
    std::cerr << "initminer public key: " << STEEM_INIT_PUBLIC_KEY_STR << "\n";
    std::cerr << "chain id: " << std::string(STEEM_CHAIN_ID) << "\n";
    std::cerr << "blockchain version: " << fc::string(STEEM_BLOCKCHAIN_VERSION) << "\n";
    std::cerr << "------------------------------------------------------\n";
}

fc::ecc::private_key get_init_private_key() {
    static string wif = "5K3gS2EyR5J87uer5YHEAy7BEfcz2Vna3AhsoKknh8eMxSY9qQn";
    auto key = steem::utilities::wif_to_key(wif);
    if (!key.valid()) {
        std::cout << "can't wif_to_key " << wif << " " << std::endl;
        exit(1);
    }
    return *key;
}

void fill_operation(account_create_operation &op, const string &name) {
    op.new_account_name = name;
    op.creator = STEEM_INIT_MINER_NAME;
    op.fee = asset(100, STEEM_SYMBOL);
    auto priv_key = fc::ecc::private_key::regenerate(fc::sha256::hash(name));
    op.owner = authority(1, priv_key.get_public_key(), 1);
    op.active = authority(2, priv_key.get_public_key(), 2);
    op.memo_key = priv_key.get_public_key();
    op.json_metadata = "{\"foo\":\"bar\"}";

	std::cout << "fee to string:" << fc::json::to_pretty_string(op.fee) << "--" << endl;
}

int __init(int argc, char **argv) {
    try {
        // Setup logging config
        options_description options;

        steem::utilities::set_logging_program_options(options);
        options.add_options()("backtrace", value<string>()->default_value("yes"), "Whether to print backtrace on SIGSEGV");

        appbase::app().add_program_options(options_description(), options);

        steem::plugins::register_plugins();
        appbase::app().set_version_string(version_string());

        bool initialized = appbase::app().initialize<steem::plugins::chain::chain_plugin, steem::plugins::p2p::p2p_plugin, steem::plugins::webserver::webserver_plugin>(argc, argv);

        info();

        if (!initialized)
            return 0;

        auto &args = appbase::app().get_args();

        try {
            fc::optional<fc::logging_config> logging_config = steem::utilities::load_logging_config(args, appbase::app().data_dir());
            if (logging_config)
                fc::configure_logging(*logging_config);
        } catch (const fc::exception &) {
            wlog("Error parsing logging config");
        }

        if (args.at("backtrace").as<string>() == "yes") {
            fc::print_stacktrace_on_segfault();
            ilog("Backtrace on segfault is enabled.");
        }
        appbase::app().startup(); //appbase::app().exec();
		cout << "application started" << endl;
        return 0;
    } catch (const boost::exception &e) {
        std::cerr << boost::diagnostic_information(e) << "\n";
    } catch (const fc::exception &e) {
        std::cerr << e.to_detail_string() << "\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
    } catch (...) {
        std::cerr << "unknown exception\n";
    }

    return -1;
}

void sign_block() {
    //// signed_block cutoff_block;
    //signed_block b = db.generate_block(db.get_slot_time(1), db.get_scheduled_witness(1), init_account_priv_key, database::skip_nothing);
    //b = db.generate_block(db.get_slot_time(1), db.get_scheduled_witness(1), init_account_priv_key, database::skip_nothing);
    //auto height = db.get_dynamic_global_properties().last_irreversible_block_num;
    //cout << "height:" << height << endl;
    //cout << "block number:" << b.block_num() << endl;

}

void new_account(const string &name) {
    //auto chain = appbase::app().find_plugin<steem::plugins::chain::chain_plugin>();
    steem::protocol::signed_transaction trx;
    string alice_name = name;
    auto alice_key = fc::ecc::private_key::regenerate(fc::sha256::hash(alice_name));
    account_create_operation alice_op;
    fill_operation(alice_op, alice_name);

    trx.expiration = fc::time_point_sec(1531606400);
    trx.operations.push_back(alice_op);

    auto init_account_priv_key = get_init_private_key();

    auto chain = appbase::app().find_plugin<steem::plugins::chain::chain_plugin>();
    trx.sign(init_account_priv_key, chain->db().get_chain_id());
    std::cout << " private key:" << steem::utilities::key_to_wif(init_account_priv_key) << std::endl;
    std::cout << " chain id:" << chain->db().get_chain_id().str() << std::endl;
    std::cout << " public key:" << std::string(steem::protocol::public_key_type(init_account_priv_key.get_public_key())) << std::endl;
    cout << fc::json::to_string(trx) << endl;
    trx.validate();

    steem::chain::database &db = chain->db();
    db.push_transaction(trx, 0);

    auto alice = db.get_account(alice_name);
    auto alice_auth = db.get<account_authority_object, by_account>(alice_name);
    ASSERT_EQUAL(alice.name == "alice", "name is not 'alice'");
    ASSERT_EQUAL(alice_auth.owner == authority(1, alice_key.get_public_key(), 1), "alice owner not equal");
    ASSERT_EQUAL(alice_auth.active == authority(2, alice_key.get_public_key(), 2), "alice active not equal");
    ASSERT_EQUAL(alice.memo_key == alice_key.get_public_key(), "alice mem key not equal");
}

template< typename AuthContainerType >
void verify_authority(const vector<AuthContainerType>& auth_containers, const flat_set<public_key_type>& sigs,
                       const authority_getter& get_active,
                       const authority_getter& get_owner,
                       const authority_getter& get_posting,
                       uint32_t max_recursion_depth = STEEM_MAX_SIG_CHECK_DEPTH,
                       bool allow_committe = false,
                       const flat_set< account_name_type >& active_approvals = flat_set< account_name_type >(),
                       const flat_set< account_name_type >& owner_approvals = flat_set< account_name_type >(),
                       const flat_set< account_name_type >& posting_approvals = flat_set< account_name_type >()
		)
{
{ try {
   flat_set< account_name_type > required_active;
   flat_set< account_name_type > required_owner;
   flat_set< account_name_type > required_posting;
   vector< authority > other;

   get_required_auth_visitor auth_visitor( required_active, required_owner, required_posting, other );

   for( const auto& a : auth_containers )
      auth_visitor( a );


   flat_set< public_key_type > avail;
   sign_state s(sigs,get_active,avail);
   s.max_recursion = max_recursion_depth;

   // fetch all of the top level authorities
   for( const auto& id : required_active )
   {
	   cout << " check authroty:" << s.check_authority(id) << "---" << endl;
	   cout << " check authroty(get_owner):" << s.check_authority(get_owner(id)) << "---" << endl;
      STEEM_ASSERT( s.check_authority(id) ||
                       s.check_authority(get_owner(id)),
                       tx_missing_active_auth, "Missing Active Authority ${id}", ("id",id)("auth",get_active(id))("owner",get_owner(id)) );
   }

   STEEM_ASSERT(
      !s.remove_unused_signatures(),
      tx_irrelevant_sig,
      "Unnecessary signature(s) detected"
      );
} FC_CAPTURE_AND_RETHROW( (auth_containers)(sigs) ) }
}
int main(int argc, char **argv) {
    __init(argc, argv);
    // new_account("alice");
    // std::cout << "exited cleanly \n";
	//auto fee = asset(9, STEEM_SYMBOL);
	//cout << fc::json::to_string(fee) << endl;
	//auto fee2 = fc::json::from_string("[\"9\",3,\"@@000000021\"]").as<asset>();
	//cout << (fee == fee2) << endl;
	//appbase::app().exec();
	try {
		auto chain = appbase::app().find_plugin<steem::plugins::chain::chain_plugin>();
		steem::chain::database &db = chain->db();
		account_object account = db.get_account("initminer");
		//const auto& idx = db.get_index<account_index>();
		//account_dao dao(idx);
		//auto account = dao.find_by_name("initminer");
		cout << account.post_count << endl;

		//cout << account->post_count << endl;
		//auto chain_id = db.get_chain_id();
        //auto get_active  = [&]( const string& name ) { return authority( db.get< account_authority_object, by_account >( name ).active ); };
	    //auto get_owner   = [&]( const string& name ) { return authority( db.get< account_authority_object, by_account >( name ).owner );  };
	    //auto get_posting = [&]( const string& name ) { return authority( db.get< account_authority_object, by_account >( name ).posting );  };
	    //auto tx = fc::json::from_file("/data0/steem/programs/util/new_account.json").as<signed_transaction>();
		//auto sig_keys = tx.get_signature_keys(chain_id);
		//verify_authority(tx.operations, sig_keys, get_active, get_owner, get_posting, 2);	
	} catch(std::exception e) {
		cout << " exeption: " << e.what() << endl;
	}
}
