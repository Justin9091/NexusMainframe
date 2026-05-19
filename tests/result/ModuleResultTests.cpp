#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include "result/ModuleResult.hpp"

TEST_CASE("ModuleResult::alreadyLoaded returns 409 with module name", "[moduleresult]") {
    auto r = ModuleResult::alreadyLoaded("my-module");

    REQUIRE(r.httpStatus() == 409);
    REQUIRE(r.isOk()       == false);
    REQUIRE(r.message().find("my-module") != std::string::npos);
}

TEST_CASE("ModuleResult::notLoaded returns 409 with module name", "[moduleresult]") {
    auto r = ModuleResult::notLoaded("other-module");

    REQUIRE(r.httpStatus() == 409);
    REQUIRE(r.isOk()       == false);
    REQUIRE(r.message().find("other-module") != std::string::npos);
}

TEST_CASE("ModuleResult inherits Result::toJson", "[moduleresult]") {
    auto j = nlohmann::json::parse(ModuleResult::alreadyLoaded("mod").toJson());

    REQUIRE(j["success"] == false);
    REQUIRE_FALSE(j["output"].get<std::string>().empty());
}
