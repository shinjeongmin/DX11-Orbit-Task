#include "Engine.h"
#include "Vector3f.h"

#include <iostream>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR pCmdLine,
	int nCmdShow)
{
	// ��ü.
	Engine engine(hInstance, 1280, 800, L"�׷��Ƚ� ����");

	// �ʱ�ȭ.
	if (engine.Initialize() == false)
	{
		MessageBox(nullptr, L"���� �ʱ�ȭ ����", L"����", 0);
		exit(-1);
	}
	
	//  ����.
	return engine.Run();
}