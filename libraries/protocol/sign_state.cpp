#include <iostream>

#include <steem/protocol/sign_state.hpp>
#include <fc/io/json.hpp>

using std::cout;
using std::endl;

namespace steem { namespace protocol {

bool sign_state::signed_by( const public_key_type& k )
{
	cout << "signed by:" << fc::json::to_string(k) << endl;
   auto itr = provided_signatures.find(k);
   if( itr == provided_signatures.end() )
   {
	   cout << "not found" << endl;
      auto pk = available_keys.find(k);
      if( pk  != available_keys.end() )
	  {
		  cout << " found in available_keys" << endl;
         return provided_signatures[k] = true;
	  }
	  cout << " not signed " << endl;
      return false;
   }
   cout << "signed success" << endl;
   return itr->second = true;
}

bool sign_state::check_authority( string id )
{
	cout << "check authority:" << id << endl;
   if( approved_by.find(id) != approved_by.end() ) return true;
   return check_authority( get_active(id) );
}

bool sign_state::check_authority( const authority& auth, uint32_t depth )
{
	cout << "check authoity by auth" << endl;
   uint32_t total_weight = 0;
   for( const auto& k : auth.key_auths )
   {
	   cout << " visit key auths " << endl;
      if( signed_by( k.first ) )
      {
         total_weight += k.second;
		  cout <<"total weight:" <<  total_weight << " " << k.second << endl;
         if( total_weight >= auth.weight_threshold )
            return true;
      }
   }

   for( const auto& a : auth.account_auths )
   {
	   cout << " visit by account " << endl;
      if( approved_by.find(a.first) == approved_by.end() )
      {
         if( depth == max_recursion )
            continue;
         if( check_authority( get_active( a.first ), depth+1 ) )
         {
            approved_by.insert( a.first );
            total_weight += a.second;
            if( total_weight >= auth.weight_threshold )
               return true;
         }
      }
      else
      {
         total_weight += a.second;
         if( total_weight >= auth.weight_threshold )
            return true;
      }
   }
   cout << "run to the end:" << total_weight <<  " " << auth.weight_threshold << endl;
   return total_weight >= auth.weight_threshold;
}

bool sign_state::remove_unused_signatures()
{
   vector<public_key_type> remove_sigs;
   for( const auto& sig : provided_signatures )
      if( !sig.second ) remove_sigs.push_back( sig.first );

   for( auto& sig : remove_sigs )
      provided_signatures.erase(sig);

   return remove_sigs.size() != 0;
}

sign_state::sign_state(
   const flat_set<public_key_type>& sigs,
   const authority_getter& a,
   const flat_set<public_key_type>& keys
   ) : get_active(a), available_keys(keys)
{
   for( const auto& key : sigs )
   {
      provided_signatures[ key ] = false;
	  cout << "provided sig:" <<  fc::json::to_string(key) << endl;
   }
   approved_by.insert( "temp"  );
}

} } // steem::protocol
