#pragma once

#define MAX_HIDDEN_AREA_VERTEX_NUM 100*3

namespace starvr {
	enum EVREye
	{
		Eye_Left = 0,
		Eye_Right = 1,
		Eye_Left_Side,
		Eye_Right_Side,
	};

	enum StarVRError {
		StarVRError_Failed = -1,
		StarVRError_None = 0,
		StarVRError_Unknown,

		StarVRError_DriverNotInit,
		StarVRError_OutOfBounds,

		StarVRError_TobiiServiceNotFound,
		StarVRError_TobiiVersionMismatch,

		StarVRError_EyeTrackingNotInit,
		StarVRError_EyeTrackerDeviceNotFound,

		StarVRError_SubprocessAlreadyRunning,

		StarVRError_TextureTypeNotSupported,
		StarVRError_TextureAlreadySubmitted
	};

	enum MV_Mode {
		MV_OVERLAY = -2,
		MV_NONE = -1,
		MV_MULTIVIEW = 0,
		MV_FOVEATED_RENDERING
	};
	struct StarVREyePose {
		vr::HmdVector3_t gazeOrigin;
		vr::HmdVector3_t gazeDirectionNormalized;
		vr::HmdVector2_t pupilCenter;
		float pupilDiameter;
		bool isOpen;
		bool isGazeOriginValid;
		bool isGazeDirectionValid;
	};
}