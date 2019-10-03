#ifndef BLADE_SDR_H
#define BLADE_SDR_H

#include <stdint.h>

#include <libbladeRF.h>
#pragma comment(lib, "bladeRF.lib")

class BladeSdr
{
public:
	// 通道参数.
	struct ChannelInfo
	{
		ChannelInfo();

		bool operator == (const ChannelInfo& rhs) const;

		uint32_t sample_rate;
		uint32_t bandwidth;
		uint64_t frequency;
		int gain;
	};

	// 同步接口配置.
	struct SyncConfig
	{
		SyncConfig();
		SyncConfig(uint32_t samples, uint64_t sample_rate);

		bladerf_format format;

		unsigned int num_buffers;
		unsigned int buffer_size;
		unsigned int num_transfers;
		unsigned int stream_timeout;
	};

public:
	BladeSdr(bool verb = false);
	~BladeSdr();

public:
	bool open();
	bool is_open();
	void close();

public:
	bool get_channel_info(bladerf_channel ch, ChannelInfo& info);
	bool set_channel_info(bladerf_channel ch, ChannelInfo info);

	bool set_sample_rate(bladerf_channel ch, uint32_t sr);
	uint32_t get_sample_rate(bladerf_channel ch);

	bool set_frequency(bladerf_channel ch, uint64_t freq);
	uint64_t get_frequency(bladerf_channel ch);

	bool set_bandwidth(bladerf_channel ch, uint32_t bw);
	uint32_t get_bandwidth(bladerf_channel ch);

	bool set_gain(bladerf_channel ch, int gain);
	int get_gain(bladerf_channel ch);
	
	/**
	 * 获取时间戳.
	 * 时间戳的数值由两个因素决定：1、enable_module 作为起始时刻；2、采样频率作为计数步长.
	 */
	uint64_t get_timestamp(bladerf_direction dir);

public:
	bool sync_config(bladerf_channel_layout layout, SyncConfig config);

	bool enable_module(bladerf_channel ch, bool enable);

	bool sync_recv(void* data, unsigned int count);
	bool sync_send(void* data, unsigned int count);

public:
	bladerf* device();

private:
	bladerf* m_device;
	bool m_verb;
	
	ChannelInfo m_info_tx, m_info_rx;
};

#endif //BLADE_SDR_H