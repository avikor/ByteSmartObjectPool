#include "StackfullObjectPool.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


struct TrivialSturct
{
	int i;
	float f;
	double d;
};


TEST_CASE("simple int pool", "[StackfullObjectPool]")
{
	sop::StackfullObjectPool<int, 2U> intPool{};
	
	REQUIRE(intPool.capacity() == 2U);
	REQUIRE(intPool.size() == 0U);
	REQUIRE(!intPool.isFull());

	sop::PoolItem<int> pInt1 = intPool.request(1);
	
	REQUIRE(intPool.capacity() == 2U);
	REQUIRE(intPool.size() == 1U);
	REQUIRE(!intPool.isFull());

	REQUIRE(*pInt1 == 1);

	{
		auto pInt2 = intPool.request();

		REQUIRE(intPool.capacity() == 2U);
		REQUIRE(intPool.size() == 2U);
		REQUIRE(intPool.isFull());

		REQUIRE(*pInt2 == 0);

		REQUIRE_THROWS_AS(intPool.request(), sop::max_capacity_exception);
	}

	REQUIRE(intPool.capacity() == 2U);
	REQUIRE(intPool.size() == 1U);
	REQUIRE(!intPool.isFull());
	
	auto pInt3 = intPool.request();
	REQUIRE(*pInt3 == 0);

	REQUIRE(intPool.capacity() == 2U);
	REQUIRE(intPool.size() == 2U);
	REQUIRE(intPool.isFull());
}

TEST_CASE("trivial struct pool", "[StackfullObjectPool]")
{
	sop::StackfullObjectPool<TrivialSturct, 2U> trivialSturctPool{};

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 0U);
	REQUIRE(!trivialSturctPool.isFull());

	sop::PoolItem<TrivialSturct> trivial1 = trivialSturctPool.request(1, 2.5f, 3.3);

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 1U);
	REQUIRE(!trivialSturctPool.isFull());

	REQUIRE(trivial1->i == 1);
	REQUIRE(trivial1->f == 2.5f);
	REQUIRE(trivial1->d == 3.3);

	{
		auto trivial2 = trivialSturctPool.request();

		REQUIRE(trivialSturctPool.capacity() == 2U);
		REQUIRE(trivialSturctPool.size() == 2U);
		REQUIRE(trivialSturctPool.isFull());

		REQUIRE(trivial2->i == 0);
		REQUIRE(trivial2->f == 0.0f);
		REQUIRE(trivial2->d == 0.0);

		REQUIRE_THROWS_AS(trivialSturctPool.request(), sop::max_capacity_exception);
	}

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 1U);
	REQUIRE(!trivialSturctPool.isFull());

	auto trivial3 = trivialSturctPool.request();

	REQUIRE(trivialSturctPool.capacity() == 2U);
	REQUIRE(trivialSturctPool.size() == 2U);
	REQUIRE(trivialSturctPool.isFull());

	REQUIRE(trivial3->i == 0);
	REQUIRE(trivial3->f == 0.0f);
	REQUIRE(trivial3->d == 0.0);
}