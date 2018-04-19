#pragma once
#include "..\UIControl.h"

class FUIContainer : public FUIControl
{
protected:
	FUIControl* FocusedChild;
	FUIControl* InRangeChild;
	vector<unique_ptr<FUIControl>> Children;

private:
	void CallMouseDownEvent(EMouseButton Button, int2 MousePosition);
	void OnRendering() override final {}

public:
	void Initialize(FUIApplication* Application) override;
	void SetWindow(FUIWindow* Window) override;
	void ReceiveMessage(SUIWinAPIMessage const& Message) override;
	void Update() override;

public:
	FUIContainer();
	FUIContainer(FUIContainer&) = delete;

	void UpdateArea();
	void AddChild(FUIControl* Child);
};

template<typename T>
class FUIFrame
{
protected:
	T* Content;

public:
	FUIFrame() : Content(nullptr) {}
	
	void SetContent(T* InContent) { Content = InContent; }
};