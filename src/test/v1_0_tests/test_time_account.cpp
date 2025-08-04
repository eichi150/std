#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../../code/time_account.h"

TEST_CASE("Time_Account basics"){
	Time_Account acc{"Minijob", "MJ"};

	CHECK(acc.get_entity() == "Minijob");
	CHECK(acc.get_alias() == "MJ");
	CHECK(acc.get_hours() == doctest::Approx(0.0f));
	CHECK(acc.get_entry().empty());
}

TEST_CASE("Set and get tag"){
	Time_Account acc{"Minijob", "MJ_tag"};
	acc.set_tag("work");
	CHECK(acc.get_tag() == "work");
}

TEST_CASE("Set and get hours"){
	Time_Account acc{"dev", "test"};

	acc.set_hours(5.5f);
	CHECK(acc.get_hours() == doctest::Approx(5.5f));
}

TEST_CASE("Add entry updates hours and entry list"){
	Time_Account acc{"Minijob", "MJ_entry"};

	std::tm time = {};

	Entry e1{1.5f, "Messung 1", time};
	Entry e2{3.0f, "Messung 2", time};

	acc.add_entry(e1);
	acc.add_entry(e2);
	CHECK(acc.get_hours() == doctest::Approx(4.5f));

	auto entries = acc.get_entry();
	REQUIRE(entries.size() == 2);
	CHECK(entries[0].description == "Messung 1");
	CHECK(entries[1].description == "Messung 2");
}

TEST_CASE("add_json_read_entry does not change hours"){
	Time_Account acc{"Job", "j"};

	std::tm time = {};
	Entry e1{2.0f, "Vorher gespeicherter Eintrag", time};
	acc.add_json_read_entry(e1);

	CHECK(acc.get_hours() == doctest::Approx(0.0f));
	CHECK(acc.get_entry().size() == 1);
	CHECK(acc.get_entry()[0].description == "Vorher gespeicherter Eintrag");
}
