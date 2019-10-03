#include "test_blade_sdr.h"

#include <boost/test/test_tools.hpp>
using namespace boost::unit_test;

#include <windows.h>
#include <vector>
#include <chrono>

#include "../bladerf_sdr/blade_sdr.h"

void bladesdr_open_close()
{
	BladeSdr sdr;
	BOOST_REQUIRE(sdr.open());

	sdr.close();
	BOOST_CHECK(! sdr.is_open());
}


void bladesdr_set_rx_tx()
{
	BladeSdr sdr;
	BOOST_REQUIRE(sdr.open());

	uint32_t sr = 1000000;
	BOOST_CHECK(sdr.set_sample_rate(BLADERF_CHANNEL_RX(0), sr));
	uint32_t sr_0 = sdr.get_sample_rate(BLADERF_CHANNEL_RX(0));
	BOOST_CHECK(sr_0 == sr);

	auto ts = sdr.get_timestamp(BLADERF_RX);

	uint32_t sr_1 = sdr.get_sample_rate(BLADERF_CHANNEL_RX(1));
	uint32_t sr_2 = sdr.get_sample_rate(BLADERF_CHANNEL_TX(0));
	
	BladeSdr::ChannelInfo info_rx_0, info_rx_1, info_tx_0, info_tx_1;
	
	info_rx_0.bandwidth = 100000;
	info_rx_0.sample_rate = 1000000;
	info_rx_0.frequency = 1500000000;
	BOOST_CHECK(sdr.set_channel_info(BLADERF_CHANNEL_RX(0), info_rx_0));

	info_tx_0.bandwidth = 100000;
	info_tx_0.sample_rate = 1000000;
	info_tx_0.frequency = 1800000000;
	BOOST_CHECK(sdr.set_channel_info(BLADERF_CHANNEL_TX(0), info_tx_0));

	BOOST_CHECK(sdr.get_channel_info(BLADERF_CHANNEL_RX(0), info_rx_0));
	BOOST_CHECK(sdr.get_channel_info(BLADERF_CHANNEL_RX(1), info_rx_1));
	BOOST_CHECK(sdr.get_channel_info(BLADERF_CHANNEL_TX(0), info_tx_0));
	BOOST_CHECK(sdr.get_channel_info(BLADERF_CHANNEL_TX(1), info_tx_1));

	BOOST_CHECK(info_rx_0 == info_rx_1);
	BOOST_CHECK(info_tx_0 == info_tx_1);
}

void bladesdr_sync_recv()
{
	BladeSdr sdr;

	BOOST_REQUIRE(sdr.open());

	int count = 100000;
	std::vector<int16_t> buffer(count * 2);

	BladeSdr::ChannelInfo info_rx_0;
	info_rx_0.bandwidth = 100000;
	info_rx_0.sample_rate = 1000000;
	info_rx_0.frequency = 103900000;
	BOOST_CHECK(sdr.set_channel_info(BLADERF_CHANNEL_RX(0), info_rx_0));
	
	BladeSdr::SyncConfig config(count, info_rx_0.sample_rate);
	BOOST_CHECK(sdr.sync_config(BLADERF_RX_X1, config));

	BOOST_CHECK(sdr.enable_module(BLADERF_RX, true));

	auto t0 = std::chrono::system_clock::now();
	for (int i = 0; i < 50; i++) {
		auto ta = std::chrono::system_clock::now();
		
		BOOST_CHECK(sdr.sync_recv(buffer.data(), count));
	
		auto da = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now() - ta);
		std::cout << "  recv data " << i << " : " << da.count() << " ms \n";
	}
	auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now() - t0);
	std::cout << "recv data " <<  delta.count() << " ms \n";

	BOOST_CHECK(sdr.enable_module(BLADERF_RX, false));
	sdr.close();
}


void bladesdr_sync_send()
{
	BladeSdr sdr;

	BOOST_REQUIRE(sdr.open());

	int count = 100000;
	std::vector<int16_t> buffer(count * 2);

	BladeSdr::ChannelInfo info_tx_0;
	info_tx_0.bandwidth = 100000;
	info_tx_0.sample_rate = 1000000;
	info_tx_0.frequency = 106900000;
	BOOST_CHECK(sdr.set_channel_info(BLADERF_CHANNEL_TX(0), info_tx_0));
	   
	BladeSdr::SyncConfig config(count, info_tx_0.sample_rate);
	BOOST_CHECK(sdr.sync_config(BLADERF_TX_X1, config));

	BOOST_CHECK(sdr.enable_module(BLADERF_TX, true));

	auto t0 = std::chrono::system_clock::now();
	for (int i = 0; i < 200; i++) {
		auto ta = std::chrono::system_clock::now();
		
		BOOST_CHECK(sdr.sync_send(buffer.data(), count));
		
		auto da = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now() - ta);
		std::cout << "  send data " << i << " : " << da.count() << " ms \n";
	}
	auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now() - t0);
	std::cout << "send data " << delta.count() << " ms \n";

	BOOST_CHECK(sdr.enable_module(BLADERF_TX, false));
	sdr.close();
}

void bladesdr_sync_recv_send()
{
	BladeSdr sdr;

	BOOST_REQUIRE(sdr.open());

	int count = 100000;
	std::vector<int16_t> buffer(count * 2);

	BladeSdr::ChannelInfo info_rx_0;
	info_rx_0.bandwidth = 100000;
	info_rx_0.sample_rate = 1000000;
	info_rx_0.frequency = 103900000;
	BOOST_CHECK(sdr.set_channel_info(BLADERF_CHANNEL_RX(0), info_rx_0));

	BladeSdr::SyncConfig config_rx(count, info_rx_0.sample_rate);
	BOOST_CHECK(sdr.sync_config(BLADERF_RX_X1, config_rx));

	BladeSdr::ChannelInfo info_tx_0;
	info_tx_0.bandwidth = 100000;
	info_tx_0.sample_rate = 1000000;
	info_tx_0.frequency = 106900000;
	BOOST_CHECK(sdr.set_channel_info(BLADERF_CHANNEL_TX(0), info_tx_0));

	BladeSdr::SyncConfig config_tx(count, info_tx_0.sample_rate);
	BOOST_CHECK(sdr.sync_config(BLADERF_TX_X1, config_tx));

	BOOST_CHECK(sdr.enable_module(BLADERF_RX, true));
	BOOST_CHECK(sdr.enable_module(BLADERF_TX, true));

	auto t0 = std::chrono::system_clock::now();
	for (int i = 0; i < 200; i++) {
		auto ta = std::chrono::system_clock::now();
		
		BOOST_CHECK(sdr.sync_recv(buffer.data(), count));
		// do something ...
		Sleep(60);
		BOOST_CHECK(sdr.sync_send(buffer.data(), count));
		
		auto da = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now() - ta);
		std::cout << "  a data " << i << " : " << da.count() << " ms \n";
	}
	auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now() - t0);
	std::cout << "send data " << delta.count() << " ms \n";

	BOOST_CHECK(sdr.enable_module(BLADERF_RX, false));
	BOOST_CHECK(sdr.enable_module(BLADERF_TX, false));

	sdr.close();
}

void bladesdr_sync_recv_metadata()
{
	BladeSdr sdr;
	BOOST_REQUIRE(sdr.open());

	int count = 100000;
	std::vector<int16_t> buffer(count * 2);

	BladeSdr::ChannelInfo info_rx_0;
	info_rx_0.bandwidth = 100000;
	info_rx_0.sample_rate = 1000000;
	info_rx_0.frequency = 103900000;
	BOOST_CHECK(sdr.set_channel_info(BLADERF_CHANNEL_RX(0), info_rx_0));

	bladerf_metadata meta;
	memset(&meta, 0, sizeof(meta));
	meta.flags = BLADERF_META_FLAG_RX_NOW;

	BladeSdr::SyncConfig config_rx(count, info_rx_0.sample_rate);
	config_rx.format = BLADERF_FORMAT_SC16_Q11_META;
	
	// 注意：每次停止 module 后，都需要重新配置.
	BOOST_CHECK(sdr.sync_config(BLADERF_RX_X1, config_rx));
	
	BOOST_CHECK(sdr.enable_module(BLADERF_RX, true));
	{
		auto t0 = std::chrono::system_clock::now();
		uint64_t prev = 0;

		Sleep(40);
		// get_timestamp 从启动 module 开始计数. 
		std::cout << "start timestamp " << sdr.get_timestamp(BLADERF_RX) << "\n";

		for (int i = 0; i < 10; i++) {
			BOOST_CHECK(bladerf_sync_rx(sdr.device(), buffer.data(), count, &meta, 5000) == 0);
			// do something ...

			std::cout << "current timestamp " << sdr.get_timestamp(BLADERF_RX) << "\n";
			// meta.timestamp 从第一个样点开始计数。
			std::cout << "recv timestamp " << i << " : " << meta.timestamp << "\n";
			prev = meta.timestamp;
			Sleep(60);
		}
		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now() - t0);
		std::cout << "send data " << delta.count() << " ms \n";
	}
	BOOST_CHECK(sdr.enable_module(BLADERF_RX, false));

	sdr.close();
}