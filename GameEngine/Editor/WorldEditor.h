#pragma once
#include "Common.h"
#include "EditorInterface.h"
#include "World\World.h"
#include "UserInterface\UIControl.h"
#include "UserInterface\UITextBlock.h"
#include "Grid.h"

class FWorldEditor : public FUIControl
{
private:
	float3 CameraPosition;	
	float CameraRotationVertical, CameraRotationHorizontal;
	Matrix ProjectionMatrix;
	Matrix ViewMatrix;
	bool PrevMouseRightButton;
	bool PrevMouseMiddleButton;
	int2 PrevOnMouseRightButtonPressed;
	int2 PrevOnMouseMiddleButtonPressed;

private:
	FUITextBlock* TextBlock;
	FWorld* World;
	FGrid* Grid;
	ITexture* RenderTarget;
	IRenderTargetView* RenderTargetView;
	IDepthStencilView* DepthStencilView;
	
public:
	FWorldEditor();
	void AddStreet(Vector2 StartPoint, Vector2 EndPoint);
};