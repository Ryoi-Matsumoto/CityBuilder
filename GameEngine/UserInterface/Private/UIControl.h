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

template<typename T>
class FUIFrame
{
protected:
	T * Content;

public:
	FUIFrame() : Content(nullptr) {}

	void SetContent(T* InContent) { Content = InContent; }
};

class FUIControl
{
	friend class FUIGrid;

private:
	FUIControl* FocusedChild;
	FUIControl* InRangeChild;
	intrect Area;
	bool IsInitialized;
	bool IsAreaSetted;
	bool IsVisible;

protected:
	class FUIApplication* Application;
	class FUIControl* Parent;
	class FUIControl* BindControl;
	class FUIWindow* Window;
	bool MouseButtonStates[3];
	vector<unique_ptr<FUIControl>> Children;

	uint XIndex;
	uint YIndex;
	SUIMargin Margin;
	EUIColorType BackColor;

private:
	FUIControl* GetChildInRange(int2 Position);

protected:
	virtual void OnInitialize() {}
	virtual void OnWindowChanged() {}
	virtual void OnAreaChanged() { for (auto& Child : Children) Child->SetArea(GetRenderingArea()); }
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
	virtual uint GetActualHeight() const;
	virtual uint GetActualWidth() const;
	virtual bool GetIsClientArea() const { return true; }
	int2 GetRenderingSize() const;
	int2 GetRenderingPosition() const;
	intrect GetRenderingArea() const { return intrect(GetRenderingPosition(), GetRenderingSize()); }
	FRManager* GetRenderer() const { return Application->GetRenderingManager(); }

	void ChangeCursor(LPCWSTR CursorType);
	void RegisterTimer(uint Milliseconds);
	bool GetMouseButtonDown(EMouseButton Button) const { return MouseButtonStates[(int)Button]; }
	void ResetMouseButtonDown() const { memset((void*)MouseButtonStates, 0, sizeof(bool) * 3); }

public:
	FUIControl();

	// これらの関数を呼び出すとOn～のイベント関数が呼び出される
	void Initialize(FUIApplication* Application);
	void SetArea(intrect Area);
	LRESULT ReceiveMessage(struct SUIWinAPIMessage const& Message);
	void Update();
	void UpdateArea();

	intrect GetArea() const { return Area; }
	uint GetHeight() const { return GetActualHeight() + Margin.Top + Margin.Bottom; }
	uint GetWidth() const { return GetActualWidth() + Margin.Left + Margin.Right; }
	const vector<unique_ptr<FUIControl>>& GetChildren() const { return Children; }

	bool GetIsInitialized() const { return IsInitialized; }
	//intrect GetArea() const { return Area; }
	class FUIControl* GetParent() const { return Parent; }
	class FUIWindow* GetWindow() const { return Window ? Window : (Parent ? Parent->GetWindow() : nullptr); }

	void AddChild(FUIControl* Child);
	void RemoveChild(FUIControl* Child);
	void SetOnlyChild(FUIControl* Child);

	void SetWindow(class FUIWindow* InWindow) { Window = InWindow; }
	void SetXIndex(uint InXIndex) { XIndex = InXIndex; }
	void SetYIndex(uint InYIndex) { YIndex = InYIndex; }
	void SetMargin(SUIMargin InMargin) { Margin = InMargin; }
	void SetBackColor(EUIColorType InBackColor) { BackColor = InBackColor; }
	void SetIsVisible(bool InIsVisible) { IsVisible = InIsVisible; }

	void SetBindControl(FUIControl* InBindControl) { BindControl = InBindControl; }
	FUIControl* GetBindControl() const { return BindControl; }
	FUIApplication* GetApplication() const { return Application; }
};