#include "robotparam.h"

void inputInit(Input* input) {
	input->theta		= 0;
	input->thetadot = 0;
	input->x				= 0;
	input->v				= 0;
	input->pitch		= 0;
	input->pitchdot = 0;
}

void outputInit(Output* output) {
	output->Tp		 = 0;
	output->Twheel = 0;
}

float Kcoeff[12][4]			= { 28.45590904, -26.14663189, -71.85491677, -57.67693323,
														20.81935214, 31.04311745, -99.97647312, 80.63747638,
														34.35211267, -62.5255255, 3.744783184, -13.39610566,
														9.539809458, -5.674417183, -9.042172469, 15.38011589,
														-8.099733304, 18.91800462, -15.63725351, -17.56065062,
														-13.46493183, 51.6815939, -63.47046464, 31.58875259,
														4.946146057, -5.071503538, -5.553098397, -20.03619027,
														-13.6641625, 51.30866802, -62.03695631, 31.35820391,
														-21.28992656, 81.7157749, -100.3556162, 49.94620332,
														51.22721124, -119.6479665, 98.89867476, 111.0633064,
														-4.99553123, 11.91361564, -11.0718808, 4.99317801,
														4.988162345, -11.37121624, 9.311143603, 4.877697792 };

float Kcoeff_wbc[40][6] = { -1.5782, -2.3503, 0.2456, 2.8044, -0.5538, -0.3792,
														-2.3012, -2.4472, 0.6088, 3.0067, -0.9434, -0.696,
														-1.7025, -1.7614, -0.1611, 1.1078, 0.4748, 0.1353,
														-0.6718, -0.8143, 0.0887, 0.5051, 0.089, 0.0446,
														-5.6233, -33.9127, 11.197, 25.7004, -12.3731, 12.9587,
														-2.1591, -2.472, 1.4636, 0.6844, -2.3293, 2.2648,
														-4.7678, 13.5134, -19.3277, -9.8997, 12.2251, -8.7767,
														-0.1143, -0.251, -4.5483, -0.1645, 2.2075, -0.8787,
														-5.7111, 10.6213, 1.6627, -6.8939, 1.6006, -3.6871,
														-1.1684, 1.8163, 0.6895, -0.9931, 0.0233, -0.9174,
														-1.5782, 0.2456, -2.3503, -0.5538, 2.8044, -0.3792,
														-2.3012, 0.6088, -2.4472, -0.9434, 3.0067, -0.696,
														1.7025, 0.1611, 1.7614, -0.4748, -1.1078, -0.1353,
														0.6718, -0.0887, 0.8143, -0.089, -0.5051, -0.0446,
														-4.7678, -19.3277, 13.5134, 12.2251, -9.8997, -8.7767,
														-0.1143, -4.5483, -0.251, 2.2075, -0.1645, -0.8787,
														-5.6233, 11.197, -33.9127, -12.3731, 25.7004, 12.9587,
														-2.1591, 1.4636, -2.472, -2.3293, 0.6844, 2.2648,
														-5.7111, 1.6627, 10.6213, 1.6006, -6.8939, -3.6871,
														-1.1684, 0.6895, 1.8163, 0.0233, -0.9931, -0.9174,
														3.4869, 5.0171, -12.4427, -7.1564, 3.2676, 9.4463,
														4.6614, 5.0502, -15.4945, -7.5485, 3.907, 11.7847,
														-3.0347, 4.4495, 1.1615, -3.7765, -0.9687, 4.441,
														-1.2444, 1.8184, 0.2787, -1.4727, -0.1415, 1.9262,
														9.2991, 46.4478, -2.144, -48.64, -13.5728, 74.2038,
														5.3844, -5.3733, 0.8031, 5.7666, -3.9449, 6.7014,
														8.8266, -16.0393, -60.4417, 20.639, 17.617, -28.6534,
														-0.0412, 2.2207, -7.3757, -0.258, -1.1245, -0.1112,
														-9.8365, -28.2392, 14.3279, 21.4837, -5.5521, -3.5966,
														-1.3797, -4.4538, 1.8205, 2.8596, -0.5427, 0.0419,
														3.4869, -12.4427, 5.0171, 3.2676, -7.1564, 9.4463,
														4.6614, -15.4945, 5.0502, 3.907, -7.5485, 11.7847,
														3.0347, -1.1615, -4.4495, 0.9687, 3.7765, -4.441,
														1.2444, -0.2787, -1.8184, 0.1415, 1.4727, -1.9262,
														8.8266, -60.4417, -16.0393, 17.617, 20.639, -28.6534,
														-0.0412, -7.3757, 2.2207, -1.1245, -0.258, -0.1112,
														9.2991, -2.144, 46.4478, -13.5728, -48.64, 74.2038,
														5.3844, 0.8031, -5.3733, -3.9449, 5.7666, 6.7014,
														-9.8365, 14.3279, -28.2392, -5.5521, 21.4837, -3.5966,
														-1.3797, 1.8205, -4.4538, -0.5427, 2.8596, 0.0419 };


bool jumpFlag						= false;
