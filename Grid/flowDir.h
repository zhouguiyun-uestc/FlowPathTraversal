#pragma once

/*
*	flow direction
*	32	64	128
*	16	0	1
*	8	4	2
*/
enum class FlowDir :unsigned char
{
	Right = 1,
	BottomRight = 2,
	Bottom = 4,
	BottomLeft = 8,
	Left = 16,
	TopLeft = 32,
	Top = 64,
	TopRight = 128,
	NoData_Zero=0,
	NoData_255=255
};



