#include "camera.h"
#include <algorithm>

using namespace DirectX;

Camera::Camera()
	: _position{XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)}, _up{XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)}, _zoom{10.0f}, _minZoom{5.0f},
	  _maxZoom{15.0f}, _angle{XM_PIDIV4}, _speed{0.15f}, _lastMousePos{0, 0}
{
	XMVECTOR baseForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMMATRIX rotPitch = XMMatrixRotationX(_angle);
	XMMATRIX rotYaw = XMMatrixRotationY(XM_PIDIV4);
	XMMATRIX combinedRot = rotPitch * rotYaw;
	_forward = XMVector3TransformNormal(baseForward, combinedRot);
}

XMMATRIX Camera::LookAt() const
{
	XMVECTOR eyePos = _position - (_forward * _zoom);
	return XMMatrixLookAtLH(eyePos, _position, _up);
}

void Camera::HandleMouse(HWND hwnd)
{
	if (GetFocus() != hwnd)
		return;

	RECT rect;
	GetClientRect(hwnd, &rect);
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);
	ScreenToClient(hwnd, &cursorPosition);

	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
		float dx = static_cast<float>(cursorPosition.x - _lastMousePos.x);
		float sensitivity = 0.005f;
		XMMATRIX rotY = XMMatrixRotationY(dx * sensitivity);
		_forward = XMVector3TransformNormal(_forward, rotY);
		_forward = XMVector3Normalize(_forward);
	} else {
		const int edgeSize = 30;
		const int width = rect.right - rect.left;
		const int height = rect.bottom - rect.top;

		XMVECTOR normForward = XMVector3Normalize(XMVectorSet(XMVectorGetX(_forward), 0.0f, XMVectorGetZ(_forward), 0.0f));
		XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR normRight = XMVector3Cross(worldUp, normForward);

		if (cursorPosition.x < edgeSize) {
			_position -= normRight * _speed;
		} else if (cursorPosition.x > width - edgeSize) {
			_position += normRight * _speed;
		}

		if (cursorPosition.y < edgeSize) {
			_position += normForward * _speed;
		} else if (cursorPosition.y > height - edgeSize) {
			_position -= normForward * _speed;
		}
	}

	_lastMousePos = cursorPosition;
}

void Camera::HandleZoom(short wheelDelta)
{
	const float zoomSensitivity = 2.0f;
	_zoom -= (static_cast<float>(wheelDelta) / WHEEL_DELTA) * zoomSensitivity;
	_zoom = std::clamp(_zoom, _minZoom, _maxZoom);
}