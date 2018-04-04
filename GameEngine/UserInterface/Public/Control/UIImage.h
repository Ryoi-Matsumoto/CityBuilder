#pragma once
#include "..\Framework\UIControl.h"

class FUIImage : public FUIControl
{
protected:
	void OnRendering() override;

public:
	FUIImage();
	uint GetWidth() const override { return _Image->GetWidth(); }
	uint GetHeight() const override { return _Image->GetHeight(); }	

	UIAttribute(FR2DObject*, Image)
	UIAttribute(float4, Color)
};