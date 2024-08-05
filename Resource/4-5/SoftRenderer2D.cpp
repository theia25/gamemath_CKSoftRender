
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// ���ڸ� �׸��� �Լ�
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// ���� ������Ʈ ���


// ���� �� �ε��� ����ϴ� �Լ�
void SoftRenderer::LoadScene2D()
{
	// ���� �� �ε����� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();

}

// ���� ������ ������ ������ �����ϴ� ����
float currentDegree = 0.f;

// ���� ������ ����ϴ� �Լ�
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� ������ ���� ����
	static float rotateSpeed = 180.f;

	float deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;

	// ��ü�� ���� ���� ����
	currentDegree += deltaDegree;
}

// ������ ������ ����ϴ� �Լ�
void SoftRenderer::Render2D()
{
	// ������ �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ��濡 ���� �׸���
	DrawGizmo2D();

	// ������ ������ ���� ����
	static float halfSize = 100.f; // ���簢���� ���� ũ�⸦ ���� halfSize�� ����
	static std::vector<Vector2> squares; // ���簢���� �����ϴ� ���͸� ���� �� �ִ� �����̳� ���� ����

	// �簢���� �����ϴ� ���� ����
	if (squares.empty())
	{
		for (float x = -halfSize; x <= halfSize; x += 0.25f)
		{
			for (float y = -halfSize; y <= halfSize; y += 0.25f)
			{
				squares.push_back(Vector2(x, y));
			}
		}
	}

	// ������ �ش��ϴ� sin�� cos�Լ� ���
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, currentDegree);

	// ���� ȭ���� ũ��κ��� ���̸� ���� ���ؾ� ���ϱ�
	static float maxLength = Vector2(_ScreenSize.X, _ScreenSize.Y).Size() * 0.5f; // ���̿� ���� ȸ������ �ٸ��� �ο��ϱ� ���� ���� ���� ���Ѵ�. ȭ�� �밢�� ũ���� ������ ���� ������ ��� ���� maxLength�� ����

	// �簢�� �׸���
	HSVColor hsv(0.f, 1.f, 0.85f);
	for (auto const& v : squares)
	{
		// ����ǥ��� ����
		Vector2 polarV = v.ToPolarCoordinate(); // �簢���� �����ϴ� ���� ���� ����ǥ��� ����

		if (polarV.Y < 0.f)
		{
			polarV.Y += Math::TwoPI; // ����ǥ�� ���� [-pi, pi]������ ������. �̸� [0, 2pi]������ �����ϱ� ���� 2pi�� �����ش�.
		}
		hsv.H = polarV.Y / Math::TwoPI; // ����ǥ�� ���� [0, 1]�� ����ȭ�Ͽ� ������ ����

		// ����ǥ���� ũ�� �����κ��� ȸ������ �����Ѵ�.
		float ratio = polarV.X / maxLength; 
		float weight = Math::Lerp(1.f, 5.f, ratio);

		// ����ǥ�踦 ����� ȸ���� �ο�
		polarV.Y += Math::Deg2Rad(currentDegree) * weight; // ȸ������ ����ġ weight�� ���� ���� ȸ������ ����� ȸ���� �ο��Ѵ�.

		// ���� ���� ��ī��Ʈ ��ǥ��� ��ȯ�Ѵ�.
		Vector2 cartesianV = polarV.ToCartesianCoordinate();
		r.DrawPoint(cartesianV, hsv.ToLinearColor());
	}

	// ���� ������ ȭ�鿡 ���
	r.PushStatisticText(std::string("Degree : ") + std::to_string(currentDegree));
}

// �޽ø� �׸��� �Լ�
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// �ﰢ���� �׸��� �Լ�
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
