#pragma once
#include <steem/plugins/json_rpc/utility.hpp>
#include <steem/protocol/types.hpp>
#include <fc/optional.hpp>
#include <fc/variant.hpp>
#include <fc/vector.hpp>

namespace steem {
namespace plugins {
namespace demo {

namespace detail {
class demo_api_impl;
}

// get_sum方法的输入参数
struct get_sum_args {
  std::vector<int64_t> nums;
};

// get_sum方法的输出参数
struct get_sum_return {
  int64_t sum;
};

class demo_api {
 public:
  demo_api();
  ~demo_api();

  DECLARE_API((get_sum))

 private:
  std::unique_ptr<detail::demo_api_impl> my;
};
}
}
}

// 将方法输入、输出参数进行反射
FC_REFLECT( steem::plugins::demo::get_sum_args, (nums)

)

FC_REFLECT( steem::plugins::demo::get_sum_return, (sum)

)