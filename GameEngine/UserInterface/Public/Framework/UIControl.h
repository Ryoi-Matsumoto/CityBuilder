#pragma once
#include "Rendering\Public\R2DRenderer.h"
#include "Rendering\Public\RViewport.h"
#include "UICore.h"
#include "UIApplication.h"
#include "UIWindow.h"

class FUIWindow;
struct SUIWinAPIMessage;

enum class ECursorType
{
	IBeam
};

struct SUIMargin
{
	uint Left;
	uint Top;
	uint Right;
	uint Bottom;

	SUIMargin(uint Left, uint Top, uint Right, uint Bottom)
		: Left(Left)
		, Top(Top)
		, Right(Right)
		, Bottom(Bottom)
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

class FUIControl;

class FUIControlList
{
public:
	vector<FUIControl*> Controls;

public:
	FUIControlList&& operator +(FUIControl&& Control)
	{
		Controls.push_back(&Control);
		return move(*this);
	}
};

class FUIControl
{
	friend class FUIControlContainer;

protected:
	FUIWindow* Window;
	bool IsMousePressed;
	FRManager* Renderer;
	intrect Area;

protected:
	virtual void OnInitialize(FUIApplication* Application) {}
	virtual void OnWindowChanged() {}
	virtual void OnAreaChanged() {}	
	virtual void OnRendering() {}
	virtual void OnKeyDown(wchar_t Key) {}
	virtual void OnKeyUp(wchar_t Key) {}
	virtual void OnCharInputed(wchar_t Char) {}
	virtual void OnMouseDown(EMouseButton Button, int2 MousePosition) {}
	virtual void OnMouseUp(EMouseButton Button, int2 MousePosition) {}
	virtual void OnMouseLeave(int2 MousePosition) {}
	virtual void OnMouseMove(int2 MousePosition) {}
	virtual void OnMouseWheel(int Delta, int2 MousePosition) {}
	virtual void OnGetFocus() {}
	virtual void OnLostFocus() {}
	virtual void OnTimer() {}

	void ChangeCursor(ECursorType CursorType);
	void RegisterTimer(uint Milliseconds);
	void RegisterTrackMouseLeave();
	bool GetIsPressed() const { return IsMousePressed; }

public:
	FUIControl();

	// これらの関数を呼び出すとOn～のイベント関数が呼び出される
	virtual void Initialize(FUIApplication* Application);
	virtual void SetWindow(FUIWindow* Window);
	virtual void SetArea(intrect Area);
	virtual void ReceiveMessage(SUIWinAPIMessage const& Message);
	virtual void Update();

	intrect GetArea() const { return Area; }
	FUIWindow* GetWindow() const { return Window; }
	virtual uint GetHeight() const { return 100; }
	virtual uint GetWidth() const { return 100; }

	UIAttribute(uint, XIndex)
	UIAttribute(uint, YIndex)
	UIAttribute(SUIMargin, Margin)
	UIAttribute(float4, BackColor)

	FUIControlList&& operator +(FUIControl&& Control)
	{
		auto List = new FUIControlList();
		List->Controls.push_back(this);
		List->Controls.push_back(&Control);
		return move(*List);
	}
};

class FUIControlContainer : public FUIControl
{
protected:
	FUIControl* FocusedContent;
	vector<FUIControl*> Contents;

private:	
	void CallMouseDownEvent(EMouseButton Button, int2 MousePosition);

public:
	void Initialize(FUIApplication* Application) override;
	void SetWindow(FUIWindow* Window) override;
	void SetArea(intrect Area) override;
	void ReceiveMessage(SUIWinAPIMessage const& Message) override;
	void Update() override;
	
public:	
	FUIControlContainer();
	FUIControlContainer(FUIControlContainer&) = delete;
	~FUIControlContainer() { for (auto Content : Contents)delete Content; }

	UIAttributeOverride(uint, XIndex)
	UIAttributeOverride(uint, YIndex)
	UIAttributeOverride(SUIMargin, Margin)
	UIAttributeOverride(float4, BackColor)

	auto&& operator [](FUIControlList&& ContentList)
	{										
		for (auto Content : ContentList.Controls)
		{
			Contents.emplace_back(Content);
		}	
		return move(*this);								
	}

	auto&& operator [](FUIControl&& Content)
	{
		Contents.emplace_back(&Content);
		return move(*this);
	}

	template<typename ControlType>
	void AddContent(ControlType* Content)
	{
		Contents.emplace_back(Content);
	}
};