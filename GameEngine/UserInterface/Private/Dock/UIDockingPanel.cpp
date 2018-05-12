#include "UIDockingPanel.h"
#include "UIUpperTab.h"

void FUIDockingPanel::OnInitialize()
{
	MainArea = new FUIUpperTab();
	MainArea->SetParentPanel(this);
	SetOnlyChild(MainArea);

	for (auto& Item : Items)
		MainArea->AddItem(&Item);
}

void FUIDockingPanel::AddItem(FUIControl* Control, wstring Title)
{
	auto Item = SUIDockingItem();
	Item.Control = Control;
	Item.Title = Title;
	Items.push_back(move(Item));

	if (GetIsInitialized())
	{
		MainArea->AddItem(&Item);
		Item.Control->Initialize(Application);
		Update();
	}
}

void FUIDockingPanel::DragItem(SUIDockingItem* Item)
{
	auto WindowArea = GetWindow()->GetWindowArea();
	SUIWindowStartupState State;
	State.Title = Item->Title;
	State.Area = Item->Control->GetArea();
	State.Area.Position += WindowArea.Position;
	Application->CreateUIWindow(Item->Control, State);
}