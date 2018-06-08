#include <steem/chain/dao.hpp>
#include <chainbase/chainbase.hpp>
#include <fc/log/logger.hpp>

namespace steem { namespace chain {

const account_object* account_dao::find_by_name( const account_name_type& name ) const{
	// lock handled by the caller
	ilog("account by name called: ${name}", ("name",name));
	const auto& idx = _idx.indicies().get<by_name>();
	auto itr = idx.find(name);
	if (itr == idx.end())
  	      return nullptr;
    return &*itr;
}

const account_object& account_dao::get_by_name( const account_name_type& name ) const{
	// lock handled by the caller
	auto obj = find_by_name(name);
	if(!obj) BOOST_THROW_EXCEPTION( std::out_of_range( "unknown key" ) );
	return *obj;
}

}; }
