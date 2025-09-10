#pragma once

class BoneMask
{
public:
	enum class PreDefined
	{
		eUpperBody = 0,
		eLowerBody = 1,
		eNone
	};

	BoneMask(PreDefined option = PreDefined::eNone)
	{
		memset(weight, 0, sizeof(float) * 64);
		switch (option)
		{
		case PreDefined::eUpperBody:
			//weight[1]  = 0.9f;	// Pelvis
			weight[2] = 0.1f; // Spine
			weight[3] = 0.5f; // Spine1
			weight[4] = 1.0f; // Spine2
			weight[5] = 1.0f; // Neck
			weight[6] = 1.0f; // L Clavicle
			weight[7] = 1.0f; // L UpperArm
			weight[8] = 1.0f; // L Forearm
			weight[9] = 1.0f; // L Hand
			weight[10] = 1.0f; // L Finger0
			weight[11] = 1.0f; // L Finger01
			weight[13] = 1.0f; // L Finger1
			weight[14] = 1.0f; // L Finger11
			weight[16] = 1.0f; // L Finger2
			weight[17] = 1.0f; // L Finger21
			weight[19] = 1.0f; // L Finger3
			weight[20] = 1.0f; // L Finger31
			weight[22] = 1.0f; // L Finger4
			weight[23] = 1.0f; // L Finger41
			weight[25] = 1.0f; // R Clavicle
			weight[26] = 1.0f; // R UpperArm
			weight[27] = 1.0f; // R Forearm
			weight[28] = 1.0f; // R Hand
			weight[29] = 1.0f; // R Finger0
			weight[30] = 1.0f; // R Finger01
			weight[32] = 1.0f; // R Finger1
			weight[33] = 1.0f; // R Finger11
			weight[35] = 1.0f; // R Finger2
			weight[36] = 1.0f; // R Finger21
			weight[38] = 1.0f; // R Finger3
			weight[39] = 1.0f; // R Finger31
			weight[41] = 1.0f; // R Finger4
			weight[42] = 1.0f; // R Finger41
			weight[44] = 1.0f; // Head

			break;
		case PreDefined::eLowerBody:
			weight[1] = 0.9f; // Pelvis
			weight[2] = 0.4f; // Spine
			weight[3] = 0.1f; // Spine1
			weight[46] = 1.0f; // L Thigh
			weight[47] = 1.0f; // L Calf
			weight[48] = 1.0f; // L Foot
			weight[49] = 1.0f; // L Toe0
			weight[51] = 1.0f; // R Thigh
			weight[52] = 1.0f; // R Calf
			weight[53] = 1.0f; // R Foot
			weight[54] = 1.0f; // R Toe0

			break;
		default:
			break;
		}
	}

	float weight[64];
};
