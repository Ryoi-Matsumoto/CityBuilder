#include "..\..\Public\Control\UIImage.h"

FUIImage::FUIImage()
	: _Color(float(1))
{
}

void FUIImage::OnRendering()
{
	_Image->Render(Renderer, Area.Position, _Color);
}