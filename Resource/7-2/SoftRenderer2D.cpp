
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
Vector2 lightPosition(200.f, 0.f);
LinearColor lightColor;
Vector2 circlePosition;

// ���� ������ ����ϴ� �Լ�
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� ������ ���� ����
	static float duration = 20.f;
	static float elapsedTime = 0.f;
	static float currentDegree = 0.f;
	static float lightDistance = 200.f;
	static HSVColor lightHSVColor;

	// ��� �ð��� ���� ���� ���� �̸� ����� [0,1]���� ����
	elapsedTime += InDeltaSeconds;
	elapsedTime = Math::FMod(elapsedTime, duration);
	float currentRad = (elapsedTime / duration) * Math::TwoPI;
	float alpha = (sinf(currentRad) + 1) * 0.5f;

	// [0,1]�� Ȱ���� �ֱ������� ũ�⸦ �ݺ��ϱ�
	currentDegree = Math::Lerp(0.f, 360.f, alpha);


	// ������ ��ǥ�� ����
	float sin = 0.f, cos = 0.f;
	Math::GetSinCosRad(sin, cos, currentRad);
	lightPosition = Vector2(cos, sin) * lightDistance;

	lightHSVColor.H = currentRad * Math::InvPI * 0.5f;
	lightColor = lightHSVColor.ToLinearColor();


}

// ������ ������ ����ϴ� �Լ�
void SoftRenderer::Render2D()
{
	// ������ �������� ����ϴ� ��� �� �ֿ� ���۷���
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ������ ������ ���� ����
	static std::vector<Vector2> light;
	static float lightRadius = 10.f;
	static std::vector<Vector2> circle;
	static float circleRadius = 50.f;

	// ������ ǥ���ϴ� ��ü
	if (light.empty())
	{
		float lightRadius = 10.f;
		for (float x = -lightRadius; x <= lightRadius; ++x)
		{
			for (float y = -lightRadius; y <= lightRadius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = lightRadius * lightRadius;
				if (sizeSquared < rr)
				{
					light.push_back(target);
				}
			}
		}
	}

	// ���� �޴� ��ü
	if (circle.empty())
	{
		for (float x = -circleRadius; x <= circleRadius; ++x)
		{
			for (float y = -circleRadius; y <= circleRadius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = circleRadius * circleRadius;
				if (sizeSquared < rr)
				{
					circle.push_back(target);
				}
			}
		}
	}

	// ���� �׸���
	static float lightLineLength = 50.f;
	r.DrawLine(lightPosition, lightPosition - lightPosition.GetNormalize() * lightLineLength, lightColor);
	for (auto const& v : light)
	{
		r.DrawPoint(v + lightPosition, lightColor);
	}


	// ������ �޴� ��ü�� ��� �ȼ��� NdotL�� ����� ������ �����ϰ� �̸� ���� ���� �ݿ�
	for (auto const& v : circle)
	{
		Vector2 n = (v - circlePosition).GetNormalize();
		Vector2 l = (lightPosition - v).GetNormalize();

		float shading = Math::Clamp(n.Dot(l), 0.f, 1.f);
		r.DrawPoint(v, lightColor * shading);
	}


	// ���� ������ ��ġ�� ȭ�鿡 ���
	r.PushStatisticText(std::string("Position : ") + lightPosition.ToString());
}

// �޽ø� �׸��� �Լ�
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// �ﰢ���� �׸��� �Լ�
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
