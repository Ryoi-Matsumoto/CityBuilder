#include "WorldEditor.h"
#include "World\Object\Corner.h"
#include "World\Object\Crosswalk.h"
#include "World\Object\Street.h"
#include "World\Object\Intersection.h"

const float CameraMoveAmountByKey = 0.01f;
const float CameraMoveAmountByMouse = 0.005f;
const float CameraMoveYAmount = 0.001f;
const float CameraRotateAmount = 0.0025f;
const float3 DefaltCameraPosition = float3(0, 5.0f, 0);
const float LenghtJointStreet = 10;

FWorldEditor::FWorldEditor()
	:CameraPosition(0, 50, 0), 
	CameraRotationVertical(0),
	CameraRotationHorizontal(0),
	PrevMouseRightButton(false),
	PrevMouseMiddleButton(false)
{
	World = new FWorld();
	Grid = new FGrid(100, 1000, float4(0,0,0,1));

	SFontType FontType;
	FontType.Size = 16;
	TextBlock = new FUITextBlock(FontType);
	TextBlock->SetPosition(int2(10, 0));
	AddChild(TextBlock);

	OnSizeChanged += [&](int2 Size)
	{
		ProjectionMatrix = Matrix::PerspectiveFov(45.0f / 180.0f * PI, Size.X / Size.Y, 1, 1000);
		Update();
	};

	OnRendering += [&]()
	{
		TextBlock->SetAsLabel
		(
			wstring(L"Pos(")
			+ L"X:"
			+ to_wstring(CameraPosition.X) 
			+ L" Y:" 
			+ to_wstring(CameraPosition.Y)
			+ L" Z:" 
			+ to_wstring(CameraPosition.Z)
			+ L" Rot(H:"
			+ to_wstring(CameraRotationHorizontal)
			+ L" V:"
			+ to_wstring(CameraRotationVertical)
			+ L")"
		);
		TextBlock->Update();

		ViewMatrix = Matrix::LookRotation(CameraPosition, CameraRotationVertical, CameraRotationHorizontal, 0);
		Grid->SetWVPMatrix(ViewMatrix * ProjectionMatrix);

		World->Render();
		Grid->Render();
	};

	OnKeyDown += [&](wchar_t Key)
	{
		float MoveAmount = CameraMoveAmountByKey * fabs(CameraPosition.Y);

		float3 MoveVector(0);
		switch (Key)
		{
		case L'W':
			MoveVector.Z -= MoveAmount;
			break;
		case L'S':
			MoveVector.Z += MoveAmount;
			break;
		case L'A':
			MoveVector.X -= MoveAmount;
			break;
		case L'D':
			MoveVector.X += MoveAmount;
			break;
		case L'E':
			MoveVector.Y -= MoveAmount;
			break;
		case L'Q':
			MoveVector.Y += MoveAmount;
			break;
		case L'R':
			MoveVector = DefaltCameraPosition;
			break;
		}
		CameraPosition += MoveVector.Transformed(Matrix::RotationY(CameraRotationHorizontal));
		Update();
	};

	OnMouseDown += [&](EMouseButton Button, int2 MousePosition)
	{
		if (Button == EMouseButton::Right)
		{
			PrevMouseRightButton = true;
			PrevOnMouseRightButtonPressed = MousePosition;
		}
		if (Button == EMouseButton::Middle)
		{
			PrevMouseMiddleButton = true;
			PrevOnMouseMiddleButtonPressed = MousePosition;
		}
	};

	OnMouseUp += [&](EMouseButton Button, int2 MousePosition)
	{
		if (Button == EMouseButton::Right)
		{
			PrevMouseRightButton = false;
		}
		if (Button == EMouseButton::Middle)
		{
			PrevMouseMiddleButton = false;
		}
	};

	OnMouseMove += [&](int2 MousePosition)
	{
		if (PrevMouseRightButton)
		{
			auto Move = MousePosition - PrevOnMouseRightButtonPressed;
			CameraRotationHorizontal -= Move.X * CameraRotateAmount;
			CameraRotationVertical -= Move.Y * CameraRotateAmount;
			PrevOnMouseRightButtonPressed = MousePosition;
			Update();
		}

		if (PrevMouseMiddleButton)
		{
			float MoveAmount = CameraMoveAmountByMouse * fabs(CameraPosition.Y);
			auto Move2D = MousePosition - PrevOnMouseMiddleButtonPressed;
			auto Move3D = float3(-Move2D.X * MoveAmount, 0, Move2D.Y * MoveAmount);
			CameraPosition += Move3D.Transformed(Matrix::RotationY(CameraRotationHorizontal));
			PrevOnMouseMiddleButtonPressed = MousePosition;
			Update();
		}
	};

	OnMouseLeave += [&](int2 MousePosition)
	{
		PrevMouseRightButton = false;
		PrevMouseMiddleButton = false;
	};

	OnMouseWheel += [&](int MouseWheel)
	{
		float MoveAmount = CameraMoveYAmount * fabs(CameraPosition.Y);
		CameraPosition += float3(0, -MouseWheel * MoveAmount, 0);
		Update();
	};
}

void FWorldEditor::AddStreet(Vector2 StartPoint, Vector2 EndPoint)
{
	// Street‚ð’Ç‰Á
	auto NewStreet = new FDesignStreet(SLine(StartPoint, EndPoint), SLine(EndPoint, StartPoint));
	World->AddStreet(NewStreet);

	Vector2 Points[2] = { StartPoint, EndPoint };	

	for (int Side = 0; Side < 2; Side++)
	{
		// Ú‘±‰Â”\‚ÈIntersection‚ð’T‚·
		FDesignIntersection* HitIntersection = nullptr;

		for (auto Intersection : World->GetIntersections())
		{
			bool IsHited = false;

			for (auto Corner : Intersection->GetCorners())
			{
				auto CornerRepresentative = Corner->GetCentor();
				if ((CornerRepresentative - Points[Side]).Lenght() < LenghtJointStreet)
				{
					HitIntersection = Intersection;
					IsHited = true;
					break;
				}
			}

			if (IsHited)break;
		}

		// Šù‘¶‚ÌIntersection‚ÉÚ‘±
		if (HitIntersection)
		{
			HitIntersection->AddStreet(NewStreet->GetConnectedStreet(Side));
		}
		// V‚½‚ÈIntersection‚ð¶¬
		else
		{ 
			auto NewIntersection = new FDesignIntersection(World);
			NewIntersection->AddStreet(NewStreet->GetConnectedStreet(Side));
			World->AddIntersection(NewIntersection);
		}
	}
}