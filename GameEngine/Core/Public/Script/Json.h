#pragma once

struct SJson : public SDynamic
{	
	bool IsError;

	SJson(const char* Text);
};