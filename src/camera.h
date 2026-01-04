#pragma once
#include <DirectXMath.h>
#include <windows.h>

class Camera {
  public:
	Camera();
	DirectX::XMMATRIX LookAt() const;
	void HandleMouse(HWND hwnd);
	void HandleZoom(short delta);
	
	inline float GetAngle() { return _angle; }
	inline void SetTarget(DirectX::FXMVECTOR pos) { _position = pos; }

  private:
	DirectX::XMVECTOR _position;
	DirectX::XMVECTOR _forward;
	DirectX::XMVECTOR _up;

	float _speed;
	float _zoom;
	float _minZoom;
	float _maxZoom;
	float _angle;
	float _moveSensitivity;
	float _zoomSensitivity;
	int _edgeSize;

	POINT _lastMousePos;
};