#include "stdafx.h"
#include "pSystem.h"
#include <cstdlib>

using namespace psys;
const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

PSystem::PSystem()
{
	_device = 0;
	_vb = 0;
	_tex = 0;
}

PSystem::~PSystem()
{
	d3d::Release(_vb);
	d3d::Release(_tex);
}
bool PSystem::init(IDirect3DDevice9* device, wchar_t* texFileName)
{
	_device = device;

	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
		_vbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC |
		D3DUSAGE_POINTS
		| D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT,
		&_vb, 0);

	if(FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexBuffer() -FAILED",
			L"PSystem", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		device,
		texFileName,
		&_tex);

	if(FAILED(hr))
	{
		::MessageBox(0, L"D3DXCreateTextureFromFile()\
			-FAILED", L"PSystem",0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(device,
		texFileName,
		&_tex);

	if(FAILED(hr))
	{
		::MessageBox(0, L"D3DXCreateTextureFromFile()\
- FAILED", L"PSystem", 0);
		
	}
}

void PSystem::resetParticle(Attribute* attribute)
{
}

void PSystem::reset()
{
	std::list<Attribute>::iterator i;
	for(i=_particles.begin() ;i!=_particles.end();++i)
	{
		resetParticle(&(*i));
	}
}

void PSystem::addParticle()
{
	Attribute attribute;
	resetParticle(&attribute);

	_particles.push_back(attribute);
}

void PSystem::update(float timeDelta)
{
}

void PSystem::preRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, false);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	_device->SetRenderState(D3DRS_POINTSIZE, d3d::FtoDw(_size));
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, d3d::FtoDw(0.0f));

	// control the size of the particle relative to distance
	_device->SetRenderState(D3DRS_POINTSCALE_A, d3d::FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_B, d3d::FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_C, d3d::FtoDw(1.0f));

	// use alpha from texture
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void PSystem::render()
{
	if(!_particles.empty())
	{
		preRender();

		_device->SetTexture(0, _tex);
		_device->SetFVF(Particle::FVF);
		_device->SetStreamSource(0, _vb, 0, sizeof(Particle));

		//// ���ؽ� ���۰� ������ �� �� �ִ� ��ƼŬ�� ��
		//DWORD _vbSize;
		//// ���ؽ� ���ۿ��� ���縦 ������ ��ƼŬ ��
		//// ���� �ܰ���� ������ (����Ʈ�� �ƴ� ��ƼŬ ����)
		//// �ܰ� 1�� 0���� 499 ��� �ܰ�2���� �������� 500�� �ȴ�.
		//DWORD _vbOffset;
		//// �ϳ��� �ܰ迡 ���ǵ� ��ƼŬ�� ��
		//DWORD _vbBatchSize;

		// �������� �� ��ƼŬ�� ���� �Ѿ��ٸ� �������� �ʱ�ȭ
		if (_vbOffset >= _vbSize)
			_vbOffset = 0;
		
		Particle* v = 0;
		
		_vb->Lock(_vbOffset * sizeof(Particle),
			_vbBatchSize * sizeof(Particle),
			(void**)&v,
			// ���� �����ӿ����� �������� Offset �� �����ϹǷ� ���ؽ� ���۸� ä���� ��.
			_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		std::list<Attribute>::iterator i;
		for(i = _particles.begin(); i != _particles.end();++i)
		{
			if(i->_isAlive)
			{
				v->_position = i->_position;
				v->_color = (D3DCOLOR)i->_color;
				v++;

				numParticlesInBatch++;

				if(numParticlesInBatch==_vbBatchSize)
				{
					_vb->Unlock();

					_device->DrawPrimitive(D3DPT_POINTLIST,
						_vbOffset,
						_vbBatchSize);

					_vbOffset += _vbBatchSize;

					if (_vbOffset >= _vbSize)
						_vbOffset = 0;

					_vb->Lock(
						_vbOffset * sizeof(Particle),
						_vbBatchSize * sizeof(Particle),
						(void**)&v,
						_vbOffset ? 
						D3DLOCK_NOOVERWRITE 
								: 
						D3DLOCK_DISCARD);

					numParticlesInBatch = 0;
				}				
			}
		}
		
		_vb->Unlock();

		if(numParticlesInBatch)
		{
			_device->DrawPrimitive(
				D3DPT_POINTLIST,
				_vbOffset,
				numParticlesInBatch);
		}

		_vbOffset += _vbBatchSize;

		postRender();
	}
}

void PSystem::postRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	
}

bool PSystem::isEmpty()
{
	return _particles.empty();
}

bool PSystem::isDead()
{
	std::list<Attribute>::iterator i;
	for(i=_particles.begin();i!=_particles.end();++i)
	{
		if (i->_isAlive)
			return false;
	}
	return true;
}

void PSystem::removeDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = _particles.begin();

	while(i!=_particles.end())
	{
		if(i->_isAlive==false)
		{
			i = _particles.erase(i);
		}
		else
		{
			i++; 
		}
	}
}

Snow::Snow(d3d::BoundingBox* boundingBox, int numParticles)
{
	_boundingBox = *boundingBox;
	
	_size = 0.25f;
	
	_vbSize = 2048;
	
	_vbOffset = 0;
	
	_vbBatchSize = 512;

	for (int i = 0; i < numParticles; ++i)
		addParticle();
}

void Snow::resetParticle(Attribute* attribute)
{
	attribute->_isAlive = true;

	d3d::GetRandomVector(
		&attribute->_position,
		&_boundingBox._min,
		&_boundingBox._max);

	attribute->_position.y = _boundingBox._max.y;

	attribute->_velocity.x = d3d::GetRandomFloat(0,
		1) * -3.f;
	attribute->_velocity.y = d3d::GetRandomFloat(0, 1) * -10.f;
	attribute->_velocity.z = 0.f;

	attribute->_color = d3d::WHITE;
}

void Snow::update(float timeDelta)
{
	std::list<Attribute>::iterator i;
	
	for(i=_particles.begin();i!=_particles.end();++i)
	{
		i->_position += i->_velocity * timeDelta;

		if(_boundingBox.isPointInside(i->_position)==false)
		{
			resetParticle(&(*i));
		}
	}
}

FireWork::FireWork(_vector* origin, int numParticles)
{
	_origin = *origin;
	_size = 0.9f;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;

	for (int i = 0; i < numParticles; ++i)
		addParticle();
}

void FireWork::resetParticle(Attribute* attribute)
{
	attribute->_isAlive = true;
	attribute->_position = _origin;

	_vector min = _vector(-1, -1, -1);
	_vector max { 1,1,1 };

	d3d::GetRandomVector(
		&attribute->_velocity,
		&min, &max);

	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 100.0f;

	attribute->_color = D3DXCOLOR
	{
		d3d::GetRandomFloat(0,1) ,
	d3d::GetRandomFloat(0, 1),
	d3d::GetRandomFloat(0, 1),
			1.f };

	attribute->_age = 0.0f;
	attribute->_lifetime = 2.f;
}

void FireWork::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i=_particles.begin();i!=_particles.end();++i)
	{
		if(i->_isAlive)
		{
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			if (i->_age > i->_lifetime)
				i->_isAlive = false;
		}
	}
}

void FireWork::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void FireWork::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

ParticleGun::ParticleGun(Camera* camera)
{
	_camera = camera;
	_size = 0.8f;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;

	for (int i = 0; i < 500; ++i);
	
}

void ParticleGun::resetParticle(Attribute* attribute)
{
	attribute->_isAlive = true;

	D3DXVECTOR3 cameraPos = _camera->GetLocation();
	D3DXVECTOR3 cameraDir = 	_camera->GetLook();

	// change to camera position
	attribute->_position = cameraPos;
	attribute->_position.y -= 1.0f; // slightly below camera
							 // so its like we're carrying a gun

	// travels in the direction the camera is looking
	attribute->_velocity = cameraDir * 100.0f;

	// green
	attribute->_color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

	attribute->_age = 0.0f;
	attribute->_lifetime= 1.0f; // lives for 1 seconds
}

void ParticleGun::update(float timeDelta)
{
	std::list<Attribute>::iterator i;
	static float f = 0;
	f += timeDelta;
	if (f >= 0.01f)
	{
		f = -0.01f;
		addParticle();
	}
		
	
	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * timeDelta;

		i->_age += timeDelta;

		if (i->_age > i->_lifetime) // kill 
			i->_isAlive = false;
	}
	removeDeadParticles();
}

