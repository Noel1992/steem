#pragma once

#include <utility>
#include <iostream>

#include <boost/multi_index_container.hpp>
#include <steem/chain/steem_objects.hpp>
#include <steem/chain/account_object.hpp>

using namespace std;

namespace steem { namespace chain {
	class account_dao
	{
		public:
			account_dao(chainbase::generic_index<account_index> idx):_idx(idx){}
			const account_object* find_by_name( const account_name_type& name )const;
			const account_object& get_by_name( const account_name_type& name )const;

		private:
			chainbase::generic_index<account_index> _idx;
	};
};}
