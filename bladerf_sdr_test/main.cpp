
#include <boost/test/included/unit_test.hpp>
using namespace boost::unit_test;

#include "test_blade_sdr.h"
#include "test_libbladerf.h"


test_suite* init_unit_test_suite(int argc, char* argv[])
{
	// ≤‚ ‘ libbladeRf API Ω”ø⁄.
	//framework::master_test_suite().add(BOOST_TEST_CASE(&libbladerf_open_close));

	// ≤‚ ‘ blade_sdr ¿‡.
	//framework::master_test_suite().add(BOOST_TEST_CASE(&bladesdr_open_close));
	//framework::master_test_suite().add(BOOST_TEST_CASE(&bladesdr_set_rx_tx)); 
	//framework::master_test_suite().add(BOOST_TEST_CASE(&bladesdr_sync_recv));
	//framework::master_test_suite().add(BOOST_TEST_CASE(&bladesdr_sync_send));
	//framework::master_test_suite().add(BOOST_TEST_CASE(&bladesdr_sync_recv_send));
	framework::master_test_suite().add(BOOST_TEST_CASE(&bladesdr_sync_recv_metadata));
	
	return 0;
}
