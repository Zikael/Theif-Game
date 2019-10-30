#include "Character.h"

#include <TL-Engine.h>

#include "CMatrix4x4cut.h"
#include "CVector3cut.h"

#include <iostream>

Character::Character(tle::IMesh* mesh, float moveSpeed, float x, float y, float z)
{
	pMoveSpeed = moveSpeed;
	pModel = mesh->CreateModel(x, y, z);
}

Character::~Character() { /* Empty */ }

tle::IModel* Character::getModel() noexcept
{
	return pModel;
}

void Character::setMoveSpeed(float newSpeed) noexcept
{
	pMoveSpeed = newSpeed;
}

float Character::getMoveSpeed()
{
	return pMoveSpeed;
}

float Character::getDistance(tle::IModel* mod)
{

	const float x = pModel->GetLocalX() - mod->GetX();
	const float y = pModel->GetLocalY() - mod->GetY();
	const float z = pModel->GetLocalZ() - mod->GetZ();

	return sqrt(x * x + y * y + z * z);
}

void Character::facingVector(float& x, float& y, float& z)
{
	float matrix[16];

	pModel->GetMatrix(matrix);

	x = matrix[8];
	y = matrix[9];
	z = matrix[10];
}

bool Character::isFacing(tle::IModel* mod)
{
	float vx, vy, vz;
	facingVector(vx, vy, vz);

	const float wx = mod->GetLocalX() - pModel->GetLocalX();
	const float wy = mod->GetLocalY() - pModel->GetLocalY();
	const float wz = mod->GetLocalZ() - pModel->GetLocalZ();

	return ((vx * wx) + (vy * wy) + (vz * wz)) > 0;
}

void Character::turnTowards(tle::IModel* mod)
{
	// Get the characters facing vector
	float vx, vy, vz;
	facingVector(vx, vy, vz);
	CVector3 facingV(vx, vy, vz);

	// Get the world Y axis
	float pMatrix[16];
	pModel->GetMatrix(pMatrix);
	CVector3 worldY(pMatrix[1], pMatrix[5], pMatrix[9]);


	// Get the vector between character and target
	float modMatrix[16];
	mod->GetMatrix(modMatrix);
	CVector3 pPosition, modPosition;
	pPosition.Set(&pMatrix[12]);
	modPosition.Set(&modMatrix[12]);
	CVector3 direction = Normalise(Subtract(modPosition, pPosition));

	// Calculate local x-axis
	CVector3 localX = Cross(worldY, facingV);

	// Test for left/right
	const float swingDirection = Dot(direction, localX);

	if (swingDirection > 0.f)
	{
		// Right
		pModel->RotateLocalY(pRotateSpeed);
	}
	else
	{
		// Left
		pModel->RotateLocalY(-pRotateSpeed);
	}
}

void Character::turnTowards(float x, float y, float z)
{
	// Get the characters facing vector
	float vx, vy, vz;
	facingVector(vx, vy, vz);
	CVector3 facingV(vx, vy, vz);

	// Get the world Y axis
	float pMatrix[16];
	pModel->GetMatrix(pMatrix);
	CVector3 worldY(pMatrix[1], pMatrix[5], pMatrix[9]);


	// Get the vector between character and target
	CVector3 pPosition, nPosition(x, y, z);
	pPosition.Set(&pMatrix[12]);
	CVector3 direction = Normalise(Subtract(nPosition, pPosition));

	// Calculate local x-axis
	CVector3 localX = Cross(worldY, facingV);

	// Test for left/right
	const float swingDirection = Dot(direction, localX);

	if (swingDirection > 0.f)
	{
		// Right
		pModel->RotateLocalY(pRotateSpeed);
	}
	else
	{
		// Left
		pModel->RotateLocalY(-pRotateSpeed);
	}
}

void Character::lookTowards(tle::IModel* mod)
{
	float pMatrix[16], modMatrix[16];
	pModel->GetMatrix(pMatrix);
	mod->GetMatrix(modMatrix);

	CVector3 pPosition, modPosition;

	pPosition.Set(&pMatrix[12]);
	modPosition.Set(&modMatrix[12]);

	// Calculate matrix axes for pModel
	// Get facing (z) vector from positions
	CVector3 vecZ = Normalise(Subtract(modPosition, pPosition));

	// Use cross products to get other axes
	// Must normalise axes
	CVector3 vecX = Normalise(Cross(kYAxis, vecZ));

	//CVector3 vecY = Normalise(Cross(vecZ, vecX));
	CVector3 vecY = Cross(vecZ, vecX);

	// Build matrix from axes + position
	CMatrix4x4 pMat;
	pMat.MakeIdentity();
	pMat.SetRow(0, vecX);
	pMat.SetRow(1, vecY);
	pMat.SetRow(2, vecZ);
	pMat.SetRow(3, pPosition);

	// Set position of guard using matrix
	pModel->SetMatrix(&pMat.e00);
}

void Character::lookTowards(float x, float y, float z)
{
	float pMatrix[16];
	pModel->GetMatrix(pMatrix);

	CVector3 nPosition(x, y, z);
	CVector3 pPosition;

	pPosition.Set(&pMatrix[12]);

	// Calculate matrix axes for pModel
	// Get facing (z) vector from positions
	CVector3 vecZ = Normalise(Subtract(nPosition, pPosition));

	// Use cross products to get other axes
	// Must normalise axes
	CVector3 vecX = Normalise(Cross(kYAxis, vecZ));

	//CVector3 vecY = Normalise(Cross(vecZ, vecX));
	CVector3 vecY = Cross(vecZ, vecX);

	// Build matrix from axes + position
	CMatrix4x4 pMat;
	pMat.MakeIdentity();
	pMat.SetRow(0, vecX);
	pMat.SetRow(1, vecY);
	pMat.SetRow(2, vecZ);
	pMat.SetRow(3, pPosition);

	// Set position of guard using matrix
	pModel->SetMatrix(&pMat.e00);
}

bool Character::isDead() noexcept
{
	return pState == stateCollection.DEAD;
}