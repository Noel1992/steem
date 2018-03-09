#include <steem/plugins/demo_api/demo_api.hpp>
#include <steem/plugins/demo_api/demo_api_plugin.hpp>

namespace steem {
namespace plugins {
namespace demo {

namespace detail {

class demo_api_impl {
 public:
  demo_api_impl() {}
  ~demo_api_impl() {}

  // get_sum 就是我们提供的一个API方法，将输入的数组进行求和
  get_sum_return get_sum(const get_sum_args &args) const {
    get_sum_return final{0};
    for (auto num : args.nums) {
      final.sum += num;
    }
    return final;
  }
};
}

demo_api::demo_api() : my(new detail::demo_api_impl()) {
  JSON_RPC_REGISTER_API(STEEM_DEMO_API_PLUGIN_NAME);
}

demo_api::~demo_api() {}

// 需要注意创建demo_api的时机，因为demo_api的构造函数中会调用JSON RPC插件去注册API，因此
// 需要等JSON RPC先初始化好，plugin_initialize被调用时，会先注册demo_api_plugin的依赖
// 模块，因此可以确保此时JSON RPC插件此时已经注册完毕。
void demo_api_plugin::plugin_initialize(const appbase::variables_map &options) {
  api = std::make_shared<demo_api>();
}

DEFINE_LOCKLESS_APIS( demo_api, (get_sum)
)

}
}
}
