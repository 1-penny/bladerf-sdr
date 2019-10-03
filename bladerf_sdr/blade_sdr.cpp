#include "blade_sdr.h"

#include <math.h>
#include <stdio.h>

BladeSdr::BladeSdr(bool verb)
	: m_device(nullptr), m_verb(verb)
{
}

BladeSdr::~BladeSdr()
{
	close();
}

bladerf* BladeSdr::device()
{
	return m_device;
}

bool BladeSdr::open() 
{
	if (!is_open()) {
		int status = bladerf_open(&m_device, nullptr);
		if (status != 0) {
			m_device = nullptr;
		}
	}

	if (is_open()) {
		get_channel_info(BLADERF_CHANNEL_RX(0), m_info_rx);
		get_channel_info(BLADERF_CHANNEL_TX(0), m_info_tx);
	}

	return is_open();
}

bool BladeSdr::is_open()
{
	return m_device != nullptr;
}

void BladeSdr::close()
{
	if (m_device) {
		bladerf_close(m_device);
		m_device = nullptr;

		m_info_rx = ChannelInfo();
		m_info_tx = ChannelInfo();
	}
}

bool BladeSdr::set_sample_rate(bladerf_channel ch, uint32_t sr)
{
	if (!is_open())
		return false;

	int status = bladerf_set_sample_rate(m_device, ch, sr, nullptr);
	if (status == 0) {
		if (BLADERF_CHANNEL_IS_TX(ch))
			m_info_tx.sample_rate = sr;
		else
			m_info_rx.sample_rate = sr;
	}

	return status == 0;
}

uint32_t BladeSdr::get_sample_rate(bladerf_channel ch)
{
	if (!is_open())
		return 0;

	uint32_t sr = 0;
	int status = bladerf_get_sample_rate(m_device, ch, &sr);
	return status == 0 ? sr : 0;
}

bool BladeSdr::set_frequency(bladerf_channel ch, uint64_t freq)
{
	if (!is_open())
		return false;

	int status = bladerf_set_frequency(m_device, ch, freq);
	if (status == 0) {
		if (BLADERF_CHANNEL_IS_TX(ch))
			m_info_tx.frequency = freq;
		else
			m_info_rx.frequency = freq;
	}
	return status == 0;
}

uint64_t BladeSdr::get_frequency(bladerf_channel ch)
{
	if (!is_open())
		return 0;

	uint64_t freq = 0;
	int status = bladerf_get_frequency(m_device, ch, &freq);
	return status == 0 ? freq : 0;
}

bool BladeSdr::set_bandwidth(bladerf_channel ch, uint32_t bw)
{
	if (!is_open())
		return false;

	int status = bladerf_set_bandwidth(m_device, ch, bw, nullptr);
	if (status == 0) {
		if (BLADERF_CHANNEL_IS_TX(ch))
			m_info_tx.bandwidth = bw;
		else
			m_info_rx.bandwidth = bw;
	}

	return status == 0;
}

uint32_t BladeSdr::get_bandwidth(bladerf_channel ch)
{
	if (!is_open())
		return 0;

	uint32_t bw = 0;
	int status = bladerf_get_bandwidth(m_device, ch, &bw);
	return status == 0 ? bw : 0;
}

bool BladeSdr::set_gain(bladerf_channel ch, int gain)
{
	if (!is_open())
		return false;

	int status = bladerf_set_gain(m_device, ch, gain);
	if (status == 0) {
		if (BLADERF_CHANNEL_IS_TX(ch))
			m_info_tx.gain = gain;
		else
			m_info_rx.gain = gain;
	}

	return status == 0;
}

int BladeSdr::get_gain(bladerf_channel ch)
{
	if (!is_open())
		return 0;

	int gain = 0;
	int status = bladerf_get_gain(m_device, ch, &gain);
	return status == 0 ? gain : 0;
}

uint64_t BladeSdr::get_timestamp(bladerf_direction dir)
{
	if (!is_open())
		return 0;

	bladerf_timestamp timestamp;
	int status = bladerf_get_timestamp(m_device, dir, &timestamp);
	return status == 0 ? timestamp : 0;
}

bool BladeSdr::get_channel_info(bladerf_channel ch, ChannelInfo& info)
{
	if (!is_open())
		return false;

	info.sample_rate = get_sample_rate(ch);
	if (info.sample_rate == 0)
		return false;

	info.frequency = get_frequency(ch);
	if (info.frequency == 0)
		return false;

	info.bandwidth = get_bandwidth(ch);
	if (info.bandwidth == 0)
		return false;

	return true;
}

bool BladeSdr::set_channel_info(bladerf_channel ch, ChannelInfo info)
{
	if (!is_open())
		return false;

	if (info.bandwidth > 0) {
		if (!set_bandwidth(ch, info.bandwidth))
			return false;
	}

	if (info.sample_rate > 0) {
		if (!set_sample_rate(ch, info.sample_rate))
			return false;
	}

	if (info.frequency > 0) {
		if (!set_frequency(ch, info.frequency))
			return false;
	}

	if (info.gain >= 0) {
		if (!set_gain(ch, info.gain))
			return false;
	}

	return true;
}

bool BladeSdr::sync_recv(void * data, unsigned int count)
{
	if (!is_open() || data == nullptr || count <= 0)
		return false;

	int status = bladerf_sync_rx(m_device, data, count, nullptr, 5000);
	return status == 0;
}

bool BladeSdr::sync_send(void* data, unsigned int count)
{
	if (!is_open() || data == nullptr || count <= 0)
		return false;

	int status = bladerf_sync_tx(m_device, data, count, nullptr, 5000);
	return status == 0;
}

bool BladeSdr::enable_module(bladerf_channel ch, bool enable)
{
	if (!is_open())
		return false;

	int status = bladerf_enable_module(m_device, ch, enable);
	return status == 0;
}

bool BladeSdr::sync_config(bladerf_channel_layout layout, SyncConfig config)
{
	if (!is_open())
		return false;

	int status = bladerf_sync_config(m_device, layout, config.format, 
		config.num_buffers, config.buffer_size, config.num_transfers, config.stream_timeout);
	return status == 0;
}

BladeSdr::ChannelInfo::ChannelInfo()
{
	sample_rate = 0;
	bandwidth = 0;
	frequency = 0;
	gain = -1;
}

bool BladeSdr::ChannelInfo::operator == (const ChannelInfo& rhs) const
{
	return sample_rate == rhs.sample_rate 
		&& bandwidth == rhs.bandwidth && frequency == rhs.frequency
		&& gain == rhs.gain;
}

BladeSdr::SyncConfig::SyncConfig()
{
	format = BLADERF_FORMAT_SC16_Q11;

	buffer_size = 8 * 1024;
	num_buffers = 16;
	num_transfers = 8;
	stream_timeout = 3500;
}

BladeSdr::SyncConfig::SyncConfig(uint32_t samples, uint64_t sample_rate)
{
	format = BLADERF_FORMAT_SC16_Q11;

	int buffer_uint = 8 * 1024;
	uint32_t total = ceil(samples / (float)(buffer_uint)) * buffer_uint * 4;
	
	buffer_size = total / 16;
	num_buffers = 16;
	num_transfers = 8;

	float ms = samples * 1000 / (float)sample_rate;
	stream_timeout = ceil(ms / 500.0) * 500;
}