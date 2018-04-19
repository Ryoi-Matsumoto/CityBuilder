#pragma once
#include "Rendering\Public\R2DRenderer.h"
#include "Rendering\Public\RViewport.h"
#include "UIApplication.h"
#include "UIWindow.h"
#include <Windows.h>

struct SUIViewRange
{
	uint ItemCount;
	uint RangeItemCount;
	uint TopItemIndex;

	SUIViewRange() : ItemCount(0), RangeItemCount(0), TopItemIndex(0) {}
};

struct SUIMargin
{
	uint Left;
	uint Top;
	uint Right;
	uint Bottom;

	SUIMargin(uint Top,  uint Bottom, uint Left, uint Right)
		: Left(Left)
		, Top(Top)
		, Right(Right)
		, Bottom(Bottom)
	{
	}

	SUIMargin(uint TopBottom, uint LeftRight)
		: Left(LeftRight)
		, Top(TopBottom)
		, Right(LeftRight)
		, Bottom(TopBottom)
	{
	}

	SUIMargin(uint Margin)
		: Left(Margin)
		, Top(Margin)
		, Right(Margin)
		, Bottom(Margin)
	{
	}

	intrect CuttedOut(intrect Rectangle) const
	{
		int2 Position = Rectangle.Position - int2(Left, Top);
		int2 Size = Rectangle.Size - int2(Left + Right, Top + Bottom);
		return intrect(Position, Size);
	}
};

template<typename... TArgs>
using SUIEvent = function<void(TArgs... Args)>;

struct SUIWinAPIMessage
{
	uint Message;
	WPARAM WParameter;
	LPARAM LParameter;

	wchar_t GetChar() const
	{
		return WParameter;
	}

	int2 GetInt2() const
	{
		return int2(LParameter & 0xffff, (LParameter >> 16) & 0xffff);
	}

	int GetWheelDelta() const
	{
		return GET_WHEEL_DELTA_WPARAM(WParameter) / WHEEL_DELTA;
	}
};

struct SUIActiveColor
{
	EUIColorType Normal;
	EUIColorType Cursor;
	EUIColorType Selected;

	SUIActiveColor() {}

	SUIActiveColor(EUIColorType Normal, EUIColorType Cursor, EUIColorType Selected)
		: Normal(Normal)
		, Cursor(Cursor)
		, Selected(Selected)
	{
	}
};

class FUIControl
{
	friend class FUIContainer;
	friend class FUIGrid;

private:
	intrect Area;

protected:
	class FUIWindow* Window;
	class FUIContainer* Parent;
	class FUIControl* BindControl;
	bool MouseButtonStates[3];
	FRManager* Renderer;

	uint XIndex;
	uint YIndex;
	SUIMargin Margin;
	EUIColorType BackColor;

protected:
	virtual void OnInitialize(FUIApplication* Application) {}
	virtual void OnWindowChanged() {}
	virtual void OnAreaChanged() {}	
	virtual void OnRendering() {}
	virtual void OnKeyDown(wchar_t Key) {}
	virtual void OnKeyUp(wchar_t Key) {}
	virtual void OnCharInputed(wchar_t Char) {}
	virtual void OnDoubleClick(int2 MousePosition) {}
	virtual void OnMouseDown(EMouseButton Button, int2 MousePosition) {}
	virtual void OnMouseUp(EMouseButton Button, int2 MousePosition) {}
	virtual void OnMouseLeave(int2 MousePosition) {}
	virtual void OnMouseMove(int2 MousePosition) {}
	virtual void OnMouseWheel(int Delta, int2 MousePosition) {}
	virtual void OnGetFocus() {}
	virtual void OnLostFocus() {}
	virtual void OnTimer() {}
	virtual uint GetActualHeight() const { return 100; }
	virtual uint GetActualWidth() const { return 100; }
	int2 GetRenderingSize() const;
	int2 GetRenderingPosition() const;
	intrect GetRenderingArea() const { return intrect(GetRenderingPosition(), GetRenderingSize()); }

	void ChangeCursor(LPCWSTR CursorType);
	void RegisterTimer(uint Milliseconds);
	bool GetMouseButtonDown(EMouseButton Button) const { return MouseButtonStates[(int)Button]; }

public:
	FUIControl();

	// これらの関数を呼び出すとOn～のイベント関数が呼び出される
	virtual void Initialize(FUIApplication* Application);
	virtual void SetWindow(class FUIWindow* Window);
	virtual void SetArea(intrect Area);
	virtual void ReceiveMessage(struct SUIWinAPIMessage const& Message);
	virtual void Update();
	uint GetHeight() const { return GetActualHeight() + Margin.Top + Margin.Bottom; }
	uint GetWidth() const { return GetActualWidth() + Margin.Left + Margin.Right; }

	intrect GetArea() const { return Area; }
	class FUIWindow* GetWindow() const { return Window; }
	class FUIContainer* GetParent() const { return Parent; }

	void SetXIndex(uint InXIndex) { XIndex = InXIndex; }
	void SetYIndex(uint InYIndex) { YIndex = InYIndex; }
	void SetMargin(SUIMargin InMargin) { Margin = InMargin; }
	void SetBackColor(EUIColorType InBackColor) { BackColor = InBackColor; }

	void SetBindControl(FUIControl* InBindControl) { BindControl = InBindControl; }
	FUIControl* GetBindControl() { return BindControl; }
};