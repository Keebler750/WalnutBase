#pragma once

/*
Set up a fixed array for F18 Fuel use which will be used in the user drop down lists. 
This will create a look up table of values that coincide with the required parameters.

The array indexes will coincide with the indexes of the drop down list items and
therefor no matrix naming will be present. Care must be taken to correlate the
columns and rows with the proper data labels. IE: 10,000ft ALT might be index [3] and
this needs to be followed for accuracy. Since it is a fixed data array, once the data
has been entered into it PROPERLY, the data won't change.

SHOULD THIS BE A VECTOR to hold custom data? (It doesn't need to be resized, though...)
*/ 

	// BELOW: Define multi-dimensional array to hold fueling data, full array will include all drag
	// indexes and temperature ranges. Data is in LBs per nautical mile as per the F18C FPAS information

int F18_FuelDataLB_NM /*TEMP[4] DIDX[5]*/ /*MACH*/[6] /*ALT*/[11]

	// DRAG ID - 0 - 10C-temp:
{
					/*0ft, 1,000, 5,000, 10,000, 15,000...etc...45,000ft*/
	/*MACH 0*/		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

	/*MACH 0.5*/	{ 0, 15, 13, 11, 10, 12, 13, 15, 17, -1, -1 },

	/*MACH 0.6*/	{ 0, 17, 15, 13, 11, 10, 9, 9, 10, 10, -1 },

	/*MACH 0.7*/	{ 0, 20, 18, 15, 12, 10, 8, 8, 8, 7, 7 },

	/*MACH 0.8*/	{ 0, 24, 21, 19, 15, 12, 9, 7, 6, 6, 6 },

	/*MACH 0.85*/	{ 0, 26, 24, 21, 17, 13, 10, 8, 7, 7, 7 }
};

/*

TWO error checks, check for zero means no param selected; notify user, 
and -1 means No Data, no aircraft performance in that range (OUT OF FLIGHT PARAMS)

*/