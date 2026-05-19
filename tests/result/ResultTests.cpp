#include <catch2/catch_test_macros.hpp>
#include "result/Result.hpp"
#include <nlohmann/json.hpp>

TEST_CASE("Result factory methods return correct HTTP status", "[result]") {
    REQUIRE(Result::ok().httpStatus()         == 200);
    REQUIRE(Result::created().httpStatus()    == 201);
    REQUIRE(Result::badRequest().httpStatus() == 400);
    REQUIRE(Result::forbidden().httpStatus()  == 403);
    REQUIRE(Result::notFound().httpStatus()   == 404);
    REQUIRE(Result::conflict().httpStatus()   == 409);
    REQUIRE(Result::failed().httpStatus()     == 500);
}

TEST_CASE("Result::isOk distinguishes 2xx from errors", "[result]") {
    REQUIRE(Result::ok().isOk()         == true);
    REQUIRE(Result::created().isOk()    == true);
    REQUIRE(Result::badRequest().isOk() == false);
    REQUIRE(Result::notFound().isOk()   == false);
    REQUIRE(Result::conflict().isOk()   == false);
    REQUIRE(Result::failed().isOk()     == false);
}

TEST_CASE("Result stores custom message", "[result]") {
    REQUIRE(Result::ok("all good").message()           == "all good");
    REQUIRE(Result::notFound("no such item").message() == "no such item");
    REQUIRE(Result::failed("boom").message()           == "boom");
}

TEST_CASE("Result::toJson contains success flag and message", "[result]") {
    auto ok  = nlohmann::json::parse(Result::ok("done").toJson());
    auto err = nlohmann::json::parse(Result::notFound("missing").toJson());

    REQUIRE(ok["success"]  == true);
    REQUIRE(ok["output"]   == "done");
    REQUIRE(err["success"] == false);
    REQUIRE(err["output"]  == "missing");
}

TEST_CASE("Result default messages are non-empty", "[result]") {
    REQUIRE_FALSE(Result::ok().message().empty());
    REQUIRE_FALSE(Result::created().message().empty());
    REQUIRE_FALSE(Result::badRequest().message().empty());
    REQUIRE_FALSE(Result::notFound().message().empty());
    REQUIRE_FALSE(Result::conflict().message().empty());
    REQUIRE_FALSE(Result::forbidden().message().empty());
    REQUIRE_FALSE(Result::failed().message().empty());
}
