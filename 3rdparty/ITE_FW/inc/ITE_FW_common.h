#pragma once

enum DisplayPanel : uint8_t {
	PANEL_LEFT = 0,
	PANEL_RIGHT = 1,
};

enum DisplayRefreshRate : uint8_t {
	REFRESH_RATE_72_HZ = 1,
	REFRESH_RATE_75_HZ = 2,
	REFRESH_RATE_89_HZ = 3,
	REFRESH_RATE_90_HZ = 0,
};