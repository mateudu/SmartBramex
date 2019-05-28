#define BOOST_TEST_MODULE tests
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include "header/socket_helper.hpp"
#include "header/server.hpp"
#include "header/client_udp.hpp"

int dodaj(int i, int j)
{
    return i + j;
}

BOOST_AUTO_TEST_CASE(testDodaj)
{
    BOOST_CHECK(dodaj(2,2) == 4);
}

//server.h
BOOST_AUTO_TEST_CASE(testAdd)
{
    BOOST_CHECK(add(2,2) == 4);
}

//client_udp.h
BOOST_AUTO_TEST_CASE(testMultiply)
{
    BOOST_CHECK(multiply(2,3) == 6);
}