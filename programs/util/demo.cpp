#include <iostream>

#include <fc/crypto/elliptic.hpp>
#include <fc/io/json.hpp>
#include <steem/utilities/key_conversion.hpp>
#include <steem/protocol/hardfork.hpp>
#include <steem/chain/steem_object_types.hpp>

struct ObjC {
  bool is_ok;
};

// 特殊类型，将一个json中的value转为一个对象
void to_variant(const ::ObjC &v, fc::variant &vo) {
// 将 ObjC 映射为一个字符串
  if (v.is_ok) {
    vo = std::string("ok");
  } else {
    vo = std::string("not ok");
  }
}

void from_variant(const fc::variant &vo, ::ObjC &v) {
  auto value = vo.as<std::string>();
  if (value == "ok") {
    v.is_ok = true;
  }
}

// 特殊类型，反射声明
FC_REFLECT_TYPENAME(::ObjC)

// 普通类型，直接按以下格式声明即可，类似于go json.Marshal，走"反射"。
struct ObjB {
  int64_t b_value;
  std::vector<int64_t> BValue;
};

struct ObjA {
  std::string aa;
  std::string AA;
  ObjB bb;
  ObjC cc;
  int64_t internal; // 未使用FC_REFLECT声明，不参与json序列化。
};

// 对Object进行反射声明，成员用()分隔
FC_REFLECT(::ObjB, (b_value)(BValue))
FC_REFLECT(::ObjA, (aa)(AA)(bb)(cc))

int main(int argc, char **argv) {
  auto priv_key = fc::ecc::private_key::regenerate(fc::sha256::hash(std::string(argv[1])));
  auto pub_key = priv_key.get_public_key();
  auto pub_key_type = steem::protocol::public_key_type(pub_key);
  auto pub_key_str = std::string(pub_key_type);
  std::cout << steem::utilities::key_to_wif(priv_key) << std::endl;
  std::cout << pub_key_str << std::endl;

  std::cout << "=== JSON DEMO ===" << std::endl;
  ObjA oa0;
  oa0.aa = "aa field is aa";
  oa0.AA = "AA field is AA";
  oa0.bb.b_value = 1000;
  oa0.bb.BValue = {0, 1, 2, 3, 4};
  oa0.cc.is_ok = true;
  oa0.internal = 1;
  auto ss = fc::json::to_string(oa0);
  std::cout << "fc::json::to_string(oa) = " << ss << std::endl;

  auto decoded = fc::json::from_string(ss);
  auto oa1 = decoded.as<::ObjA>();
  FC_ASSERT(oa1.cc.is_ok == oa0.cc.is_ok);
}
